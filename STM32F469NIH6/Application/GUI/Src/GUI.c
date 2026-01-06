/*
 * GUI.c
 *
 *  Created on: Jan 5, 2026
 *      Author: Jack Herron
 */

#include <stdlib.h>
#include <stdint.h>
#include "App.h"
#include "GUI.h"
#include "cmsis_os.h"
#include "lvgl.h"
#include "ui.h"

static uint32_t GUI___LV_Get_Tick_Callback(void)
{
    return(App___Get_Tick());
}

static void GUI___LV_Touch_Read_Callback(lv_indev_t * indev, lv_indev_data_t * data)
{
	osMutexWait(App___GUI_GFX_State_Mutex, osWaitForever);

	data->point.x 	= App___GUI_TS_State.last_Point.x;
	data->point.y 	= App___GUI_TS_State.last_Point.y;
	data->state 	= App___GUI_TS_State.last_Point.pressed;

	osMutexRelease(App___GUI_GFX_State_Mutex);
}

static void GUI___LV_Flush_Callback(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
	if(lv_disp_flush_is_last(disp))
	{
		App___Frame_Ready(px_map);
	}
	else
	{
		lv_display_flush_ready(disp);
	}
}

void GUI___LV_Flush_wait_Callback(lv_display_t * disp)
{
    osSignalWait(APP___GUI_GFX_FLUSH_READY_FLAG, 1000);
    lv_display_flush_ready(disp);
}


// TODO rename this function
void GUI_Toggle_Bright_Mode(lv_event_t * e)
{
	osMutexWait(App___IO_Control_State_Mutex, osWaitForever);

	App___IO_Control_State.lighting.mode ^=1;

	osMutexRelease(App___IO_Control_State_Mutex);
}

void GUI___Refresh_Lighting_Indicators(void)
{
	osMutexWait(App___IO_Control_State_Mutex, osWaitForever);

	_ui_bar_set_property(uic_White_Light_Indicator, 	0, 	App___IO_Control_State.lighting.white);
	_ui_bar_set_property(uic_Purple_Light_Indicator, 	0, 	App___IO_Control_State.lighting.purple);
	_ui_bar_set_property(uic_Lime_Light_Indicator, 		0, 	App___IO_Control_State.lighting.lime);
	_ui_bar_set_property(uic_Red_Light_Indicator, 		0, 	App___IO_Control_State.lighting.red);
	_ui_bar_set_property(uic_Far_Red_Light_Indicator, 	0, 	App___IO_Control_State.lighting.far_Red);

	osMutexRelease(App___IO_Control_State_Mutex);
}

void GUI___GFX_Start_Task(void const * argument)
{
	osSignalWait(APP___GUI_GFX_TASK_START_FLAG, osWaitForever);

	lv_init();

	lv_display_t* display = lv_display_create(App___GUI_Width, App___GUI_Height);

	lv_display_set_buffers(display, App___GUI_Buffer1_Ptr, App___GUI_Buffer2_Ptr, App___GUI_Buffer_Size, LV_DISPLAY_RENDER_MODE_DIRECT);

	lv_tick_set_cb(GUI___LV_Get_Tick_Callback);
	lv_display_set_flush_cb(display, GUI___LV_Flush_Callback);
	lv_display_set_flush_wait_cb(display, GUI___LV_Flush_wait_Callback);

	lv_indev_t * indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(indev, GUI___LV_Touch_Read_Callback);

	ui_init();

	DuckAnim_Animation(ui_Duck, 0);

	for(;;)
	{
		GUI___Refresh_Lighting_Indicators();
		uint32_t t = lv_timer_handler();
		osDelay(t);
	}
}

void GUI___TS_Start_Task(void const * argument)
{
	osSignalWait(APP___GUI_TS_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		osSignalWait(APP___GUI_TS_EVENT_FLAG, osWaitForever);
		uint16_t x;
		uint16_t y;
		uint8_t pressed = 0;
		uint8_t num_Points = 0;

		num_Points = App___GUI_TS_Get_Point(&x, &y);

		if(num_Points != 0)
		{
			pressed = 1;
		}

		// remap points to LVGLs format

		uint16_t new_X;
		uint16_t new_Y;

		new_X = y;
		new_Y = 479 - x;

		osMutexWait(App___GUI_TS_State_Mutex, osWaitForever);
		App___GUI_TS_State.last_Point.x = new_X;
		App___GUI_TS_State.last_Point.y = new_Y;
		App___GUI_TS_State.last_Point.pressed = pressed;
		osMutexRelease(App___GUI_TS_State_Mutex);
	}
}
