/*
 * USB_HID_Host.h
 *
 *  Created on: Dec 27, 2024
 *      Author: jackh
 */

#ifndef INC_USB_HID_HOST_H_
#define INC_USB_HID_HOST_H_

#include <stdint.h>
#include <stdbool.h>

#define USB_HID_Host___DYNAMICALLY_ALLOCATE_HID_DEVICES		false
#define USB_HID_Host___NUMBER_OF_HID_INSTANCES				0x10
#define USB_HID_Host___HID_INTERFACE_CLASS					0x03
#define USB_HID_Host___COMPOSITE_DEVICE_CLASS				0x00

typedef struct
{
	uint8_t port_Number;
	uint8_t device_Address;
	uint8_t configuration_Number;
	uint8_t interface_Number;
	uint8_t interrupt_In_Endpoint_Number;
	uint8_t interrupt_Out_Endpoint_Number;
} USB_HID_Host___HID_Device_TypeDef;

typedef struct USB_HID_Host___HID_Node
{
	uint8_t is_Allocated;
	USB_HID_Host___HID_Device_TypeDef   HID_Device;
	struct USB_HID_Host___HID_Node* 	next_Node;
	struct USB_HID_Host___HID_Node* 	previous_Node;
} USB_HID_Host___HID_Node_TypeDef;

typedef struct {
	USB_HID_Host___HID_Node_TypeDef* 	first_Node;
	USB_HID_Host___HID_Node_TypeDef* 	last_Node;
}USB_HID_Host___HID_List_TypeDef;

void 	USB_HID_Host___Setup_Device			(uint8_t port_Number, uint8_t device_Address);
uint8_t USB_HID_Host___Is_Device_HID_Device	(uint8_t port_Number, uint8_t device_Address);
void 	USB_HID_Host___Setup_HID_Device		(uint8_t port_Number, uint8_t device_Address);

#endif /* INC_USB_HID_HOST_H_ */
