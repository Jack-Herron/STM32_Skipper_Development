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
		if(App___IO_Control_State.lighting.mode == 0)
		{
			App___IO_Control_State.lighting.white 		= 100;
			App___IO_Control_State.lighting.purple 		= 180;
			App___IO_Control_State.lighting.lime 		= 90;
			App___IO_Control_State.lighting.red 		= 200;
			App___IO_Control_State.lighting.far_Red 	= 70;
		}
		else
		{
			App___IO_Control_State.lighting.white 		= 255;
			App___IO_Control_State.lighting.purple 		= 0;
			App___IO_Control_State.lighting.lime 		= 0;
			App___IO_Control_State.lighting.red 		= 0;
			App___IO_Control_State.lighting.far_Red 	= 0;
		}
		osMutexRelease(App___Profiles_State_Mutex);

		//TODO implement QSPI + littlefs reads
		osDelay(100);
	}
}
