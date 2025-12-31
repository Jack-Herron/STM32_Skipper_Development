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
uint8_t j = 0;
void StartDefaultTask(void const * argument)
{

	for(;;)
	{
		osDelay(100);
		uint32_t* bfr = (uint32_t*)0xc0000000;
		for(uint32_t i = 0; i < 500000; i++)
		{
			if(j==0)
			{
				bfr[i]=0xff00ff00;

			}
			else if (j == 1) {
				bfr[i] = 0xffff0000;

			}
			else if (j == 2)
			{
				bfr[i] = 0xff0000ff;

			}
		}
		j++;
		if (j > 2) {
			j = 0;
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
