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
		osMutexWait(App___Profiles_State_Mutex, osWaitForever);

		if(App___Profiles_State.lighting_Mode == 0)
		{
			osMutexWait(App___IO_Control_State_Mutex, osWaitForever);

			App___IO_Control_State.lighting.white 		= 1;
			App___IO_Control_State.lighting.purple 		= 10;
			App___IO_Control_State.lighting.lime 		= 5;
			App___IO_Control_State.lighting.red 		= 10;
			App___IO_Control_State.lighting.far_Red 	= 5;

			osMutexRelease(App___IO_Control_State_Mutex);
		}
		else
		{
			osMutexWait(App___IO_Control_State_Mutex, osWaitForever);

			App___IO_Control_State.lighting.white 		= 100;
			App___IO_Control_State.lighting.purple 		= 100;
			App___IO_Control_State.lighting.lime 		= 100;
			App___IO_Control_State.lighting.red 		= 100;
			App___IO_Control_State.lighting.far_Red 	= 100;

			osMutexRelease(App___IO_Control_State_Mutex);
		}

		osMutexRelease(App___Profiles_State_Mutex);
		//TODO implement QSPI + littlefs reads
		osDelay(100);
	}
}
