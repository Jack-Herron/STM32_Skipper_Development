/*
 * USB_Host_Config.h
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#ifndef CONFIG_USB_HOST_CONFIG_H_
#define CONFIG_USB_HOST_CONFIG_H_

#include <stdbool.h>

#define USB_Host_Config___NUMBER_OF_HOST_PORTS_USED						0x01

#define USB_Host_Config___DYNAMICALLY_ALLOCATE_DEVICES 					false		// highly recommended to leave this option false
#define USB_Host_Config___PORT_DEVICE_LIMIT								0x1

#define USB_Host_Config___DYNAMICALLY_ALLOCATE_USB_REQUEST_BLOCKS		false		// highly recommended to leave this option false
#define USB_Host_Config___MAX_USB_REQUEST_BLOCKS						0x10

#define USB_Host_Config___MAX_USB_ENUMERATORS							USB_Host_Config___NUMBER_OF_HOST_PORTS_USED		// how many devices can be in the setup process at one time (only up to one per port is recommended)

#define USB_Host_Config___DEVICE_MANUFACTURER_STRING_MAX_LENGTH			0x40
#define USB_Host_Config___DEVICE_PRODUCT_STRING_MAX_LENGTH				0x40
#define USB_Host_Config___DEVICE_SERIAL_NUMBER_STRING_MAX_LENGTH		0x20
#define USB_Host_Config___DEVICE_LANGUAGE_ID_STRING_MAX_LENGTH			0x10

#define USB_Host_Config___DEVICE_MAX_CONFIGURATION_DESCRIPTOR_LENGTH	0x240
#define USB_Host_Config___DEVICE_MAX_NUMBER_OF_CONFIGURATIONS			0x01
#define USB_Host_Config___DEVICE_MAX_NUMBER_OF_INTERFACE_DESCRIPTORS	0x08
#define USB_Host_Config___DEVICE_MAX_NUMBER_OF_ENDPOINT_DESCRIPTORS		0x08

#endif /* CONFIG_USB_HOST_CONFIG_H_ */
