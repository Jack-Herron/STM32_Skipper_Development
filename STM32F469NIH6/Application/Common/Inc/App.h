/*
 * App.h
 *
 *  Created on: Jan 5, 2026
 *      Author: Jack Herron
 */

#ifndef COMMON_INC_APP_H_
#define COMMON_INC_APP_H_

#include <stdint.h>
#include "cmsis_os.h"
#include "lvgl.h"

#define SPP___GUI_HORIZONTAL_RESOLUTION 800
#define APP___GUI_VERTICAL_RESOLUTION 	480
#define APP___GUI_BUFFER_SIZE 			(LCD_VERTICAL_RESOLUTION * LCD_HORIZONTAL_RESOLUTION * 2)
#define APP___GUI_BUFFER1_ADDRESS 		(uint8_t*)(0xc0000000)
#define APP___GUI_BUFFER2_ADDRESS 		(uint8_t*)(LCD_BUFFER1_ADDRESS + LCD_BUFFER_SIZE)

// module data structures

// IO structures
enum App___IO_Control_Lighting_Mode_Enum
{
	lighting_Mode_Profile,
	lighting_Mode_Bright,
	lighting_Mode_Off
};

typedef struct {
	uint8_t 	white;
	uint8_t 	purple;
	uint8_t 	lime;
	uint8_t 	red;
	uint8_t 	far_Red;
	uint8_t 	mode;
} App___IO_Control_Lighting_State_TypeDef;

typedef struct {
	App___IO_Control_Lighting_State_TypeDef lighting;
} App___IO_Control_State_TypeDef;

typedef struct {
	uint8_t tmp;
} App___IO_Sensor_State_TypeDef;

// GUI typedefs
typedef struct {
	uint8_t tmp;
} App___GUI_GFX_State_TypeDef;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t  pressed;
} App___GUI_TS_Point_TypeDef;

typedef struct {
	App___GUI_TS_Point_TypeDef last_Point;
} App___GUI_TS_State_TypeDef;

// Profiles typedefs
typedef struct {
	uint8_t tmp;
} App___Profiles_State_TypeDef;

extern volatile App___Profiles_State_TypeDef 	App___Profiles_State;
extern volatile App___GUI_TS_State_TypeDef 		App___GUI_TS_State;
extern volatile App___GUI_GFX_State_TypeDef	 	App___GUI_GFX_State;
extern volatile App___IO_Sensor_State_TypeDef 	App___IO_Sensor_State;
extern volatile App___IO_Control_State_TypeDef 	App___IO_Control_State;

extern osMutexId App___IO_Control_State_Mutex;
extern osMutexId App___IO_Sensor_State_Mutex;
extern osMutexId App___GUI_GFX_State_Mutex;
extern osMutexId App___GUI_TS_State_Mutex;
extern osMutexId App___Profiles_State_Mutex;

extern uint32_t App___GUI_Width;
extern uint32_t App___GUI_Height;

extern void* App___GUI_Buffer1_Ptr;
extern void* App___GUI_Buffer2_Ptr;

extern uint32_t App___GUI_Buffer_Size;

// Graphics task flags
#define APP___GUI_GFX_TASK_START_FLAG (1 << 0)
#define APP___GUI_GFX_FLUSH_READY_FLAG (1U << 1)

// Profile task flags
#define APP___PROFILES_TASK_START_FLAG (1 << 0)

// Touch screen task flags
#define APP___GUI_TS_TASK_START_FLAG (1 << 0)
#define APP___GUI_TS_EVENT_FLAG (1U << 1)

// Sensor task flags
#define APP___SENSOR_TASK_START_FLAG (1 << 0)

// Control task flags
#define APP___CONTROL_TASK_START_FLAG (1 << 0)

void App___Init(void);
void App___Set_Get_Tick_Callback(uint32_t (*tick_CallBack)(void));
void App___GUI_TS_Event_Detected();
void App___Frame_Ready(uint8_t* buf);
void App___Set_GUI_GFX_Frame_Ready_CallBack(void (*frame_Ready_CallBack)(uint8_t* buf));
void App___GUI_GFX_Buffer_Swap_Ready(void);
void App___GUI_Set_Dimentions(const uint32_t width, const uint32_t height);
void App___GUI_Set_Buffers(void* buf1, void* buf2, uint32_t size);
uint8_t App___GUI_TS_Get_Point(uint16_t* x, uint16_t* y);
void App___Set_GUI_TS_Get_Point_CallBack(uint8_t (*get_Point_Callback)(uint16_t* x, uint16_t* y));

// functions
uint32_t App___Get_Tick(void);

#endif /* COMMON_INC_APP_H_ */
