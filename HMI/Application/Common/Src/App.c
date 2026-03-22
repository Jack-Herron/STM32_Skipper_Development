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

volatile uint8_t App___Ready = 0;

uint32_t 			(*App___Get_Tick_CallBack)					(void) 												= NULL;
void 				(*App___GUI_GFX_Frame_Ready_CallBack)		(uint8_t* buf) 										= NULL;
uint8_t 			(*App___GUI_TS_Get_Point_Callback)			(uint16_t* x, uint16_t* y) 							= NULL;
App___Time_TypeDef 	(*App___Get_Time_Callback)					(void) 												= NULL;
App___Date_TypeDef 	(*App___Get_Date_Callback)					(void) 												= NULL;
void 				(*App___Set_Backlight_Brightness_Callback)	(uint16_t level) 									= NULL;
void 				(*App___Time_And_Date_Set_Callback)			(App___Time_TypeDef time, App___Date_TypeDef date) 	= NULL;
void 				(*App___Transmit_Callback)					(App___IO_TX_Data_Typedef) 							= NULL;

volatile App___Profiles_State_TypeDef 		App___Profiles_State;
volatile App___GUI_TS_State_TypeDef 		App___GUI_TS_State;
volatile App___GUI_GFX_State_TypeDef	 	App___GUI_GFX_State;
volatile App___IO_Sense_State_TypeDef 		App___IO_Sense_State;
volatile App___IO_Control_State_TypeDef 	App___IO_Control_State;

osMutexId App___IO_Control_State_Mutex;
osMutexDef(App___IO_CONTROL_STATE_MUTEX);

osMutexId App___IO_Sense_State_Mutex;
osMutexDef(App___IO_SENSE_STATE_MUTEX);

osMutexId App___GUI_GFX_State_Mutex;
osMutexDef(App___GUI_GFX_STATE_MUTEX);

osMutexId App___GUI_TS_State_Mutex;
osMutexDef(App___GUI_TS_STATE_MUTEX);

osMutexId App___Profiles_State_Mutex;
osMutexDef(App___PROFILES_STATE_MUTEX);

// Declare thread handles

osThreadId Control_Task_Handle;
osThreadId Sense_Task_Handle;
osThreadId profiles_Task_Handle;
osThreadId TS_Task_Handle;
osThreadId GFX_Task_Handle;

uint32_t App___GUI_Width;
uint32_t App___GUI_Height;

void* App___GUI_Buffer1_Ptr;
void* App___GUI_Buffer2_Ptr;
void* App___GUI_Buffer3_Ptr;
void* App___GUI_Buffer4_Ptr;

uint32_t App___GUI_Buffer_Size;



void App___Init(void)
{
	// Create threads

	osThreadDef(ControlTask, IO___Control_Start_Task, osPriorityNormal, 0, 512);
	Control_Task_Handle = osThreadCreate(osThread(ControlTask), NULL);

	osThreadDef(SenseTask, IO___Sense_Start_Task, osPriorityNormal, 0, 512);
	Sense_Task_Handle = osThreadCreate(osThread(SenseTask), NULL);

	osThreadDef(profilesTask, Profiles___Start_Task, osPriorityNormal, 0, 4096);
	profiles_Task_Handle = osThreadCreate(osThread(profilesTask), NULL);

	osThreadDef(GFXTask, GUI___GFX_Start_Task, osPriorityNormal, 0, 16384);
	GFX_Task_Handle = osThreadCreate(osThread(GFXTask), NULL);

	osThreadDef(TSTask, GUI___TS_Start_Task, osPriorityNormal, 0, 512);
	TS_Task_Handle = osThreadCreate(osThread(TSTask), NULL);

	// Register Mutexes

	App___IO_Control_State_Mutex 	= osMutexCreate(osMutex(App___IO_CONTROL_STATE_MUTEX));
	App___IO_Sense_State_Mutex 		= osMutexCreate(osMutex(App___IO_SENSE_STATE_MUTEX));
	App___GUI_GFX_State_Mutex 		= osMutexCreate(osMutex(App___GUI_GFX_STATE_MUTEX));
	App___GUI_TS_State_Mutex 		= osMutexCreate(osMutex(App___GUI_TS_STATE_MUTEX));
	App___Profiles_State_Mutex 		= osMutexCreate(osMutex(App___PROFILES_STATE_MUTEX));

	// Set signals to start tasks

	osSignalSet(GFX_Task_Handle, 		APP___GUI_GFX_TASK_START_FLAG);
	osSignalSet(profiles_Task_Handle, 	APP___PROFILES_TASK_START_FLAG);
	osSignalSet(TS_Task_Handle, 		APP___GUI_TS_TASK_START_FLAG);
	osSignalSet(Control_Task_Handle, 	APP___IO_CONTROL_TASK_START_FLAG);
	osSignalSet(Sense_Task_Handle, 		APP___IO_SENSE_TASK_START_FLAG);

	App___Ready = 1;
}

void App___GUI_TS_Event_Detected()
{
	osSignalSet(TS_Task_Handle, APP___GUI_TS_EVENT_FLAG);
}

void App___IO_Data_Received(App___IO_RX_Data_Typedef* packet)
{
	if(App___Ready)
	{
		IO___Store_Packet(packet);
		osSignalSet(Sense_Task_Handle, APP___IO_RX_EVENT_FLAG);
	}
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

void App___Set_Get_Time_CallBack(App___Time_TypeDef (*get_Time_CallBack)(void))
{
	App___Get_Time_Callback = get_Time_CallBack;
}

void App___Set_Get_Date_CallBack(App___Date_TypeDef (*get_Date_CallBack)(void))
{
	App___Get_Date_Callback = get_Date_CallBack;
}

void App___Set_Change_Backlight_Brightness_Callback(void (*callback)(uint16_t))
{
	App___Set_Backlight_Brightness_Callback = callback;
}

void App___Set_Backlight_Brightness(uint16_t level)
{
	if(App___Set_Backlight_Brightness_Callback != NULL)
	{
		App___Set_Backlight_Brightness_Callback(level);
	}

}

App___Time_TypeDef App___Get_Time(void)
{
	App___Time_TypeDef ret = {0};

	if(App___Get_Time_Callback != NULL)
	{
		ret = App___Get_Time_Callback();
	}

	return(ret);
}

App___Date_TypeDef App___Get_Date(void)
{
	App___Date_TypeDef ret = {0};

	if(App___Get_Date_Callback != NULL)
	{
		ret = App___Get_Date_Callback();
	}

	return(ret);
}

void App___Set_Time_And_Date_Callback(void (*time_And_Date_Set_Callback)(App___Time_TypeDef time, App___Date_TypeDef date))
{
	App___Time_And_Date_Set_Callback = time_And_Date_Set_Callback;
}

void App___Set_Time_And_Date(App___Time_TypeDef time, App___Date_TypeDef date)
{
	if(App___Time_And_Date_Set_Callback != NULL)
	{
		App___Time_And_Date_Set_Callback(time, date);
	}
}
void APP___Set_Transmit_Callback(void (*callback)(App___IO_TX_Data_Typedef))
{

}
