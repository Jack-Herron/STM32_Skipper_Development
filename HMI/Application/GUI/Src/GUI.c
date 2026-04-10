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
#include "string.h"

uint32_t GUI___TS_Get_Recent_Point_Index()
{
	uint32_t w_Cursor = App___GUI_TS_State.point_FIFO.w_Cursor;
	return((w_Cursor+APP___GUI_TS_FIFO_DEPTH-1) % APP___GUI_TS_FIFO_DEPTH);
}

void GUI___TS_Push_Point(App___GUI_TS_Point_TypeDef point)
{
	osMutexWait(App___GUI_TS_State_Mutex, osWaitForever);

	uint32_t w_Cursor = App___GUI_TS_State.point_FIFO.w_Cursor;
	uint32_t r_Cursor = App___GUI_TS_State.point_FIFO.r_Cursor;

	if(point.pressed != App___GUI_TS_State.point_FIFO.point[w_Cursor].pressed)
	{

		if(w_Cursor+1 == r_Cursor)
		{
			App___GUI_TS_State.point_FIFO.r_Cursor = (r_Cursor+1) % APP___GUI_TS_FIFO_DEPTH;
		}

		App___GUI_TS_State.point_FIFO.w_Cursor = (w_Cursor+1) % APP___GUI_TS_FIFO_DEPTH;
		w_Cursor = (w_Cursor+1) % APP___GUI_TS_FIFO_DEPTH;

		App___GUI_TS_State.point_FIFO.point[w_Cursor] = point;

	}
	else
	{
		App___GUI_TS_State.point_FIFO.point[w_Cursor].x = point.x;
		App___GUI_TS_State.point_FIFO.point[w_Cursor].y = point.y;
	}

	osMutexRelease(App___GUI_TS_State_Mutex);
}
//32.7715kHz
App___GUI_TS_Point_TypeDef GUI___TS_Pop_Point()
{
	App___GUI_TS_Point_TypeDef ret;

	osMutexWait(App___GUI_TS_State_Mutex, osWaitForever);

	uint32_t w_Cursor = App___GUI_TS_State.point_FIFO.w_Cursor;
	uint32_t r_Cursor = App___GUI_TS_State.point_FIFO.r_Cursor;

	ret = App___GUI_TS_State.point_FIFO.point[r_Cursor];

	if(r_Cursor != w_Cursor)
	{
		App___GUI_TS_State.point_FIFO.r_Cursor = (r_Cursor+1) % APP___GUI_TS_FIFO_DEPTH;
	}

	osMutexRelease(App___GUI_TS_State_Mutex);

	return(ret);
}

static uint32_t GUI___LV_Get_Tick_Callback(void)
{
    return(App___Get_Tick());
}

uint16_t debug_Y;
uint16_t debug_X;

static void GUI___LV_Touch_Read_Callback(lv_indev_t * indev, lv_indev_data_t * data)
{
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    App___GUI_TS_Point_TypeDef point = GUI___TS_Pop_Point();

    if(point.pressed)
    {
        last_x = point.x;
        last_y = point.y;
    }

    data->point.x = last_x;
    data->point.y = last_y;
    data->state = point.pressed ? LV_INDEV_STATE_PRESSED
                                : LV_INDEV_STATE_RELEASED;
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
    osSignalWait(APP___GUI_GFX_FLUSH_READY_FLAG, 2000);
    lv_display_flush_ready(disp);
}

int GUI___Set_Text_If_Changed(lv_obj_t *label, const char *new_text)
{
    const char *current = lv_label_get_text(label);
    if(current && new_text && strcmp(current, new_text) == 0) return false;

    lv_label_set_text(label, new_text);
    return true;
}

uint32_t profile_Length = 115;

void GUI___Select_Pepper(lv_event_t * e)
{
	lv_obj_set_flag(uic_Pepper, LV_OBJ_FLAG_HIDDEN, 0);
	lv_obj_set_flag(uic_Onion, LV_OBJ_FLAG_HIDDEN, 1);
	GUI___Set_Text_If_Changed(uic_Profile_Label, "Carolina Reaper");
	profile_Length = 115;

	osMutexWait(App___Profiles_State_Mutex, osWaitForever);
	App___Profiles_State.initial_Days_Remaining = 115;
	App___Profiles_State.days_Remaining = 115;
	App___Profiles_State.day_Number = 0;

	osMutexRelease(App___Profiles_State_Mutex);

}

