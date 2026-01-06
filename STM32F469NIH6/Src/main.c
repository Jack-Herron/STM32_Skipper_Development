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
#include "I2C.h"
#include "TS.h"
#include "App.h"

uint32_t SystemCoreClock = HCLK_FREQ;

void StartControlTask(void const * argument);
void StartSensorTask(void const * argument);
void StartProfileTask(void const * argument);
void StartTSTask(void const * argument);
void startGFXTask(void const * argument);

void DSI_Buffer_Swap_Callback(void)
{
	App___GUI_GFX_Buffer_Swap_Ready();
}

void TS_Event_Callback(void)
{
	App___GUI_TS_Event_Detected();
}

void frame_Ready_Callback(uint8_t* buf)
{
	DSI_LCD___Set_Pending_Buffer(buf);
}

static uint32_t tick_Callback(void)
{
    return(clock___millis());
}

uint8_t TS_Get_Point_Callback(uint16_t* x, uint16_t* y)
{
	uint8_t num_Points = TS___Get_Num_Points_Pressed();
	if(num_Points > 0)
	{
		TS___Get_Point(x, y);
	}

	return(num_Points);
}

int main(void)
{
	clock_Init();
	millis_Init();
	FMC_SDRAM___SDRAM_Init();
	DSI_LCD___Init();
	DSI_LCD___Set_Swap_Callback(DSI_Buffer_Swap_Callback);
	TS___Init();
	TS___Set_Event_Callback(TS_Event_Callback);

	App___GUI_Set_Buffers((void*)0xc0000000, (void*)(0xc0000000 + 768000), 768000);
	App___GUI_Set_Dimentions(800, 480);
	App___Set_GUI_GFX_Frame_Ready_CallBack(frame_Ready_Callback);
	App___Set_Get_Tick_Callback(tick_Callback);

	App___Set_GUI_TS_Get_Point_CallBack(TS_Get_Point_Callback);

	App___Init();

	osKernelStart();

	for(;;);
}

