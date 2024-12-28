/*
 * USB_HID_Host.h
 *
 *  Created on: Dec 27, 2024
 *      Author: jackh
 */

#ifndef INC_USB_HID_HOST_H_
#define INC_USB_HID_HOST_H_

#include <stdint.h>

#define USB_HID_Host___HID_INTERFACE_CLASS		0x03
#define USB_HID_Host___COMPOSITE_DEVICE_CLASS	0x00

void 	USB_HID_Host___Setup_Device			(uint8_t port_Number, uint8_t device_Address);
uint8_t USB_HID_Host___Is_Device_HID_Device	(uint8_t port_Number, uint8_t device_Address);


#endif /* INC_USB_HID_HOST_H_ */
