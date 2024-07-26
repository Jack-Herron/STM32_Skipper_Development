/*
 * USB_Host_Enumerate.h
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_ENUMERATE_H_
#define CORE_INC_USB_HOST_ENUMERATE_H_

#include <stdint.h>
#include "../Inc/USB_Host_Device_Manager.h"
#define USB_Host_Enumerate___DEFAULT_MAX_PACKET_SIZE							8
#define USB_Host_Enumerate___STANDARD_NUMBER_OF_RETRIES							0xffff
typedef struct {
	uint8_t is_Allocated;
	uint8_t is_Busy;
	uint8_t port_Number;
	uint8_t pending_USB_Device_Address;
	uint8_t current_USB_Device_Address;
	uint8_t setup_Stage;
	uint8_t current_String_Descriptor_Type;
	uint8_t current_String_Descriptor_Length;
	uint16_t current_String_Descriptor_Language_ID;
	uint8_t current_Configuration_Descriptor_Index;
} USB_Host_Enumerate___Enumerator_TypeDef;

typedef struct USB_Host_Enumerate___Enumerator_Node {
	uint8_t 										is_Allocated;
	USB_Host_Enumerate___Enumerator_TypeDef 		enumerator;
	struct USB_Host_Enumerate___Enumerator_Node* 	next_Node;
	struct USB_Host_Enumerate___Enumerator_Node* 	previous_Node;
} USB_Host_Enumerate___Enumerator_Node_TypeDef;

typedef struct {
	USB_Host_Enumerate___Enumerator_Node_TypeDef* 	first_Node;
	USB_Host_Enumerate___Enumerator_Node_TypeDef* 	last_Node;
}USB_Host_Enumerate___Enumerator_Queue_TypeDef;

uint8_t USB_Host_Enumerate___Enumerate_Device(uint8_t port_Number, uint8_t device_Address);
void USB_Host_Enumerate___Process(uint8_t port_Number);

#endif /* CORE_INC_USB_HOST_ENUMERATE_H_ */
