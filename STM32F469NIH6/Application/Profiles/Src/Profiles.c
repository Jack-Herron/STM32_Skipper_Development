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
		osDelay(100);
	}
}
