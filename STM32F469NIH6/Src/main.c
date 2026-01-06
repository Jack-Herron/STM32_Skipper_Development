// Jack Herron


#define LV_LVGL_H_INCLUDE_SIMPLE


#include <stdint.h>
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "Clock.h"
#include "FMC_SDRAM.h"
#include "QSPI.h"
#include "DSI_LCD.h"
#include "lvgl.h"
#include "stdio.h"
#include "ui.h"
#include "lv_draw_dma2d.h"
#include "I2C.h"
#include "TS.h"

#define LCD_HORIZONTAL_RESOLUTION 800
#define LCD_VERTICAL_RESOLUTION 480
#define LCD_BUFFER_SIZE (LCD_VERTICAL_RESOLUTION * LCD_HORIZONTAL_RESOLUTION * 2)
#define LCD_BUFFER1_ADDRESS (uint8_t*)(0xc0000000)
#define LCD_BUFFER2_ADDRESS (uint8_t*)(LCD_BUFFER1_ADDRESS + LCD_BUFFER_SIZE)

#define FLUSH_READY_FLAG (1U << 1)
#define TS_EVENT_FLAG (1U << 1)

#define GFX_TASK_START_FLAG (1 << 0)
#define PROFILE_TASK_START_FLAG (1 << 0)
#define TS_TASK_START_FLAG (1 << 0)
#define SENSOR_TASK_START_FLAG (1 << 0)
#define CONTROL_TASK_START_FLAG (1 << 0)

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t  pressed;
} ts_sample_t;

typedef struct {
	uint8_t 	white;
	uint8_t 	purple;
	uint8_t 	lime;
	uint8_t 	red;
	uint8_t 	far_Red;
	uint8_t 	mode;
} lighting_Channels_TypeDef;

static volatile lighting_Channels_TypeDef lighting = {0};
static osMutexId lighting_Channels_Mutex;
osMutexDef(LIGHTING_MUTEX);

static volatile ts_sample_t g_ts = {0};
static osMutexId g_ts_mutex;
osMutexDef(TS_MUTEX);

uint32_t SystemCoreClock = HCLK_FREQ;

osThreadId HWTaskHandle;
osThreadId controlTaskHandle;
osThreadId sensorHandle;
osThreadId profileTaskHandle;
osThreadId TSTaskHandle;
osThreadId GFXTaskHandle;

uint8_t flush_ready = 0;

void StartHWTask(void const * argument);
void StartControlTask(void const * argument);
void StartSensorTask(void const * argument);
void StartProfileTask(void const * argument);
void StartTSTask(void const * argument);
void startGFXTask(void const * argument);


void DSI_Buffer_Swap_Callback(void)
{
	osSignalSet(GFXTaskHandle, FLUSH_READY_FLAG);
}

void TS_Event_Callback(void)
{
	osSignalSet(TSTaskHandle, TS_EVENT_FLAG);
}

int main(void)
{
	clock_Init();

	osThreadDef(HWTask, StartHWTask, osPriorityNormal, 0, 512);
	HWTaskHandle = osThreadCreate(osThread(HWTask), NULL);

	osThreadDef(sensorTask, StartSensorTask, osPriorityNormal, 0, 512);
	sensorHandle = osThreadCreate(osThread(sensorTask), NULL);

	osThreadDef(controlTask, StartControlTask, osPriorityNormal, 0, 512);
	controlTaskHandle = osThreadCreate(osThread(controlTask), NULL);

	osThreadDef(profileTask, StartProfileTask, osPriorityNormal, 0, 4096);
	profileTaskHandle = osThreadCreate(osThread(profileTask), NULL);

	osThreadDef(GFXTask, startGFXTask, osPriorityNormal, 0, 16384);
	GFXTaskHandle = osThreadCreate(osThread(GFXTask), NULL);

	osThreadDef(TSTask, StartTSTask, osPriorityNormal, 0, 512);
	TSTaskHandle = osThreadCreate(osThread(TSTask), NULL);

	lighting_Channels_Mutex = osMutexCreate(osMutex(LIGHTING_MUTEX));
	g_ts_mutex = osMutexCreate(osMutex(TS_MUTEX));

	osKernelStart();

	for(;;);
}

void StartHWTask(void const * argument)
{
	millis_Init();
	FMC_SDRAM___SDRAM_Init();
	DSI_LCD___Init();
	DSI_LCD___Set_Swap_Callback(DSI_Buffer_Swap_Callback);
	TS___Init();
	TS___Set_Event_Callback(TS_Event_Callback);

	osSignalSet(GFXTaskHandle, GFX_TASK_START_FLAG);
	osSignalSet(profileTaskHandle, PROFILE_TASK_START_FLAG);
	osSignalSet(TSTaskHandle, TS_TASK_START_FLAG);
	osSignalSet(sensorHandle, SENSOR_TASK_START_FLAG);
	osSignalSet(controlTaskHandle, CONTROL_TASK_START_FLAG);

	for (;;)
	{
		osDelay(100);
	}
}

