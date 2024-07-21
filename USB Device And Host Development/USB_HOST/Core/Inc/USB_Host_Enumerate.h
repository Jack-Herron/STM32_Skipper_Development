/*
 * USB_Host_Enumerate.h
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_ENUMERATE_H_
#define CORE_INC_USB_HOST_ENUMERATE_H_

#include <stdint.h>

#define USB_Host_Enumerate___DEFAULT_MAX_PACKET_SIZE							8


void USB_Host_Enumerate___Enumerate_Device(uint8_t port_Number, uint8_t device_Address);

#endif /* CORE_INC_USB_HOST_ENUMERATE_H_ */
