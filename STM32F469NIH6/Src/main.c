// Jack Herron


#define LV_LVGL_H_INCLUDE_SIMPLE


#include <stdint.h>
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "Clock.h"
#include "FMC_SDRAM.h"
#include "QSPI_Flash.h"
#include "DSI_LCD.h"
#include "lvgl.h"
#include "stdio.h"
#include "ui.h"
#include "lv_draw_dma2d.h"

#define LCD_HORIZONTAL_RESOLUTION 800
#define LCD_VERTICAL_RESOLUTION 480
#define LCD_BUFFER_SIZE (LCD_VERTICAL_RESOLUTION * LCD_HORIZONTAL_RESOLUTION * 4)
#define LCD_BUFFER1_ADDRESS (uint8_t*)(0xc0000000)
#define LCD_BUFFER2_ADDRESS (uint8_t*)(LCD_BUFFER1_ADDRESS + LCD_BUFFER_SIZE)


uint32_t* frame_buffer_address = (uint32_t*)0xc0000000;

uint32_t SystemCoreClock = HCLK_FREQ;

lv_display_t * display = NULL;

osThreadId defaultTaskHandle;
osThreadId GFXTaskHandle;

#define FPS_SAMPLES 150
uint32_t FPS_Array[FPS_SAMPLES];
float FPS_Value = 0;
uint32_t frame_Counter = 0;
void StartDefaultTask(void const * argument);
void startGFXTask(void const * argument);

void DSI_Buffer_Swap_Callback(void)
{
    lv_display_flush_ready(display);

    uint32_t now = clock___millis();
    uint32_t idx = frame_Counter % FPS_SAMPLES;

    // CRUDE FPS CALCULATION
    FPS_Array[idx] = now;
    frame_Counter++;

    if(frame_Counter >= FPS_SAMPLES)
    {
        /* Oldest sample is the next one that will be overwritten */
        uint32_t oldest_idx = frame_Counter % FPS_SAMPLES;
        uint32_t oldest = FPS_Array[oldest_idx];

        uint32_t dt = now - oldest;
        if(dt > 0)
        	FPS_Value = 1000.0f * (float)(FPS_SAMPLES - 1) / (float)dt;
    }
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
	//TODO add init functions for QSPI, DMA2D (ChromArt)

	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 4096);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);


	osThreadDef(GFXTask, startGFXTask, osPriorityNormal, 0, 16384);
	GFXTaskHandle = osThreadCreate(osThread(GFXTask), NULL);

	osKernelStart();

	for(;;);
}

void StartDefaultTask(void const * argument)
{

	for(;;)
	{
		osDelay(100);

	}
}

static uint32_t my_tick_cb(void)
{
    return(clock___millis());
}

static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
	//TODO: after dual buffer integration, this function needs to wait for tearing signal, and swap buffers
	if(lv_disp_flush_is_last(disp))
	{
		DSI_LCD___Set_Pending_Buffer(px_map);
	}
	else
	{
		lv_display_flush_ready(disp);
	}
}

void startGFXTask(void const * argument)
{
	lv_init();

	display = lv_display_create(LCD_HORIZONTAL_RESOLUTION, LCD_VERTICAL_RESOLUTION);

	lv_display_set_buffers(display, LCD_BUFFER1_ADDRESS, LCD_BUFFER2_ADDRESS, LCD_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_DIRECT);
	lv_tick_set_cb(my_tick_cb);
	lv_display_set_flush_cb(display, my_flush_cb);

	ui_init();
	for(;;)
	{
		uint32_t time_Until_Refresh = lv_timer_handler();
		char str[30] = "Initializing...";
		sprintf(str, "FPS: %.2f", FPS_Value);

		lv_label_set_text(uic_FPS_DISPLAY, str);
		osDelay(time_Until_Refresh);
	}
}

