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

void IO___Sensor_Start_Task(void const * argument)
{
	osSignalWait(APP___SENSOR_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		osDelay(100);
	}
}

void IO___Control_Start_Task(void const * argument)
{
	osSignalWait(APP___CONTROL_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		osDelay(100);
	}
}
