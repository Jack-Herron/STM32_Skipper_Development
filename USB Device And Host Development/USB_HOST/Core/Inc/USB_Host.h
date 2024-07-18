/*
 * USB_Host.h
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_H_
#define CORE_INC_USB_HOST_H_

#include <USB_LL_Definitions.h>
#include <USB_LL_Host.h>

#define USB_Host___DEVICE_DESCRIPTOR_LENGTH			0x12
#define USB_Host___CONFIGURATION_DESCRIPTOR_LENGTH	0x09
#define USB_Host___INTERFACE_DESCRIPTOR_LENGTH		0x09
#define USB_Host___ENDPOINT_DESCRIPTOR_LENGTH		0x07
#define USB_Host___NUMBER_OF_PORTS					USB_LL_Definitions___NUMBER_OF_PORTS
#define USB_Host___TRANSFER_DIRECTION_OUT			USB_LL_Host___TRANSFER_DIRECTION_OUT
#define USB_Host___TRANSFER_DIRECTION_IN			USB_LL_Host___TRANSFER_DIRECTION_IN

typedef struct __attribute__((packed)) {
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  bcdUSB;        			/* USB Specification Number which device complies too */
  uint8_t   bDeviceClass;
  uint8_t   bDeviceSubClass;
  uint8_t   bDeviceProtocol;
  uint8_t   bMaxPacketSize;
  uint16_t  idVendor;      			/* Vendor ID (Assigned by USB Org) */
  uint16_t  idProduct;     			/* Product ID (Assigned by Manufacturer) */
  uint16_t  bcdDevice;     			/* Device Release Number */
  uint8_t   iManufacturer; 			/* Index of Manufacturer String Descriptor */
  uint8_t   iProduct;       		/* Index of Product String Descriptor */
  uint8_t   iSerialNumber;  		/* Index of Serial Number String Descriptor */
  uint8_t   bNumConfigurations; 	/* Number of Possible Configurations */
} USB_Host___Device_Descriptor_TypeDef;

typedef struct __attribute__((packed)) {
  uint8_t	bLength;
  uint8_t	bDescriptorType;
  uint16_t	wTotalLength;        	/* Total Length of Data Returned */
  uint8_t	bNumInterfaces;       	/* Number of Interfaces */
  uint8_t	bConfigurationValue;  	/* Value to use as an argument to select this configuration*/
  uint8_t	iConfiguration;      	/*Index of String Descriptor Describing this configuration */
  uint8_t	bmAttributes;         	/* D7 Bus Powered , D6 Self Powered, D5 Remote Wakeup , D4..0 Reserved (0)*/
  uint8_t	bMaxPower;           	/*Maximum Power Consumption */
} USB_Host___Configuration_Descriptor_TypeDef;

typedef struct __attribute__((packed)) {
  uint8_t	bLength;
  uint8_t	bDescriptorType;
  uint8_t	bInterfaceNumber;
  uint8_t	bAlternateSetting;    	/* Value used to select alternative setting */
  uint8_t	bNumEndpoints;        	/* Number of Endpoints used for this interface */
  uint8_t	bInterfaceClass;      	/* Class Code (Assigned by USB Org) */
  uint8_t	bInterfaceSubClass;   	/* Subclass Code (Assigned by USB Org) */
  uint8_t	bInterfaceProtocol;   	/* Protocol Code */
  uint8_t	iInterface;           	/* Index of String Descriptor Describing this interface */
} USB_Host___Interface_Descriptor_TypeDef;

typedef struct __attribute__((packed)) {
  uint8_t	bLength;
  uint8_t	bDescriptorType;
  uint8_t	bEndpointAddress;   	/* indicates what endpoint this descriptor is describing */
  uint8_t	bmAttributes;       	/* specifies the transfer type. */
  uint16_t	wMaxPacketSize;    		/* Maximum Packet Size this endpoint is capable of sending or receiving */
  uint8_t	bInterval;          	/* is used to specify the polling interval of certain transfers. */
} USB_Host___Endpoint_Descriptor_TypeDef;

void USB_Host___Init(uint8_t port_Number);
void USB_Host___Process(uint8_t port_Number);
#endif /* CORE_INC_USB_HOST_H_ */
