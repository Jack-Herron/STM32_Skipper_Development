/*
 * DSI_LCD.h
 *
 *  Created on: Dec 22, 2025
 *      Author: jackh
 */

#ifndef INC_DSI_LCD_H_
#define INC_DSI_LCD_H_

// -- configuration values --

#define DSI_LCD___HACT					800	// Display horizontal active size 		(pixels)
#define DSI_LCD___HSA					2	// Display horizontal sync size 		(pixels)
#define DSI_LCD___HFP					34	// Display horizontal front porch size 	(pixels)
#define DSI_LCD___HBP					34	// Display horizontal back porch size 	(pixels)
#define DSI_LCD___VACT					480	// Display vertical actual size 		(pixels)
#define DSI_LCD___VSA					120	// Display vertical sync size 			(pixels)
#define DSI_LCD___VFP					150	// Display vertical front porch size 	(pixels)
#define DSI_LCD___VBP					150	// Display vertical back porch size 	(pixels)

#define DSI_LDC___LANE_FREQUENCY		500000000

#define DSI_LCD___LANE_COUNT				0x02 	// number of data lanes, 1: 1 lane, 2: 2 lanes
#define DSI_LCD___DATA_HS2LP_TIME			0x23	// Max high speed to low power mode switch time in lengths of byte lane periods		 One byte-cycle period = 8 * UI, where UI = 1 / lane_bitrate (seconds per bit)
#define DSI_LCD___CLOCK_HS2LP_TIME 			0x23	// same as data HS2LP time, but for the clock lane
#define DSI_LCD___DATA_LP2HS_TIME			0x23	// same idea
#define DSI_LCD___CLOCK_LP2HS_TIME			0x23	// same idea
#define DSI_LCD___DATA_MAXIMUM_READ_TIME	0x00	// Read timeout in lane byte periods

#define DSI_LCD___STOP_WAIT_TIME			0x0a 	// the minimum wait period to request a high-speed transmission after the Stop state
#define DSI_LCD___TX_ESCAPE_CLOCK_DIV		4		// indicates the division factor for the TX escape clock source (lanebyteclk)
#define DSI_LCD___TX_TIMEOUT_CLOCK_DIV		0		// indicates the division factor for the timeout clock

// -- Calculated values --

#define DSI_LCD___LANE_BYTE_CLK				DSI_LDC___LANE_FREQUENCY / 8
#define DSI_LCD___LANE_PERIODX4				(uint32_t)((1.0/DSI_LDC___LANE_FREQUENCY)*1e9*4) // Lane period in 0.25ns increments

#define DSI_LCD___HLINE						(uint32_t)((DSI_LCD___HACT + DSI_LCD___HSA + DSI_LCD___HBP + DSI_LCD___HFP) * ((uint64_t)DSI_LCD___LANE_BYTE_CLK) / ((uint64_t)LCDTFT_FREQ))
#define DSI_LCD___HSA_FOR_DSI				(uint32_t)((DSI_LCD___HSA) * ((uint64_t)DSI_LCD___LANE_BYTE_CLK) / ((uint64_t)LCDTFT_FREQ))
#define DSI_LCD___HBP_FOR_DSI				(uint32_t)((DSI_LCD___HBP) * ((uint64_t)DSI_LCD___LANE_BYTE_CLK) / ((uint64_t)LCDTFT_FREQ))

#define DSI_LCD___DATA_TYPE_SHORT_WRITE_1P 	0x15
#define DSI_LCD___DATA_TYPE_LONG_WRITE		0x39

// Register Fill values

#define DSI_LCD___LCOLC_RGB888 				0x05
#define DSI_LCD___COLMUX_RGB888				0x0A
#define DSI_LCD___VMT_MODE_BURST			0x02

// Functions

void DSI_LCD___Long_Write(uint32_t channel_ID, uint32_t data_Type, uint8_t *pdata, uint32_t size);
void DSI_LCD___Short_Write(uint32_t channel_ID, uint32_t data_Type, uint32_t data1, uint32_t data2);
void DSI_LCD___Init(void);

#endif /* INC_DSI_LCD_H_ */
