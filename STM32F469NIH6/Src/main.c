// Jack Herron


#define CORE_CLOCK 180000000ul

#include <stdint.h>
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "Clock.h"
#include "FMC_SDRAM.h"
#include "QSPI_Flash.h"
#include "DSI_LCD.h"

osThreadId defaultTaskHandle;
osThreadId touchGFXTaskHandle;

void StartDefaultTask(void const * argument);
void startTGFXTask(void const * argument);

int main(void)
{
	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 4096);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);


	osThreadDef(touchGFXTask, startTGFXTask, osPriorityNormal, 0, 8192);
	touchGFXTaskHandle = osThreadCreate(osThread(touchGFXTask), NULL);

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


void startTGFXTask(void const * argument)
{
	//MX_TouchGFX_Process();
	for(;;)
	{
		osDelay(1);
	}
}
