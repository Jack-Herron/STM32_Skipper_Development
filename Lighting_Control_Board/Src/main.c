#include <stdint.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "indicate.h"
#include "boost.h"
#include "CCS.h"
#include "CAN.h"
#include "ADC.h"
#include "main.h"

#define test_Current 		250
#define HEADROOM_LOW		2.7f
#define HEADROOM_HIGH		3.3f
#define BOOST_STEP_UP       0.1f
#define BOOST_STEP_DOWN     0.1f

uint8_t setup_Complete = 0;

float measure_Vf(uint8_t channel, float current)
{
	float boost_Voltage = 60.0;
	boost___Set_Voltage(boost_Voltage);
	float channel_Voltage = 60.0;
	float gen_Voltage;

	while(channel_Voltage > CCS___OVERHEAD_VOLTAGE + test_Current)
	{
		CCS___Write_Channel(channel, current);
		clock___Delay_ms(125);
		gen_Voltage = ADC___Get_Voltage(ADC___BOOST);
		channel_Voltage = ADC___Get_Voltage(channel);
		CCS___Write_Channel(channel, 0);
		boost_Voltage -= 0.1;
		boost___Set_Voltage(boost_Voltage);
	}
	return(gen_Voltage - channel_Voltage);
}

void TIM5_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;   // Enable TIM5 clock

    TIM3->CR1 = 0;                        // Upcounting, edge-aligned, disabled during setup
    TIM3->PSC = 71;                       // 72 MHz / (71 + 1) = 1 MHz
    TIM3->ARR = 99999;                    // 1 MHz / (99999 + 1) = 10 Hz (100 ms)

    TIM3->CNT = 0;                        // Clear counter

    TIM3->DIER |= TIM_DIER_UIE;           // Enable update interrupt
    TIM3->EGR |= TIM_EGR_UG;              // Load prescaler immediately

    NVIC_EnableIRQ(TIM3_IRQn);            // Enable TIM5 interrupt

    TIM3->CR1 |= TIM_CR1_CEN;             // Start timer
}

void write_Channels(uint8_t* data)
{
	for(uint8_t i = 0; i < 7; i++)
	{
		if(setup_Complete)
		{
			CCS___Write_Channel(i, ((float)data[i] * 2.5f * 0.001f));
		}
	}
}

void RX_Callback(CAN___Receive_TypeDef packet)
{
	switch(packet.ID)
	{
	case 0x100:
		write_Channels(packet.data);
		break;
	}
}

float get_Lowest_CC_Voltage()
{
	float channels[7] = {0};
	for(uint8_t i = 0; i < 7; i++)
	{
		channels[i] = ADC___Get_Voltage(i) - CCS___Get_Channel_Current(i);
	}
	float ret = 100.0;
	for(uint8_t i = 0; i < 7; i++)
	{
		if(channels[i] < ret)
		{
			ret = channels[i];
		}
	}
	return(ret);
}

int main(void)
{
	clock___Init();
	indicate___Init();
	boost___Init();
	CCS___Init();
	CAN___Init();
	CAN___Accept_All_Messages();
	CAN___Set_RX_Callback(RX_Callback);
	ADC___Init();
	TIM5_Init();

	CCS___Write_Channel(0, 0);
	CCS___Write_Channel(1, 0);
	CCS___Write_Channel(2, 0);
	CCS___Write_Channel(3, 0);
	CCS___Write_Channel(4, 0);
	CCS___Write_Channel(5, 0);
	CCS___Write_Channel(6, 0);
	CCS___Write_Channel(7, 0);

	boost___Set_Voltage(58.0);

	clock___Delay_ms(1000);
	indicate___Set_Value(1);
	clock___Delay_ms(1000);
	indicate___Set_Value(3);
	clock___Delay_ms(1000);
	indicate___Set_Value(7);
	boost___Enable();

	setup_Complete = 1;

	float boost_cmd = 58.0f;   // start high

	for(;;)
	{
	    float V_min = get_Lowest_CC_Voltage();

	    if(V_min < HEADROOM_LOW)
	    {
	        boost_cmd += BOOST_STEP_UP;     // increase
	    }
	    else if(V_min > HEADROOM_HIGH)
	    {
	        boost_cmd -= BOOST_STEP_DOWN;   // reduce
	    }

	    // clamp
	    if(boost_cmd > V_FULLSCALE) boost_cmd = V_FULLSCALE;
	    if(boost_cmd < V_ZEROSCALE) boost_cmd = V_ZEROSCALE;

	    boost___Set_Voltage(boost_cmd);

	    clock___Delay_ms(50);   // ~20 Hz loop
	}
}

void TIM3_IRQHandler(void)				// 10Hz CAN status interrupt
{
    if (TIM3->SR & TIM_SR_UIF)
    {
        TIM3->SR &= ~TIM_SR_UIF;          // Clear interrupt flag

        CAN_Tansmit_TypeDef payload;
        payload.ID = CAN_ID_LIGHT_LEVEL_STATUS;
        payload.data_Length = 7;
        for(uint8_t i = 0; i < 7; i++)
        {
        	payload.data[i] = (uint8_t)((CCS___Get_Channel_Current(i) / CCS___MAX_CURRENT) * 100.0f);
        }
        CAN___Transmit(payload);
    }
}
