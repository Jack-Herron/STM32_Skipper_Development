// Jack Herron


#define LV_LVGL_H_INCLUDE_SIMPLE


#include <stdint.h>
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "Clock.h"
#include "FMC_SDRAM.h"
#include "QSPI_Flash.h"
#include "DSI_LCD.h"
#include "lvgl.h"
#include "stdio.h"

uint32_t SystemCoreClock = HCLK_FREQ;

osThreadId defaultTaskHandle;
osThreadId GFXTaskHandle;

void StartDefaultTask(void const * argument);
void startGFXTask(void const * argument);

int main(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable GPIOA clock
	GPIOA->MODER = 0;
	GPIOA->MODER |= GPIO_MODER_MODER8_0; // Set PA8 as output
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8; // Set PA8 speed to high
	GPIOA->MODER |= GPIO_MODER_MODER15_0; // Set PA15 as output
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15; // Set PA15 speed to high

	clock_Init();
	//TODO add init functions for clock, SDRAM, QSPI, DSI LCD

	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 4096);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);


	osThreadDef(GFXTask, startGFXTask, osPriorityNormal, 0, 8192);
	GFXTaskHandle = osThreadCreate(osThread(GFXTask), NULL);

	osKernelStart();

	for(;;);
}

void StartDefaultTask(void const * argument)
{

	for(;;)
	{
		GPIOA->ODR ^= GPIO_ODR_ODR_8; // TOGGLE
		osDelay(1);
	}

}


void startGFXTask(void const * argument)
{
	//TODO add LVGL init functions();
	for(;;)
	{
		//lv_timer_handler();
		GPIOA->ODR ^= GPIO_ODR_ODR_15; // TOGGLE
		osDelay(1);
	}
}

//TODO add 1ms systick tick incrementer test
