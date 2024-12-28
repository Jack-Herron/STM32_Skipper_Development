/*
 * USB_VICE_Host.h
 *
 *  Created on: Dec 25, 2024
 *      Author: jackh
 */

#ifndef INC_USB_VICE_HOST_H_
#define INC_USB_VICE_HOST_H_

#include <stdint.h>

#define USB_VICE_Host___VICE_INTERFACE_CLASS	0x15
#define USB_VICE_Host___COMPOSITE_DEVICE_CLASS	0x00
void 	USB_VICE_Host___Setup_Device			(uint8_t port_Number, uint8_t device_Address);
uint8_t USB_VICE_Host___Is_Device_VICE_Device	(uint8_t port_Number, uint8_t device_Address);


#endif /* INC_USB_VICE_HOST_H_ */
