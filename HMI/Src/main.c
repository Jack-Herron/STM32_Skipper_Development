// Jack Herron
#define LV_LVGL_H_INCLUDE_SIMPLE


#include "stm32f4xx.h"
#include <stdint.h>

//#include "cmsis_os.h"
#include "Clock.h"
#include "FMC_SDRAM.h"
//#include "QSPI.h"
#include "DSI_LCD.h"
//#include "lvgl.h"
#include "stdio.h"
//#include "ui.h"
//#include "I2C.h"
//#include "TS.h"
//#include "App.h"
#include "RTC.h"
#include "USART.h"
#include "CAN.h"

uint32_t test = 0;

/*
#define COMPILE_HOUR   ((__TIME__[0] - '0') * 10 + (__TIME__[1] - '0'))
#define COMPILE_MINUTE ((__TIME__[3] - '0') * 10 + (__TIME__[4] - '0'))
#define COMPILE_SECOND ((__TIME__[6] - '0') * 10 + (__TIME__[7] - '0'))

uint32_t SystemCoreClock = HCLK_FREQ;

void StartControlTask(void const * argument);
void StartSensorTask(void const * argument);
void StartProfileTask(void const * argument);
void StartTSTask(void const * argument);
void startGFXTask(void const * argument);

// TODO make HW driver access thread safe

void DSI_Buffer_Swap_Callback(void)
{
	App___GUI_GFX_Buffer_Swap_Ready();
}

void TS_Event_Callback(void)
{
	App___GUI_TS_Event_Detected();
}

void frame_Ready_Callback(uint8_t* buf)
{
	DSI_LCD___Set_Pending_Buffer(buf);
}

static uint32_t tick_Callback(void)
{
    return(clock___millis());
}

uint8_t TS_Get_Point_Callback(uint16_t* x, uint16_t* y)
{
	uint8_t num_Points = TS___Get_Num_Points_Pressed();
	if(num_Points > 0)
	{
		TS___Get_Point(x, y);
	}

	return(num_Points);
}

App___Time_TypeDef get_Time_CallBack(void)
{
	RTC___Time_TypeDef time = RTC___Get_Time();
	App___Time_TypeDef ret;
	ret.hour = time.hour;
	ret.minute = time.minute;
	ret.second = time.second;
	ret.pm = time.pm;

	return(ret);
}

App___Date_TypeDef get_Date_CallBack(void)
{
	RTC___Date_TypeDef date = RTC___Get_Date();
	App___Date_TypeDef ret;
	ret.year = date.year;
	ret.month = date.month;
	ret.day = date.day;

	return(ret);
}

void set_Time_And_Date_Callback(App___Time_TypeDef time, App___Date_TypeDef date)
{
	RTC___Time_TypeDef RTC_Time;
	RTC_Time.hour = time.hour;
	RTC_Time.minute = time.minute;
	RTC_Time.second = time.second;
	RTC_Time.pm = time.pm;

	RTC___Date_TypeDef RTC_Date;
	RTC_Date.day = date.day;
	RTC_Date.month = date.month;
	RTC_Date.year = date.year;

	RTC___Set_Time_And_Date(RTC_Time, RTC_Date);
}
*/
// Tclk = 90Mhz
void TIM4CH2_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

    GPIOD->MODER &= ~GPIO_MODER_MODER13_Msk;
    GPIOD->MODER |= GPIO_MODER_MODER13_1;              // Alternate function mode

    GPIOD->OTYPER &= ~GPIO_OTYPER_OT13;                // Push-pull
    GPIOD->OSPEEDR |= GPIO_OSPEEDR_OSPEED13_Msk;     // High speed
    GPIOD->PUPDR &= ~GPIO_PUPDR_PUPD13_Msk;            // No pull-up/down

    GPIOD->AFR[1] &= ~GPIO_AFRH_AFSEL13_Msk;
    GPIOD->AFR[1] |= (2U << GPIO_AFRH_AFSEL13_Pos);    // AF2 = TIM4_CH2

    TIM4->PSC = 9 - 1;
    TIM4->ARR = 1000 - 1;
    TIM4->CCR2 = 250;                                 // duty 0-1000

    TIM4->CR1 |= TIM_CR1_ARPE;
    TIM4->CCMR1 &= ~(TIM_CCMR1_CC2S_Msk | TIM_CCMR1_OC2M_Msk);
    TIM4->CCMR1 |= (6U << TIM_CCMR1_OC2M_Pos);         // PWM mode 1
    TIM4->CCMR1 |= TIM_CCMR1_OC2PE;                    // CCR preload enable
    TIM4->CCER &= ~TIM_CCER_CC2P;
    TIM4->CCER |= TIM_CCER_CC2E;
    TIM4->EGR |= TIM_EGR_UG;
    TIM4->CR1 |= TIM_CR1_CEN;
}

int main(void)
{
	// HW init

	clock_Init();
	millis_Init();
	RTC___Init();
	FMC_SDRAM___SDRAM_Init();
	DSI_LCD___Init();
	TIM4CH2_Init();
	USART___Init(4);
	CAN___Init();
	CAN___Accept_All_Messages();
	//DSI_LCD___Set_Swap_Callback(DSI_Buffer_Swap_Callback);
	//TS___Init();
	//TS___Set_Event_Callback(TS_Event_Callback);

	// App setup

	//App___GUI_Set_Buffers((void*)0xc0000000, (void*)(0xc0000000 + 768000), 768000);
	//App___GUI_Set_Dimentions(800, 480);
	//App___Set_GUI_GFX_Frame_Ready_CallBack(frame_Ready_Callback);
	//App___Set_Get_Tick_Callback(tick_Callback);
	//App___Set_GUI_TS_Get_Point_CallBack(TS_Get_Point_Callback);
	//App___Set_Get_Time_CallBack(get_Time_CallBack);
	//App___Set_Get_Date_CallBack(get_Date_CallBack);
	//App___Set_Time_And_Date_Callback(set_Time_And_Date_Callback);

	// App init

	//App___Init();

	//osKernelStart();
	uint8_t mode = 0;
	for(;;)
	{
		for(uint32_t i = 0; i <1000; i++)
		{
			TIM4->CCR2 = i;
			clock___delay_ms(2);
		}

		for(uint32_t i = 1000; i > 0; i--)
		{
			TIM4->CCR2 = i-1;
			clock___delay_ms(2);
		}
		mode = !mode;
		DSI_LCD___Generate_Pattern(0, mode);
		clock___delay_ms(50);
	}
}

