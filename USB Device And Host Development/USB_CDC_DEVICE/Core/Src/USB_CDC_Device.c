/*
 * USB_CDC_Device.c
 *
 *  Created on: Nov 5, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>
#include <stdio.h>

#include "../Inc/USB_CDC_Device.h"
#include <USB_Device.h>
#include <USB_Device_Descriptors.h>

void USB_CDC_Device___RX_Callback(USB_LL_Device___RX_CALLBACK_PARAMETERS);

static uint16_t language_String_Descriptor[] 				= 	{0x0409};
static uint16_t manufacturer_String_Descriptor[] 			= 	{'V', 'I', 'K', 'I' , 'N', 'G', ' ', 'L', 'A', 'B', 'S'};
static uint16_t product_String_Descriptor[] 				= 	{'S', 'K' ,'I', 'P', 'P' , 'E', 'R'};
static uint16_t serial_Number_String_Descriptor[] 			= 	{'0', '1', '1', '0'};

static uint8_t USB_CDC_Device___Device_Descriptor[] =
{
	0x12,							// bLength
	0x01,							// bDescriptorType
	0x00, 0x02,						// bcdUSB
	0x02,							// bDeviceClass
	0x02,							// bDeviceSubClass
	0x00,							// bDeviceProtocol
	0x40,							// bMaxPacketSize0
	0x83, 0x04,						// idVendor
	0x10, 0x00,						// idProduct
	0x00, 0x01,						// bcdDevice
	0x01,							// iManufacturer
	0x02,							// iProduct
	0x03,							// iSerialNumber
	0x01							// bNumConfigurations
};

static uint8_t USB_CDC_Device___Configuration_Descriptor[] =
{
	0x09,							// configurations.bLength
	0x02,							// configurations.bDescriptorType
	0x43, 0x00,						// configurations.wTotalLength
	0x02,							// configurations.bNumInterfaces
	0x01,							// configurations.bConfigurationValue
	0x00,							// configurations.iConfiguration
	0xc0,							// configurations.bmAttributes
	0x32,							// configurations.bMaxPower

	0x09,							// Interface.bLength
	0x04,							// Interface.bDescriptorType
	0x00,							// Interface.bInterfaceNumber
	0x00,							// Interface.bAlternateSetting
	0x01,							// Interface.bNumEndpoints
	0x02,							// Interface.bInterfaceClass
	0x02,							// Interface.bInterfaceSubClass
	0x01,							// Interface.bInterfaceProtocol
	0x00,							// Interface.iInterface

	0x05,							// CS.bLength
	0x24,							// CS.bDescriptorType
	0x00,							// CS.bDescriptorSubtype
	0x10, 0x01,						// CS.CDC

	0x05,							// CS.bLength
	0x24,							// CS.bDescriptorType
	0x01,							// CS.bDescriptorSubtype
	0x00,							// CS.bmCapabilities
	0x01,							// CS.data interface

	0x04,							// CS.bLength
	0x24,							// CS.bDescriptorType
	0x02,							// CS.bDescriptorSubtype
	0x02,							// CS.bmCapabilities

	0x05,							// CS.bLength
	0x24,							// CS.bDescriptorType
	0x06,							// CS.bDescriptorSubtype
	0x00,							// CS.Control Interface
	0x01,							// CS.subordinate interface

	0x07,							// Endpoint.bLength
	0x05,							// Endpoint.bDescriptorType
	0x82,							// Endpoint.bEndpointAddress
	0x03,							// Endpoint.bmAttributes
	0x08, 0x00,						// Endpoint.wMaxPacketSize
	0x10,							// Endpoint.bInterval

	0x09,							// Interface.bLength
	0x04,							// Interface.bDescriptorType
	0x01,							// Interface.bInterfaceNumber
	0x00,							// Interface.bAlternateSetting
	0x02,							// Interface.bNumEndpoints
	0x0a,							// Interface.bInterfaceClass
	0x00,							// Interface.bInterfaceSubClass
	0x00,							// Interface.bInterfaceProtocol
	0x00,							// Interface.iInterface

	0x07,							// Endpoint.bLength
	0x05,							// Endpoint.bDescriptorType
	0x01,							// Endpoint.bEndpointAddress
	0x02,							// Endpoint.bmAttributes
	0x40, 0x00,						// Endpoint.wMaxPacketSize
	0x00,							// Endpoint.bInterval

	0x07,							// Endpoint.bLength
	0x05,							// Endpoint.bDescriptorType
	0x81,							// Endpoint.bEndpointAddress
	0x02,							// Endpoint.bmAttributes
	0x40, 0x00,						// Endpoint.wMaxPacketSize
	0x00							// Endpoint.bInterval
};



void USB_CDC_Device___Init(uint8_t port_Number)
{
	USB_Device_Descriptors___Add_Entry(port_Number, USB_Device_Descriptors___DESCRIPTOR_TYPE_DEVICE, 		0, USB_CDC_Device___Device_Descriptor, 			sizeof(USB_CDC_Device___Device_Descriptor));
	USB_Device_Descriptors___Add_Entry(port_Number, USB_Device_Descriptors___DESCRIPTOR_TYPE_CONFIGURATION, 0, USB_CDC_Device___Configuration_Descriptor, 	sizeof(USB_CDC_Device___Configuration_Descriptor));
	USB_Device_Descriptors___Add_Entry(port_Number, USB_Device_Descriptors___DESCRIPTOR_TYPE_STRING, 		0, (uint8_t*)language_String_Descriptor, 		sizeof(language_String_Descriptor));
	USB_Device_Descriptors___Add_Entry(port_Number, USB_Device_Descriptors___DESCRIPTOR_TYPE_STRING, 		1, (uint8_t*)manufacturer_String_Descriptor, 	sizeof(manufacturer_String_Descriptor));
	USB_Device_Descriptors___Add_Entry(port_Number, USB_Device_Descriptors___DESCRIPTOR_TYPE_STRING, 		2, (uint8_t*)product_String_Descriptor, 		sizeof(product_String_Descriptor));
	USB_Device_Descriptors___Add_Entry(port_Number, USB_Device_Descriptors___DESCRIPTOR_TYPE_STRING, 		3, (uint8_t*)serial_Number_String_Descriptor, 	sizeof(serial_Number_String_Descriptor));

	USB_Device___Init(port_Number);
}
