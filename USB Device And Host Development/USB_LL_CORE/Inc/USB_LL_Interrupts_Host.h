/*
 * USB_LL_Interrupts_Host.h
 *
 *  Created on: Jun 27, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_INTERRUPTS_HOST_H_
#define INC_USB_LL_INTERRUPTS_HOST_H_

#define USB_LL_Interrupts_Host___LOW_SPEED_VALUE							0x00
#define USB_LL_Interrupts_Host___FULL_SPEED_VALUE						0x01
#define USB_LL_Interrupts_Host___HIGH_SPEED_VALUE						0x02

typedef enum {
	Channel_Status_Enum___IDLE,
	Channel_Status_Enum___TRANSFER_COMPLETE,
	Channel_Status_Enum___STALL,
	Channel_Status_Enum___HALT,
	Channel_Status_Enum___NAK,
	Channel_Status_Enum___ERROR
} USB_LL_Interrupts_Host___Channel_Status_Enum;

typedef struct {
	uint8_t												status_Change_Flag;
	USB_LL_Interrupts_Host___Channel_Status_Enum 		status;
	uint8_t 											device_Address;
} USB_LL_Interrupts_Host___Channel_Status_TypeDef;

typedef struct {
	uint8_t												is_Root_Device_Connection_Status_Change;
	uint8_t 											is_Root_Device_Connected;
	uint8_t 											is_Root_Device_Disconnected;
	uint8_t 											root_Device_Speed;
	uint8_t												all_Channels_Status_Change_Flag;
	USB_LL_Interrupts_Host___Channel_Status_TypeDef		channel_Status[USB_LL_Host___MAX_NUMBER_OF_CHANNELS];
} USB_LL_Interrupts_Host___Status_TypeDef;

USB_LL_Interrupts_Host___Status_TypeDef* USB_LL_Interrupts_Host___Get_Host_Status(uint8_t port_Number);
void USB_LL_Interrupts_Host___Channel_Interrupt_Handler(uint8_t port_Number);
void USB_LL_Interrupts_Host___Port_Interrupt_Handler(uint8_t port_Number);
void USB_LL_Interrupts_Host___Packet_Received(uint8_t port_Number);
uint8_t USB_LL_Interrupts_Host___Is_Root_Device_Connection_Status_Change(uint8_t port_Number);
uint8_t USB_LL_Interrupts_Host___Is_Root_Device_Connected(uint8_t port_Number);
uint8_t USB_LL_Interrupts_Host___Is_Root_Device_Disconnected(uint8_t port_Number);
uint8_t USB_LL_Interrupts_Host___Get_Root_Device_Speed(uint8_t port_Number);
void USB_LL_Interrupts_Host___Clear_Connection_Status_Change(uint8_t port_Number);
uint8_t USB_LL_Interrupts_Host___Get_All_Channels_Status_Change_Flag(uint8_t port_Number);
uint8_t USB_LL_Interrupts_Host___Get_Channel_Status_Change_Flag(uint8_t port_Number, uint8_t channel_Number);
#endif /* INC_USB_LL_INTERRUPTS_HOST_H_ */
