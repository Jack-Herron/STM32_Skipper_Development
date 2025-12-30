/*
 * DSI_LCD.h
 *
 *  Created on: Dec 22, 2025
 *      Author: jackh
 */

#ifndef INC_DSI_LCD_H_
#define INC_DSI_LCD_H_



#define DSI_LCD___LANE_PERIODX4			8 	// Lane period in 0.25ns increments (for 500Mbps: LANEPERIODX4 = (period[ns] * 4) = (2ns*4) = 8)
#define DSI_LCD___LANE_COUNT			2 	// number of data lanes, 1: 1 lane, 2: 2 lanes
#define DSI_LCD___DATA_HS2LP_TIME		15	// Max high speed to low power mode switch time in lengths of byte lane periods		 One byte-cycle period = 8 * UI, where UI = 1 / lane_bitrate (seconds per bit)
#define DSI_LCD___CLOCK_HS2LP_TIME 		28	// same as data HS2LP time, but for the clock lane
#define DSI_LCD___DATA_LP2HS_TIME		25	// same idea
#define DSI_LCD___CLOCK_LP2HS_TIME		33	// same idea
#define DSI_LCD___STOP_WAIT_TIME		10 	// the minimum wait period to request a high-speed transmission after the Stop state
#define DSI_LCD___TX_ESCAPE_CLOCK_DIV	4	// indicates the division factor for the TX escape clock source (lanebyteclk)
#define DSI_LCD___TX_TIMEOUT_CLOCK_DIV	1	// indicates the division factor for the timeout clock

#define DSI_LCD___DATA_TYPE_LONG_WRITE		0x39	// DCS mode for long packet write
#define DSI_LCD___DATA_TYPE_SHORT_WRITE_1P	0x15	// DCS mode for short packet write with one parameter
#define DSI_LCD___DATA_TYPE_SHORT_WRITE_0P	0x05	// DCS mode for short packet write with one parameter
#define DSI_LCD___PIXEL_FORMAT_RGB888		0x77 	// DCS code for RGB888
#define DSI_LCD___CMD_SLEEPOUT				0x11
#define DSI_LCD___CMD_SET_PIXEL_FORMAT		0x3A	// DCS Register to set the color format
#define DSI_LCD___CMD_SET_MADCTR			0x36	// Register for MADCTR
#define DSI_LCD___MADCTR_A0					0xA0
#define DSI_LCD___CMD_DISPLAY_ON			0x29
#define DSI_LCD___CMD_CABC_SET_MODE			0x55
#define DSI_LCD___CABC_MODE_STILL_PICTURE	0x02
#define DSI_LCD___CMD_ALLPON				0x23

void DSI_LCD___Long_Write(uint32_t channel_ID, uint32_t data_Type, uint8_t *pdata, uint32_t size);
void DSI_LCD___Short_Write(uint32_t channel_ID, uint32_t data_Type, uint32_t data1, uint32_t data2);
void DSI_LCD___Init(void);

#endif /* INC_DSI_LCD_H_ */
