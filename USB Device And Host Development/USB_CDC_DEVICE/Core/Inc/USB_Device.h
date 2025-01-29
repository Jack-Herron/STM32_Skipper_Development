/*
 * USB_Device.h
 *
 *  Created on: Jan 27, 2025
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_DEVICE_H_
#define CORE_INC_USB_DEVICE_H_

#include	<stdint.h>
#include 	<USB_LL_Device.h>

#define USB_Device___GET_MIN(a, b)					(((a) < (b)) ? (a) : (b)
#define USB_Device___NUM_PORTS                 	 	(USB_LL_Device___PORT_COUNT)
#define USB_Device___NUM_ENDPOINTS       			(USB_LL_Device___ENDPOINT_COUNT)
#define USB_Device___DEFAULT_RX_FIFO_SIZE        	(0x80)
#define USB_Device___DEFAULT_TX_FIFO_SIZE        	(0x40)
#define USB_Device___SETUP_PACKET_SIZE				(0x08)
#define USB_Device___bRequest_GET_STATUS			(0x00)
#define USB_Device___bRequest_CLEAR_FEATURE			(0x01)
#define USB_Device___bRequest_SET_FEATURE			(0x03)
#define USB_Device___bRequest_SET_ADDRESS			(0x05)
#define USB_Device___bRequest_GET_DESCRIPTOR		(0x06)
#define USB_Device___bRequest_SET_DESCRIPTOR		(0x07)
#define USB_Device___bRequest_GET_CONFIGURATION		(0x08)
#define USB_Device___bRequest_SET_CONFIGURATION		(0x09)

struct __attribute__((packed)) USB_Device___Setup_Packet
{
	uint8_t 								bmRequestType;
	uint8_t 								bRequest;
	uint16_t 								wValue;
	uint16_t 								wIndex;
	uint16_t 								wLength;
};

struct USB_Device___Control_Transfer
{
	struct USB_Device___Setup_Packet 		Setup_Packet;
	uint8_t*								data;
};

struct USB_Device___Endpoint
{
	uint8_t 								Endpoint_Type;
	uint8_t 								Endpoint_Direction;
	struct USB_Device___Control_Transfer 	control_Transfer;
	uint16_t 								Max_Packet_Size;
	uint8_t 								Interval;
};

void USB_Device___Init(uint8_t port_Number);

#endif /* CORE_INC_USB_DEVICE_H_ */
