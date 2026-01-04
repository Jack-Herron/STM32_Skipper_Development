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

#define FLUSH_READY_FLAG (1U << 0)
#define TS_EVENT_FLAG (1 << 0)

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t  pressed;
} ts_sample_t;

static volatile ts_sample_t g_ts = {0};
static osMutexId g_ts_mutex;
osMutexDef(TS_MUTEX);

uint32_t* frame_buffer_address = (uint32_t*)0xc0000000;

uint32_t SystemCoreClock = HCLK_FREQ;

lv_display_t * display = NULL;

osThreadId defaultTaskHandle;
osThreadId GFXTaskHandle;

uint8_t flush_ready = 0;

void StartDefaultTask(void const * argument);
void startGFXTask(void const * argument);

void DSI_Buffer_Swap_Callback(void)
{
	osSignalSet(GFXTaskHandle, FLUSH_READY_FLAG);
}

void TS_Event_Callback(void)
{
	osSignalSet(defaultTaskHandle, TS_EVENT_FLAG);
}

int main(void)
{
	clock_Init();
	millis_Init();
	FMC_SDRAM___SDRAM_Init();

	/* CRUDE FRAMEBUFFER INITIALIZATION */
	for(uint32_t i = 0; i < (LCD_BUFFER_SIZE/4)*2; i++)
	{
		((uint32_t*)LCD_BUFFER1_ADDRESS)[i] = 0xFF000000; // Fill the framebuffer with black
	}

	DSI_LCD___Init();
	DSI_LCD___Set_Swap_Callback(DSI_Buffer_Swap_Callback);
	TS___Init();
	TS___Set_Event_Callback(TS_Event_Callback);

	//TODO add init functions for QSPI

	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 4096);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);


	osThreadDef(GFXTask, startGFXTask, osPriorityNormal, 0, 16384);
	GFXTaskHandle = osThreadCreate(osThread(GFXTask), NULL);
	g_ts_mutex = osMutexCreate(osMutex(TS_MUTEX));

	osKernelStart();

	for(;;);
}

void StartDefaultTask(void const * argument)
{

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
    osSignalWait(FLUSH_READY_FLAG, 100);
    lv_display_flush_ready(disp);
}

void startGFXTask(void const * argument)
{
	lv_init();

	display = lv_display_create(LCD_HORIZONTAL_RESOLUTION, LCD_VERTICAL_RESOLUTION);

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
		uint32_t t = lv_timer_handler();
		osDelay(t);
	}
}
