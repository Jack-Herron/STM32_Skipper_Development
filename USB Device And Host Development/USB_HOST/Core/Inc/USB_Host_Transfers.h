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

struct USB_Host_Transfers___URB;

#define USB_Host_Transfers___CONTROL_SETUP_PACKET_LENGTH 	8
#define USB_Host_Transfers___URB_STAGE_SETUP				0
#define USB_Host_Transfers___URB_STAGE_DATA					1
#define USB_Host_Transfers___URB_STAGE_STATUS				2
#define USB_Host_Transfers___URB_STAGE_COMPLETE				3
#define USB_Host_Transfers___URB_TYPE_CONTROL				0
#define USB_Host_Transfers___URB_TYPE_BULK					1
#define USB_Host_Transfers___URB_TYPE_INTERRUPT				2
#define USB_Host_Transfers___URB_TYPE_ISOCHRONOUS			3
#define USB_Host_Transfers___URB_DIRECTION_IN				0
#define USB_Host_Transfers___URB_DIRECTION_OUT				1
#define USB_Host_Transfers___URB_FIFO_QUEUE_LENGTH			USB_Host_Config___MAX_USB_REQUEST_BLOCKS
#define USB_Host_Transfers___URB_CALLBACK_PARAMETERS		uint8_t port_Number, struct USB_Host_Transfers___URB URB

typedef struct USB_Host_Transfers___URB
{
	uint8_t											device_Address;
	uint8_t 										transfer_Direction;
	uint8_t											transfer_Stage;
	uint8_t 										busy;
	uint8_t											transfer_Type;
	uint32_t										transfer_Length;
	uint8_t											control_Setup_Packet[USB_Host_Transfers___CONTROL_SETUP_PACKET_LENGTH];
	uint8_t*										transfer_Buffer;
	void(*URB_Callback)								(USB_Host_Transfers___URB_CALLBACK_PARAMETERS);
}USB_Host_Transfers___URB_TypeDef;

typedef struct USB_Host_Transfers___URB_Node
{
	uint8_t											is_Allocated;
	USB_Host_Transfers___URB_TypeDef				URB;
	struct USB_Host_Transfers___URB_Node*			next_Node;
}USB_Host_Transfers___URB_Node_TypeDef;

typedef struct
{
	USB_Host_Transfers___URB_Node_TypeDef* 			current_Node;
	USB_Host_Transfers___URB_Node_TypeDef*			last_Node;
}USB_Host_Transfers___URB_Queue_TypeDef;

int8_t USB_Host_Transfers___Isochronous_Transfer_Out(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));
int8_t USB_Host_Transfers___Isochronous_Transfer_In(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));
int8_t USB_Host_Transfers___Interrupt_Transfer_Out(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));
int8_t USB_Host_Transfers___Interrupt_Transfer_In(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));
int8_t USB_Host_Transfers___Bulk_Transfer_Out(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));
int8_t USB_Host_Transfers___Bulk_Transfer_In(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));
int8_t USB_Host_Transfers___Control_Transfer_Out(uint8_t device_Address, uint8_t* setup_Packet, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));
int8_t USB_Host_Transfers___Control_Transfer_In(uint8_t device_Address, uint8_t* setup_Packet, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS));

#endif /* CORE_INC_USB_HOST_TRANSFERS_H_ */