void GUI___Select_Onion(lv_event_t * e)
{
	lv_obj_set_flag(uic_Pepper, LV_OBJ_FLAG_HIDDEN, 1);
	lv_obj_set_flag(uic_Onion, LV_OBJ_FLAG_HIDDEN, 0);
	GUI___Set_Text_If_Changed(uic_Profile_Label, "Onion");
	profile_Length = 132;

	osMutexWait(App___Profiles_State_Mutex, osWaitForever);

	App___Profiles_State.initial_Days_Remaining = 135;
	App___Profiles_State.days_Remaining = 135;
	App___Profiles_State.day_Number = 0;

	osMutexRelease(App___Profiles_State_Mutex);
}

void GUI___Play_Pause(lv_event_t * e)
{
	osMutexWait(App___Profiles_State_Mutex, osWaitForever);

	if(App___Profiles_State.lighting_Mode == 0)		// paused mode
	{
		lv_obj_set_flag(uic_Pause_Image, LV_OBJ_FLAG_HIDDEN, 1);
		lv_obj_set_flag(uic_Play_Image, LV_OBJ_FLAG_HIDDEN, 0);

		lv_obj_set_style_bg_color(uic_Progress_Bar, lv_color_hex(0x343604), LV_PART_MAIN);
		lv_obj_set_style_bg_color(uic_Progress_Bar, lv_color_hex(0xFFD600), LV_PART_INDICATOR);

		lv_obj_set_style_text_color(uic_Status_Text, lv_color_hex(0xFFD600), LV_PART_MAIN);
		GUI___Set_Text_If_Changed(uic_Status_Text, "Paused");
	}
	else
	{
		lv_obj_set_flag(uic_Pause_Image, LV_OBJ_FLAG_HIDDEN, 0);
		lv_obj_set_flag(uic_Play_Image, LV_OBJ_FLAG_HIDDEN, 1);

		lv_obj_set_style_bg_color(uic_Progress_Bar, lv_color_hex(0x043616), LV_PART_MAIN);
		lv_obj_set_style_bg_color(uic_Progress_Bar, lv_color_hex(0x07FF00), LV_PART_INDICATOR);

		lv_obj_set_style_text_color(uic_Status_Text, lv_color_hex(0x00FF1D), LV_PART_MAIN);
		//GUI___Set_Text_If_Changed(uic_Status_Text, "Ready To Harvest");
	}

	App___Profiles_State.lighting_Mode ^= 1;
	App___Profiles_State.paused ^= 1;
	osMutexRelease(App___Profiles_State_Mutex);
}

void GUI___Refresh_Lighting_Indicators(void)
{
	uint8_t colors[5];
	osMutexWait(App___IO_Sense_State_Mutex, osWaitForever);
		colors[0] = 	App___IO_Sense_State.lighting_Status.white * 2.55f;
		colors[1] = 	App___IO_Sense_State.lighting_Status.purple * 2.55f;
		colors[2] = 	App___IO_Sense_State.lighting_Status.lime * 2.55f;
		colors[3] = 	App___IO_Sense_State.lighting_Status.red * 2.55f;
		colors[4] = 	App___IO_Sense_State.lighting_Status.far_Red * 2.55f;
	osMutexRelease(App___IO_Sense_State_Mutex);

	_ui_bar_set_property(uic_White_Light_Indicator, 	0, 	colors[0]);
	_ui_bar_set_property(uic_Purple_Light_Indicator, 	0, 	colors[1]);
	_ui_bar_set_property(uic_Lime_Light_Indicator, 		0, 	colors[2]);
	_ui_bar_set_property(uic_Red_Light_Indicator, 		0,  colors[3]);
	_ui_bar_set_property(uic_Far_Red_Light_Indicator, 	0, 	colors[4]);
}

uint8_t GUI___Get_Hour_xHours_Ago(uint8_t hour_Now, uint8_t x)
{
	uint8_t hour_Now_Adjusted = hour_Now-1;
	uint32_t hour_New = (((1200ul + (hour_Now_Adjusted - x)) % 12ul)+1ul);		// 1200 picked because it is > 255+12, and divisible by 12

	return((uint8_t)hour_New);
}

