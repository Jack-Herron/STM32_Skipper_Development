// Jack Herron


#define CORE_CLOCK 180000000ul
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

uint32_t SystemCoreClock = CORE_CLOCK;

osThreadId defaultTaskHandle;
osThreadId GFXTaskHandle;

void StartDefaultTask(void const * argument);
void startGFXTask(void const * argument);

int main(void)
{
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
	osDelay(1);
	}

}


void startGFXTask(void const * argument)
{
	//TODO add LVGL init functions();
	for(;;)
	{
		osDelay(1);
	}
}

//TODO add 1ms systick tick incrementer test
