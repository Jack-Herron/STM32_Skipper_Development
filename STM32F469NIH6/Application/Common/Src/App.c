/*
 * App.c
 *
 *  Created on: Jan 5, 2026
 *      Author: Jack Herron
 */

// Dependencies: FreeRTOS, LVGL (eventually LittleFS, FatFS)

#include "App.h"
#include "GUI.h"
#include "IO.h"
#include "Profiles.h"
#include <stdlib.h>


uint32_t (*App___Get_Tick_CallBack)(void) = NULL;
void (*App___GUI_GFX_Frame_Ready_CallBack)(uint8_t* buf) = NULL;
uint8_t (*App___GUI_TS_Get_Point_Callback)(uint16_t* x, uint16_t* y);


volatile App___Profiles_State_TypeDef 		App___Profiles_State;
volatile App___GUI_TS_State_TypeDef 		App___GUI_TS_State;
volatile App___GUI_GFX_State_TypeDef	 	App___GUI_GFX_State;
volatile App___IO_Sensor_State_TypeDef 		App___IO_Sensor_State;
volatile App___IO_Control_State_TypeDef 	App___IO_Control_State;

osMutexId App___IO_Control_State_Mutex;
osMutexDef(App___IO_CONTROL_STATE_MUTEX);

osMutexId App___IO_Sensor_State_Mutex;
osMutexDef(App___IO_SENSOR_STATE_MUTEX);

osMutexId App___GUI_GFX_State_Mutex;
osMutexDef(App___GUI_GFX_STATE_MUTEX);

osMutexId App___GUI_TS_State_Mutex;
osMutexDef(App___GUI_TS_STATE_MUTEX);

osMutexId App___Profiles_State_Mutex;
osMutexDef(App___PROFILES_STATE_MUTEX);

// Declare thread handles

osThreadId control_Task_Handle;
osThreadId sensor_Task_Handle;
osThreadId profiles_Task_Handle;
osThreadId TS_Task_Handle;
osThreadId GFX_Task_Handle;

uint32_t App___GUI_Width;
uint32_t App___GUI_Height;

void* App___GUI_Buffer1_Ptr;
void* App___GUI_Buffer2_Ptr;

uint32_t App___GUI_Buffer_Size;

void App___Init(void)
{
	// Create threads

	osThreadDef(sensorTask, IO___Sensor_Start_Task, osPriorityNormal, 0, 512);
	sensor_Task_Handle = osThreadCreate(osThread(sensorTask), NULL);

	osThreadDef(controlTask, IO___Control_Start_Task, osPriorityNormal, 0, 512);
	control_Task_Handle = osThreadCreate(osThread(controlTask), NULL);

	osThreadDef(profilesTask, Profiles___Start_Task, osPriorityNormal, 0, 4096);
	profiles_Task_Handle = osThreadCreate(osThread(profilesTask), NULL);

	osThreadDef(GFXTask, GUI___GFX_Start_Task, osPriorityNormal, 0, 16384);
	GFX_Task_Handle = osThreadCreate(osThread(GFXTask), NULL);

	osThreadDef(TSTask, GUI___TS_Start_Task, osPriorityNormal, 0, 512);
	TS_Task_Handle = osThreadCreate(osThread(TSTask), NULL);

	// Register Mutexes

	App___IO_Control_State_Mutex 	= osMutexCreate(osMutex(App___IO_CONTROL_STATE_MUTEX));
	App___IO_Sensor_State_Mutex 	= osMutexCreate(osMutex(App___IO_SENSOR_STATE_MUTEX));
	App___GUI_GFX_State_Mutex 		= osMutexCreate(osMutex(App___GUI_GFX_STATE_MUTEX));
	App___GUI_TS_State_Mutex 		= osMutexCreate(osMutex(App___GUI_TS_STATE_MUTEX));
	App___Profiles_State_Mutex 		= osMutexCreate(osMutex(App___PROFILES_STATE_MUTEX));

	// Set signals to start tasks

	osSignalSet(GFX_Task_Handle, 		APP___GUI_GFX_TASK_START_FLAG);
	osSignalSet(profiles_Task_Handle, 	APP___PROFILES_TASK_START_FLAG);
	osSignalSet(TS_Task_Handle, 		APP___GUI_TS_TASK_START_FLAG);
	osSignalSet(sensor_Task_Handle, 	APP___SENSOR_TASK_START_FLAG);
	osSignalSet(control_Task_Handle, 	APP___CONTROL_TASK_START_FLAG);

}

void App___GUI_TS_Event_Detected()
{
	osSignalSet(TS_Task_Handle, APP___GUI_TS_EVENT_FLAG);
}

void App___Frame_Ready(uint8_t* buf)
{
	if(App___GUI_GFX_Frame_Ready_CallBack != NULL)
	{
		App___GUI_GFX_Frame_Ready_CallBack(buf);
	}
}

uint32_t App___Get_Tick(void)
{
	uint32_t ret = 0;

	if(App___Get_Tick_CallBack != NULL)
	{
		ret = App___Get_Tick_CallBack();
	}

	return(ret);
}

void App___Set_Get_Tick_Callback(uint32_t (*tick_CallBack)(void))
{
	App___Get_Tick_CallBack = tick_CallBack;
}

void App___Set_GUI_GFX_Frame_Ready_CallBack(void (*frame_Ready_CallBack)(uint8_t* buf))
{
	App___GUI_GFX_Frame_Ready_CallBack = frame_Ready_CallBack;
}

void App___GUI_GFX_Buffer_Swap_Ready(void)
{
	osSignalSet(GFX_Task_Handle, APP___GUI_GFX_FLUSH_READY_FLAG);
}

void App___GUI_Set_Dimentions(uint32_t width, uint32_t height)
{
	App___GUI_Width = width;
	App___GUI_Height = height;
}

void App___GUI_Set_Buffers(void* buf1, void* buf2, uint32_t size)
{
	App___GUI_Buffer1_Ptr = buf1;
	App___GUI_Buffer2_Ptr = buf2;
	App___GUI_Buffer_Size = size;
}

uint8_t App___GUI_TS_Get_Point(uint16_t* x, uint16_t* y)
{
	uint8_t ret = 0;

	if(App___GUI_TS_Get_Point_Callback != NULL)
	{
		ret = App___GUI_TS_Get_Point_Callback(x,y);
	}

	return(ret);
}

void App___Set_GUI_TS_Get_Point_CallBack(uint8_t (*get_Point_Callback)(uint16_t* x, uint16_t* y))
{
	App___GUI_TS_Get_Point_Callback = get_Point_Callback;
}



