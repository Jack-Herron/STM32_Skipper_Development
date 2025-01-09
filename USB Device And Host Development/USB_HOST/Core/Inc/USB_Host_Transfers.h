/*
 * USB_Host_URB.h
 *
 *  Created on: Jul 8, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_TRANSFERS_H_
#define CORE_INC_USB_HOST_TRANSFERS_H_

#include <stdint.h>
#include "../Inc/USB_Host_Device_Manager.h"
#include <USB_LL_Interrupts_Host.h>
struct USB_Host_Transfers___URB;

#define USB_Host_Transfers___DEBUG_ON

#ifdef USB_Host_Transfers___DEBUG_ON
#define USB_Host_Transfers___DPRINTF 1
#else
#define USB_Host_Transfers___DPRINTF 0
#endif
#define USB_Host_Transfers___Debug_Log(...) if (USB_Host_Transfers___DPRINTF) printf(__VA_ARGS__)

#define USB_Host_Transfers___CONTROL_SETUP_PACKET_LENGTH									 	8
#define USB_Host_Transfers___URB_STAGE_SETUP													0
#define USB_Host_Transfers___URB_STAGE_DATA														1
#define USB_Host_Transfers___URB_STAGE_STATUS													2
#define USB_Host_Transfers___URB_STAGE_COMPLETE													3
#define USB_Host_Transfers___URB_TYPE_CONTROL													0
#define USB_Host_Transfers___URB_TYPE_ISOCHRONOUS												1
#define USB_Host_Transfers___URB_TYPE_BULK														2
#define USB_Host_Transfers___URB_TYPE_INTERRUPT													3
#define USB_Host_Transfers___URB_DIRECTION_IN													USB_Host___TRANSFER_DIRECTION_IN
#define USB_Host_Transfers___URB_DIRECTION_OUT													USB_Host___TRANSFER_DIRECTION_OUT
#define USB_Host_Transfers___URB_FIFO_QUEUE_LENGTH												USB_Host_Config___MAX_USB_REQUEST_BLOCKS
#define USB_Host_Transfers___URB_CALLBACK_PARAMETERS											struct USB_Host_Transfers___URB URB

#define USB_Host_Transfers___URB_STATUS_SUCCESS													USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_COMPLETE
#define USB_Host_Transfers___URB_STATUS_NAK														USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_FAILED_NAK
#define USB_Host_Transfers___URB_STATUS_ERROR													USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_FAILED_ERROR
#define USB_Host_Transfers___URB_STATUS_STALL													USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_FAILED_STALL

#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_DEVICE_TO_HOST			0X80
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_INTERFACE_TO_HOST		0X81
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_ENDPOINT_TO_HOST		0X82
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_OTHER_TO_HOST			0X83
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_HOST_TO_DEVICE			0X00
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_HOST_TO_INTERFACE		0X01
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_HOST_TO_ENDPOINT		0X02
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_HOST_TO_OTHER			0X03

#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_VENDOR_DEVICE_TO_HOST			0XC0
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_VENDOR_INTERFACE_TO_HOST		0XC1
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_VENDOR_ENDPOINT_TO_HOST			0XC2
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_VENDOR_OTHER_TO_HOST			0XC3
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_VENDOR_HOST_TO_DEVICE			0X40
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_VENDOR_HOST_TO_INTERFACE		0X41
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_VENDOR_HOST_TO_ENDPOINT			0X42
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_VENDOR_HOST_TO_OTHER			0X43

#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_DEVICE_TO_HOST			0XA0
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_INTERFACE_TO_HOST			0XA1
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_ENDPOINT_TO_HOST			0XA2
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_OTHER_TO_HOST				0XA3
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_HOST_TO_DEVICE			0X20
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_HOST_TO_INTERFACE			0X21
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_HOST_TO_ENDPOINT			0X22
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_HOST_TO_OTHER				0X23

#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_STATUS							0X00
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_CLEAR_FEATURE						0X01
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_SET_FEATURE							0X03
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_SET_ADDRESS							0X05
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_DESCRIPTOR						0X06
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_SET_DESCRIPTOR						0X07
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_CONFIGURATION					0X08
#define USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_SET_CONFIGURATION					0X09



typedef struct  __attribute__((packed))
{
	uint8_t 	bmRequestType;
	uint8_t 	bRequest;
	uint16_t 	wValue;
	uint16_t 	wIndex;
	uint16_t 	wLength;
}USB_Host_Transfers___Control_Setup_Packet_TypeDef;

typedef struct USB_Host_Transfers___URB
{
	uint8_t												port_Number;
	uint8_t												device_Address;
	uint8_t 											endpoint_Number;
	uint8_t 											transfer_Direction;
	uint8_t												transfer_Stage;
	uint8_t 											busy;
	uint8_t												transfer_Type;
	uint32_t											transfer_Length;
	uint16_t                                       		num_Retries;
	uint8_t												is_Odd_Frame;
	USB_Host_Transfers___Control_Setup_Packet_TypeDef	control_Setup_Packet;
	uint8_t 											multi_Count;
	uint8_t*											transfer_Buffer;
	uint8_t												transfer_Status;
	void(*URB_Callback)									(USB_Host_Transfers___URB_CALLBACK_PARAMETERS);
}USB_Host_Transfers___URB_TypeDef;

typedef struct USB_Host_Transfers___URB_Node
{
	uint8_t											is_Allocated;
	USB_Host_Transfers___URB_TypeDef				URB;
	struct USB_Host_Transfers___URB_Node*			next_Node;
	struct USB_Host_Transfers___URB_Node*			previous_Node;
}USB_Host_Transfers___URB_Node_TypeDef;

typedef struct
{
	USB_Host_Transfers___URB_Node_TypeDef* 			first_Node;
	USB_Host_Transfers___URB_Node_TypeDef*			last_Node;
	uint16_t										num_URBs;
}USB_Host_Transfers___URB_Queue_TypeDef;

int8_t USB_Host_Transfers___Isochronous_Transfer(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t transfer_Direction, uint8_t* transfer_Buffer, uint32_t transfer_Length, uint16_t number_Of_Retries, uint8_t odd_Frame, uint8_t multi_Count, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));
int8_t USB_Host_Transfers___Interrupt_Transfer(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t transfer_Direction, uint8_t* transfer_Buffer, uint32_t transfer_Length, uint16_t number_Of_Retries, uint8_t odd_Frame, uint8_t multi_Count, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));
int8_t USB_Host_Transfers___Bulk_Transfer(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t transfer_Direction, uint8_t* transfer_Buffer, uint32_t transfer_Length, uint16_t number_Of_Retries, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));
int8_t USB_Host_Transfers___Control_Transfer(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t transfer_Direction, USB_Host_Transfers___Control_Setup_Packet_TypeDef setup_Packet, uint8_t* transfer_Buffer, uint32_t transfer_Length, uint16_t number_Of_Retries, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));

void USB_Host_Transfers___Process_URB(uint8_t port_Number);

#endif /* CORE_INC_USB_HOST_TRANSFERS_H_ */
