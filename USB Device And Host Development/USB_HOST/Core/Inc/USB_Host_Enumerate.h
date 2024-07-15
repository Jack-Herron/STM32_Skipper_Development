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
#define USB_Host_Enumerate___SETUP_STAGE_IDLE									0
#define USB_Host_Enumerate___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR 		1
#define USB_Host_Enumerate___SETUP_STAGE_SET_ADDRESS 							2
#define USB_Host_Enumerate___SETUP_STAGE_GET_FULL_DEVICE_DESCRIPTOR				3
#define USB_Host_Enumerate___SETUP_STAGE_GET_SHORT_STRING_DESCRIPTOR 			4
#define USB_Host_Enumerate___SETUP_STAGE_GET_FULL_STRING_DESCRIPTOR 			5
#define USB_Host_Enumerate___SETUP_STAGE_GET_SHORT_CONFIGURATION_DESCRIPTOR 	6
#define USB_Host_Enumerate___SETUP_STAGE_GET_FULL_CONFIGURATION_DESCRIPTOR		7
#define USB_Host_Enumerate___SETUP_STAGE_COMPLETE	 							8

void USB_Host_Enumerate___Enumerate_Device(uint8_t port_Number, uint8_t device_Address);

#endif /* CORE_INC_USB_HOST_ENUMERATE_H_ */
