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
#include "stdio.h"



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

void GUI___Refresh_Time()
{
	App___Time_TypeDef time = App___Get_Time();
	App___Date_TypeDef date = App___Get_Date();

	lv_calendar_date_t shown_date = {
	    .year  	= date.year,
	    .month 	= date.month,
		.day	= date.day
	};

	lv_calendar_set_today_date(uic_Calendar, shown_date.year, shown_date.month, shown_date.day);

	char time_Str[16];

	if(time.pm)
	{
		sprintf(time_Str, "%d:%02d PM", time.hour, time.minute);
	}
	else
	{
		sprintf(time_Str, "%d:%02d AM", time.hour, time.minute);
	}

	_ui_label_set_property(uic_Main_Clock, 0, time_Str);
	_ui_label_set_property(uic_Calendar_Clock, 0, time_Str);
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

void GUI___Date_Time_Change_Callback(lv_event_t * e)
{
	uint16_t month_Index = lv_roller_get_selected(uic_DT_Selector_Month);		// starts at 0:	Jan
	uint16_t day_Index = lv_roller_get_selected(uic_DT_Selector_Day);			// starts at 0: 1
	uint16_t year_Index = lv_roller_get_selected(uic_DT_Selector_Year);			// starts at 0: 2000
	uint16_t hour_Index = lv_roller_get_selected(uic_DT_Selector_Hour);			// starts at 0: 1
	uint16_t minute_Index = lv_roller_get_selected(uic_DT_Selector_Minute);		// starts at 0: 0
	uint16_t AMPM_Index = lv_roller_get_selected(uic_DT_Selector_AMPM);			// starts at 0: AM

	App___Time_TypeDef time;
	time.hour 	= hour_Index + 1;
	time.minute = minute_Index;
	time.second = 0;
	time.pm		= AMPM_Index;

	App___Date_TypeDef date;
	date.year	= year_Index;
	date.month 	= month_Index+1;
	date.day 	= day_Index+1;


	lv_calendar_set_showed_date(uic_Calendar, date.year, date.month);
	App___Set_Time_And_Date(time, date);

	GUI___Refresh_Time();
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

	uint32_t last_UI_Element_Update = 0;

	for(;;)
	{
		uint32_t tick = App___Get_Tick();

		if(((App___Get_Tick() - last_UI_Element_Update) > GUI___GUI_UPDATE_PERIOD_ms))
		{
			GUI___Refresh_Time();
			GUI___Refresh_Lighting_Indicators();
			last_UI_Element_Update = tick;
		}

		uint32_t t = lv_timer_handler();
		osDelay(t);
	}
}
