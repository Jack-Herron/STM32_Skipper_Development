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
	clock_Init();
	FMC_SDRAM___SDRAM_Init();
	DSI_LCD___Init();

	//TODO add init functions for SDRAM, QSPI, DSI LCD

	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 4096);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);


	osThreadDef(GFXTask, startGFXTask, osPriorityNormal, 0, 8192);
	GFXTaskHandle = osThreadCreate(osThread(GFXTask), NULL);

	osKernelStart();

	for(;;);
}

uint8_t orientation = 0;
uint8_t mode = 0;

void StartDefaultTask(void const * argument)
{

	for(;;)
	{
		osDelay(400);
		DSI_LCD___Generate_Pattern(mode, orientation);

		switch(mode | (orientation<<1))
		{
		case 0:
			mode = 1;
			break;
		case 1:
			orientation = 1;
			mode = 0;
			break;
		case 2:
			mode = 1;
			break;
		case 3:
			orientation = 0;
			mode = 0;
			break;
		}
	}
}

void startGFXTask(void const * argument)
{
	//TODO add LVGL init functions();
	for(;;)
	{
		//lv_timer_handler();
		osDelay(1);
	}
}