void StartControlTask(void const * argument)
{
	osSignalWait(CONTROL_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		osDelay(100);
	}
}

void StartSensorTask(void const * argument)
{
	osSignalWait(SENSOR_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		osDelay(100);
	}
}

void StartProfileTask(void const * argument)
{
	osSignalWait(PROFILE_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		// simulated profile reads
		osMutexWait(lighting_Channels_Mutex, osWaitForever);
		if(lighting.mode == 0)
		{
			lighting.white 		= 100;
			lighting.purple 	= 180;
			lighting.lime 		= 90;
			lighting.red 		= 200;
			lighting.far_Red 	= 70;
		}
		else
		{
			lighting.white 		= 255;
			lighting.purple 	= 0;
			lighting.lime 		= 0;
			lighting.red 		= 0;
			lighting.far_Red 	= 0;
		}
		osMutexRelease(lighting_Channels_Mutex);

		//TODO implement QSPI + littlefs reads

		osDelay(100);
	}
}

void StartTSTask(void const * argument)
{
	osSignalWait(TS_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		osSignalWait(TS_EVENT_FLAG, osWaitForever);

		uint16_t x;
		uint16_t y;
		uint8_t pressed = 0;
		uint8_t num_Points = 0;

		num_Points = TS___Get_Num_Points_Pressed();
		TS___Get_Point(&x, &y);

		if(num_Points != 0)
		{
			pressed = 1;
		}

		// remap points to LVGLs format

		uint16_t new_X;
		uint16_t new_Y;

		new_X = y;
		new_Y = 479 - x;

		osMutexWait(g_ts_mutex, osWaitForever);
		g_ts.x = new_X;
		g_ts.y = new_Y;
		g_ts.pressed = pressed;
		osMutexRelease(g_ts_mutex);
	}
}

static uint32_t my_tick_cb(void)
{
    return(clock___millis());
}

static void my_touch_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
	osMutexWait(g_ts_mutex, osWaitForever);

	data->point.x = g_ts.x;
	data->point.y = g_ts.y;
	data->state = g_ts.pressed;

	osMutexRelease(g_ts_mutex);
}

static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
	if(lv_disp_flush_is_last(disp))
	{
		DSI_LCD___Set_Pending_Buffer(px_map);
	}
	else
	{
		lv_display_flush_ready(disp);
	}
}

void my_flush_wait_cb(lv_display_t * disp)
{
    osSignalWait(FLUSH_READY_FLAG, 1000);
    lv_display_flush_ready(disp);
}

void GUI_Toggle_Bright_Mode(lv_event_t * e)
{
	osMutexWait(lighting_Channels_Mutex, osWaitForever);

	lighting.mode ^= 1;

	osMutexRelease(lighting_Channels_Mutex);
}

void GUI_Refresh_Lighting_Indicators(void)
{
	osMutexWait(lighting_Channels_Mutex, osWaitForever);

	_ui_bar_set_property(uic_White_Light_Indicator, 	0, 	lighting.white);
	_ui_bar_set_property(uic_Purple_Light_Indicator, 	0, 	lighting.purple);
	_ui_bar_set_property(uic_Lime_Light_Indicator, 		0, 	lighting.lime);
	_ui_bar_set_property(uic_Red_Light_Indicator, 		0, 	lighting.red);
	_ui_bar_set_property(uic_Far_Red_Light_Indicator, 	0, 	lighting.far_Red);

	osMutexRelease(lighting_Channels_Mutex);
}

void startGFXTask(void const * argument)
{
	osSignalWait(GFX_TASK_START_FLAG, osWaitForever);

	lv_init();

	lv_display_t* display = lv_display_create(LCD_HORIZONTAL_RESOLUTION, LCD_VERTICAL_RESOLUTION);

	lv_display_set_buffers(display, LCD_BUFFER1_ADDRESS, LCD_BUFFER2_ADDRESS, LCD_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_DIRECT);

	lv_tick_set_cb(my_tick_cb);
	lv_display_set_flush_cb(display, my_flush_cb);
	lv_display_set_flush_wait_cb(display, my_flush_wait_cb);

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read_cb);

	ui_init();

	for(;;)
	{
		GUI_Refresh_Lighting_Indicators();
		uint32_t t = lv_timer_handler();
		osDelay(t);
	}
}
