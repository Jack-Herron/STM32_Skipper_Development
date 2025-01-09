/*
 * USB_LL_Interrupts_Host.h
 *
 *  Created on: Jun 27, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_INTERRUPTS_HOST_H_
#define INC_USB_LL_INTERRUPTS_HOST_H_

//#define USB_LL_Interrupts_Host___DEBUG_ON

#include "../Inc/USB_LL_Host.h"

#ifdef USB_LL_Interrupts_Host___DEBUG_ON
	#define USB_LL_Interrupts_Host___DPRINTF 1
#else
	#define USB_LL_Interrupts_Host___DPRINTF 0
#endif


#define USB_LL_Interrupts_Host___Debug_Log(...) if (USB_LL_Interrupts_Host___DPRINTF) printf(__VA_ARGS__)

#define USB_LL_Interrupts_Host___LOW_SPEED_VALUE							0x00
#define USB_LL_Interrupts_Host___FULL_SPEED_VALUE							0x01
#define USB_LL_Interrupts_Host___HIGH_SPEED_VALUE							0x02
#define USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_COMPLETE			0x00
#define USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_FAILED_NAK			0x01
#define USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_FAILED_STALL		0x02
#define USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_FAILED_ERROR		0x03
#define USB_LL_Interrupts_Host___CHANNEL_STATUS_CHANNEL_HALTED				0x04

typedef struct {
	uint8_t												status_Change_Flag;
	uint8_t										 		status;
	uint8_t 											is_Busy;
	uint8_t 											device_Address;
} USB_LL_Interrupts_Host___Channel_Status_TypeDef;

typedef struct {
	uint8_t                                             is_Start_Of_Frame;
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
uint8_t USB_LL_Interrupts_Host___Get_Channel_Status(uint8_t port_Number, uint8_t channel_Number);
void USB_LL_Interrupts_Host___Clear_All_Channels_Status_Change_Flag(uint8_t port_Number);
void USB_LL_Interrupts_Host___Clear_Channel_Status_Change_Flag(uint8_t port_Number,  uint8_t channel_Number);
void USB_LL_Interrupts_Host___Start_Of_Frame_Interrupt_Received(uint8_t port_Number);
uint8_t USB_LL_Interrupts_Host___Is_Start_Of_Frame(uint8_t port_Number);
void USB_LL_Interrupts_Host___Clear_Start_Of_Frame(uint8_t port_Number);
uint8_t USB_LL_Interrupts_Host___Channel_Is_Busy(uint8_t port_Number, uint8_t channel_Number);
void USB_LL_Interrupts_Host___Channel_Set_Is_Busy(uint8_t port_Number, uint8_t channel_Number, uint8_t is_Free);

#endif /* INC_USB_LL_INTERRUPTS_HOST_H_ */
