/*
 * USB_LL_Interrupts_Host.h
 *
 *  Created on: Jun 27, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_INTERRUPTS_HOST_H_
#define INC_USB_LL_INTERRUPTS_HOST_H_
#define USB_LL_Interrupts_Host___PORT_INTERRUPTS_MASK						0x000a
#define USB_LL_Interrupts_Host___CHANNEL_INTERRUPTS_MASK					0x029b

typedef enum {
	Channel_Status_Enum___IDLE,
	Channel_Status_Enum___TRANSFER_COMPLETE,
	Channel_Status_Enum___STALL,
	Channel_Status_Enum___HALT,
	Channel_Status_Enum___NAK,
	Channel_Status_Enum___ERROR
} USB_LL_Interrupts_Host___Channel_Status_Enum;

typedef struct {
	USB_LL_Interrupts_Host___Channel_Status_Enum 		status;
	uint8_t 											device_Address;
	uint8_t*											p_Data;
	uint32_t											data_Length;
} USB_LL_Interrupts_Host___Channel_Status_TypeDef;

typedef struct {
	USB_LL_Interrupts___Status_TypeDef*					port_Status;
	USB_LL_Interrupts_Host___Channel_Status_TypeDef		channel_Status[USB_LL_Definitions___MAX_NUMBER_OF_CHANNELS];
} USB_LL_Interrupts_Host___Status_TypeDef;

void USB_LL_Interrupts_Host___Channel_Interrupt_Handler(uint8_t port_Number);
void USB_LL_Interrupts_Host___Port_Interrupt_Handler(uint8_t port_Number);
void USB_LL_Interrupts_Host___Packet_Received(uint8_t port_Number);
#endif /* INC_USB_LL_INTERRUPTS_HOST_H_ */
