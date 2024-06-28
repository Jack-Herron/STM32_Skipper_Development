/*
 * USB_LL_Interrupts.h
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_INTERRUPTS_H_
#define INC_USB_LL_INTERRUPTS_H_

#define USB_LL_Interrupts___WORD_32_MSB								0x1f
#define USB_LL_Interrupts___GLOBAL_INTERRUPTS_MASK					0xa33c7c98
#define USB_LL_Interrupts___IN_ENDPOINT_INTERRUPTS_MASK				0x000b
#define USB_LL_Interrupts___OUT_ENDPOINT_INTERRUPTS_MASK			0x000b
#define USB_LL_Interrupts___LOW_SPEED_VALUE							0x00
#define USB_LL_Interrupts___FULL_SPEED_VALUE						0x01
#define USB_LL_Interrupts___HIGH_SPEED_VALUE						0x02

typedef struct {
	uint8_t 									start_Of_Frame;
	uint8_t 									root_Device_Connected;
	uint8_t 									root_Device_Disconnected;
	uint8_t 									root_Device_Connected_Speed;
} USB_LL_Interrupts___Status_TypeDef;

#endif /* INC_USB_LL_INTERRUPTS_H_ */
