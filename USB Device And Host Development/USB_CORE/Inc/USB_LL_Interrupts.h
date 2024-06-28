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

typedef enum {
	USB_Proccess_Status___IDLE,
	USB_Proccess_Status___TRANSFER_COMPLETE,
	USB_Proccess_Status___SETUP_TRANSFER_COMPLETE,
	USB_Proccess_Status___STALL,
	USB_Proccess_Status___HALT,
	USB_Proccess_Status___NAK,
	USB_Proccess_Status___ERROR
} USB_LL_Interrupts___CH_EP_Status_Enum;

typedef struct {
	USB_LL_Interrupts___CH_EP_Status_Enum 		status;
	uint8_t 									device_Address;
	uint8_t*									p_Data;
	uint32_t									data_Length;
} USB_LL_Interrupts___CH_EP_Status_TypeDef;

typedef struct {
	uint8_t 									start_Of_Frame;
	uint8_t 									root_Device_Connected;
	uint8_t 									root_Device_Disconnected;
	uint8_t 									root_Device_Connected_Speed;
	uint16_t 									host_Channel_Device_Endpoint_Status;
	USB_LL_Interrupts___CH_EP_Status_TypeDef 	CH_EP_Status[USB_LL_Definitions___MAX_CHANNELS_OR_ENDPOINTS];
} USB_LL_Interrupts___Status_TypeDef;

#endif /* INC_USB_LL_INTERRUPTS_H_ */
