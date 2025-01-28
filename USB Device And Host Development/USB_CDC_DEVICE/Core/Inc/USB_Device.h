/*
 * USB_Device.h
 *
 *  Created on: Jan 27, 2025
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_DEVICE_H_
#define CORE_INC_USB_DEVICE_H_

#define USB_Device___DEFAULT_RX_FIFO_SIZE        (0x80)
#define USB_Device___DEFAULT_TX_FIFO_SIZE        (0x40)

void USB_Device___Init(uint8_t port_Number);

struct USB_Device___Setup_Packet
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

#endif /* CORE_INC_USB_DEVICE_H_ */
