#include <stdint.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "indicate.h"
#include "CAN.h"
#include "ADC.h"
#include "main.h"

uint8_t setup_Complete = 0;

void TIM3_Init(void)
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

void TIM2_Init(void)
{
    /* Enable clocks */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;   // GPIOA clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   // TIM2 clock
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;   // Enable AFIO clock

    AFIO->MAPR &= ~AFIO_MAPR_TIM2_REMAP;
    AFIO->MAPR |=  AFIO_MAPR_TIM2_REMAP_1;   // Partial remap

    GPIOB->CRH &= ~(GPIO_CRH_MODE10 	| GPIO_CRH_CNF10);
    GPIOB->CRH |=  (GPIO_CRH_MODE10_1 	| GPIO_CRH_MODE10_0);   // Output 50 MHz
    GPIOB->CRH |=  (GPIO_CRH_CNF10_1);                      	// AF push-pull

    TIM2->CR1 = 0;
    TIM2->PSC = 71;        // 36 MHz / (71+1) = 1 MHz timer clock
    TIM2->ARR = 99;       // 1 MHz / (99+1) = 10 kHz PWM frequency
    TIM2->CNT = 0;

    TIM2->CCR3 = 50;      // 50% duty cycle
    TIM2->CCMR2 &= ~TIM_CCMR2_OC3M;
    TIM2->CCMR2 |= (6 << TIM_CCMR2_OC3M_Pos); // PWM mode 1
    TIM2->CCMR2 |= TIM_CCMR2_OC3PE;           // Enable preload for CCR1

    TIM2->CCER |= TIM_CCER_CC3E;
    TIM2->CR1 |= TIM_CR1_ARPE;
    TIM2->EGR |= TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void set_Motor_Speed(uint16_t speed)
{
	TIM2->CCR3 = speed;
}
void RX_Callback(CAN___Receive_TypeDef packet)
{
	switch(packet.ID)
	{
	case 0x100:

		break;
	}
}


int main(void)
{
	clock___Init();
	indicate___Init();
	CAN___Init();
	CAN___Accept_All_Messages();
	CAN___Set_RX_Callback(RX_Callback);
	ADC___Init();
	TIM3_Init();
	TIM2_Init();

	setup_Complete = 1;

	GPIOB->CRH &= ~GPIO_CRH_MODE15_Msk;				// Clear PB11 mode
	GPIOB->CRH &= ~GPIO_CRH_CNF15_Msk;				// clear PB11 configuration

	GPIOB->ODR |= GPIO_ODR_ODR15;					// Clear

	GPIOB->CRH |= GPIO_CRH_MODE15_0;				// Set PB11 to output (slow)


	for(;;)
	{
		printf("{\"soil_moisture_1\":%.2f,\"soil_moisture_2\":%.2f,\"soil_moisture_3\":%.2f}\n",
		       ADC___Get_Voltage(1),
		       ADC___Get_Voltage(2),
		       ADC___Get_Voltage(3));
//		for(uint16_t i = 25; i < 100; i++)
//		{
//			set_Motor_Speed(i);
//			clock___Delay_ms(50);
//		}
//		clock___Delay_ms(1000);
//		for(uint16_t i = 99; i > 25; i--)
//		{
//			set_Motor_Speed(i);
//			clock___Delay_ms(50);
//		}
  		clock___Delay_ms(1000);
	}
}

void TIM3_IRQHandler(void)				// 10Hz CAN status interrupt
{
    if (TIM3->SR & TIM_SR_UIF)
    {
        TIM3->SR &= ~TIM_SR_UIF;          // Clear interrupt flag

        //CAN_Tansmit_TypeDef payload;
        //CAN___Transmit(payload);
    }
}
