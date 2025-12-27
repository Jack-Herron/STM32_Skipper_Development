/*
 * DSI_LCD.h
 *
 *  Created on: Dec 22, 2025
 *      Author: jackh
 */

#ifndef INC_DSI_LCD_H_
#define INC_DSI_LCD_H_

void DSI_LCD_Init(void);

#define DSI_LCD___LANE_PERIODX4			8 	// Lane period in 0.25ns increments (for 500Mbps: LANEPERIODX4 = (period[ns] * 4) = (2ns*4) = 8)
#define DSI_LCD___LANE_COUNT			2 	// number of data lanes, 1: 1 lane, 2: 2 lanes
#define DSI_LCD___DATA_HS2LP_TIME		15	// Max high speed to low power mode switch time in lengths of byte lane periods		 One byte-cycle period = 8 * UI, where UI = 1 / lane_bitrate (seconds per bit)
#define DSI_LCD___CLOCK_HS2LP_TIME 		28	// same as data HS2LP time, but for the clock lane
#define DSI_LCD___DATA_LP2HS_TIME		25	// same idea
#define DSI_LCD___CLOCK_LP2HS_TIME		33	// same idea
#define DSI_LCD___STOP_WAIT_TIME		10 	// the minimum wait period to request a high-speed transmission after the Stop state
#define DSI_LCD___TX_ESCAPE_CLOCK_DIV	4	// indicates the division factor for the TX escape clock source (lanebyteclk)
#define DSI_LCD___TX_TIMEOUT_CLOCK_DIV	1	// indicates the division factor for the timeout clock


#endif /* INC_DSI_LCD_H_ */
