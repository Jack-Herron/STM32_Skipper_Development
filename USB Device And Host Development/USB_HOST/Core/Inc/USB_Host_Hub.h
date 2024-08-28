/*
 * USB_Host_Hub.h
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_HUB_H_
#define CORE_INC_USB_HOST_HUB_H_

#include <stdint.h>

#define USB_Host_Hub___HUB_DEVICE_CLASS		0x09

void USB_Host_Hub___Initiate_Hub(uint8_t port_Number, uint8_t device_Address);

typedef struct USB_Host_Hub___Hub_Node
{
	uint8_t is_Allocated;
	struct USB_Host_Hub___Hub_Node* next_Node;
	struct USB_Host_Hub___Hub_Node* previous_Node;
} USB_Host_Hub___Hub_Node_TypeDef;

typedef struct {
	USB_Host_Hub___Hub_Node_TypeDef* 	first_Node;
	USB_Host_Hub___Hub_Node_TypeDef* 	last_Node;
}USB_Host_Hub___Hub_Queue_TypeDef;

#endif /* CORE_INC_USB_HOST_HUB_H_ */