char* GUI___Formated_Time_String(uint8_t hour, uint8_t minute, uint8_t pm, char* buffer)
{
	if(pm == 1)
	{
		sprintf(buffer, "%d:%02d PM", hour, minute);
	}
	else if(pm == 0)
	{
		sprintf(buffer, "%d:%02d AM", hour, minute);
	}
	else
	{
		sprintf(buffer, "%d:%02d", hour, minute);
	}

	return(buffer);
}

void GUI___Refresh_Time()
{
	App___Time_TypeDef time = App___Get_Time();
	App___Date_TypeDef date = App___Get_Date();

	lv_calendar_date_t shown_date =
	{
	    .year  	= date.year,
	    .month 	= date.month,
		.day	= date.day
	};

	lv_calendar_set_today_date(uic_Calendar, shown_date.year, shown_date.month, shown_date.day);

	char time_Str[32];

//	GUI___Set_Text_If_Changed(uic_Graph_7HAgo, GUI___Formated_Time_String(GUI___Get_Hour_xHours_Ago(time.hour, 7), time.minute, 2, time_Str));
//	GUI___Set_Text_If_Changed(uic_Graph_6HAgo, GUI___Formated_Time_String(GUI___Get_Hour_xHours_Ago(time.hour, 6), time.minute, 2, time_Str));
//	GUI___Set_Text_If_Changed(uic_Graph_5HAgo, GUI___Formated_Time_String(GUI___Get_Hour_xHours_Ago(time.hour, 5), time.minute, 2, time_Str));
//	GUI___Set_Text_If_Changed(uic_Graph_4HAgo, GUI___Formated_Time_String(GUI___Get_Hour_xHours_Ago(time.hour, 4), time.minute, 2, time_Str));
//	GUI___Set_Text_If_Changed(uic_Graph_3HAgo, GUI___Formated_Time_String(GUI___Get_Hour_xHours_Ago(time.hour, 3), time.minute, 2, time_Str));
//	GUI___Set_Text_If_Changed(uic_Graph_2HAgo, GUI___Formated_Time_String(GUI___Get_Hour_xHours_Ago(time.hour, 2), time.minute, 2, time_Str));
//	GUI___Set_Text_If_Changed(uic_Graph_1HAgo, GUI___Formated_Time_String(GUI___Get_Hour_xHours_Ago(time.hour, 1), time.minute, 2, time_Str));

	GUI___Set_Text_If_Changed(uic_Main_Clock, GUI___Formated_Time_String(time.hour, time.minute, time.pm, time_Str));

	osMutexWait(App___Profiles_State_Mutex, osWaitForever);

	char buffer[16];

	sprintf(buffer, "%d days", App___Profiles_State.days_Remaining);

	GUI___Set_Text_If_Changed(uic_Time_Remaining, buffer);

	sprintf(buffer, "%d days", App___Profiles_State.day_Number);

	GUI___Set_Text_If_Changed(uic_Time_Since_Start, buffer);

	if(App___Profiles_State.lighting_Mode == 0)
	{
		if(App___Profiles_State.days_Remaining < 10)
		{
			GUI___Set_Text_If_Changed(uic_Status_Text, "Ready To Harvest");
		}
		else if(App___Profiles_State.day_Number < 10)
		{
			GUI___Set_Text_If_Changed(uic_Status_Text, "Seedling");
		}
		else
		{
			GUI___Set_Text_If_Changed(uic_Status_Text, "Growing");
		}
	}

	_ui_bar_set_property(uic_Progress_Bar, 	0, 	((float)App___Profiles_State.day_Number / ((float)App___Profiles_State.days_Remaining + (float)App___Profiles_State.day_Number)) * 100.0f);

	osMutexRelease(App___Profiles_State_Mutex);

	_ui_label_set_property(uic_Calendar_Clock, 0, time_Str);

}

void GUI___TS_Start_Task(void const * argument)
{
	osSignalWait(APP___GUI_TS_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		osSignalWait(APP___GUI_TS_EVENT_FLAG, osWaitForever);
		uint16_t x = 0;
		uint16_t y = 0;
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

		new_X = x;
		new_Y = y;

		App___GUI_TS_Point_TypeDef point;

		point.x = new_X;
		point.y = new_Y;
		point.pressed = pressed;


		GUI___TS_Push_Point(point);
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

void GUI___Backlight_Changed_Callback(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
	App___Set_Backlight_Brightness(lv_slider_get_value(slider));
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
	lv_obj_add_event_cb(uic_Backlight_Brightness, GUI___Backlight_Changed_Callback, LV_EVENT_VALUE_CHANGED, NULL);

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
