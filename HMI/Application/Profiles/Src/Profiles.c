/*
 * Profiles.c
 *
 *  Created on: Jan 5, 2026
 *      Author: Jack Herron
 */

#include <stdlib.h>
#include <stdint.h>
#include "App.h"
#include "Profiles.h"
#include "cmsis_os.h"

void Profiles___Start_Task(void const * argument)
{
	osSignalWait(APP___PROFILES_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		// simulated profile reads
		osMutexWait(App___IO_Control_State_Mutex, osWaitForever);

		App___IO_Control_State.lighting.white 		= 3;
		App___IO_Control_State.lighting.purple 		= 3;
		App___IO_Control_State.lighting.lime 		= 3;
		App___IO_Control_State.lighting.red 		= 3;
		App___IO_Control_State.lighting.far_Red 	= 3;

		osMutexRelease(App___IO_Control_State_Mutex);

		//TODO implement QSPI + littlefs reads
		osDelay(100);
	}
}
