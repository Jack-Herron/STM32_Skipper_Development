/*
 * IO.c
 *
 *  Created on: Jan 5, 2026
 *      Author: Jack Herron
 */

#include <stdlib.h>
#include <stdint.h>
#include "App.h"
#include "IO.h"
#include "cmsis_os.h"

void IO___RX_Start_Task(void const * argument)
{
	osSignalWait(APP___IO_RX_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		osDelay(100);
	}
}

void IO___TX_Start_Task(void const * argument)
{
	osSignalWait(APP___IO_TX_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		osDelay(100);
	}
}
