/*
 * USB_Host_Config.h
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#ifndef CONFIG_USB_HOST_CONFIG_H_
#define CONFIG_USB_HOST_CONFIG_H_

#include <stdbool.h>

#define USB_Host_Config___DYNAMICALLY_ALLOCATE_DEVICES 					true

#define USB_Host_Config___MAX_DEVICES_PER_PORT							0x8

#define USB_Host_Config___PORT_DEVICE_LIMIT								0x08

#define USB_Host_Config___DEVICE_MANUFACTURER_STRING_MAX_LENGTH			0x40
#define USB_Host_Config___DEVICE_PRODUCT_STRING_MAX_LENGTH				0x40
#define USB_Host_Config___DEVICE_SERIAL_NUMBER_STRING_MAX_LENGTH		0x20
#define USB_Host_Config___DEVICE_LANGUAGE_ID_STRING_MAX_LENGTH			0x10

#define USB_Host_Config___DEVICE_MAX_CONFIGURATION_DESCRIPTOR_LENGTH	0x240
#define USB_Host_Config___DEVICE_MAX_NUMBER_OF_CONFIGURATIONS			0x01
#define USB_Host_Config___DEVICE_MAX_NUMBER_OF_INTERFACE_DESCRIPTORS	0x08
#define USB_Host_Config___DEVICE_MAX_NUMBER_OF_ENDPOINT_DESCRIPTORS		0x08

#endif /* CONFIG_USB_HOST_CONFIG_H_ */
