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
#define USB_Device___ENDPOINT_DERECTION_OUT			USB_LL_Device___ENDPOINT_DERECTION_OUT
#define USB_Device___ENDPOINT_DERECTION_IN			USB_LL_Device___ENDPOINT_DERECTION_IN
#define USB_Device___ENDPOINT_TYPE_CONTROL			USB_LL_Device___ENDPOINT_TYPE_CONTROL
#define USB_Device___ENDPOINT_TYPE_ISOCHRONOUS		USB_LL_Device___ENDPOINT_TYPE_ISOCHRONOUS
#define USB_Device___ENDPOINT_TYPE_BULK				USB_LL_Device___ENDPOINT_TYPE_BULK
#define USB_Device___ENDPOINT_TYPE_INTERRUPT		USB_LL_Device___ENDPOINT_TYPE_INTERRUPT
#define USB_Device___SETUP_PACKET_SIZE				(0x08)
#define USB_Device___bRequest_GET_STATUS			(0x00)
#define USB_Device___bRequest_CLEAR_FEATURE			(0x01)
#define USB_Device___bRequest_SET_FEATURE			(0x03)
#define USB_Device___bRequest_SET_ADDRESS			(0x05)
#define USB_Device___bRequest_GET_DESCRIPTOR		(0x06)
#define USB_Device___bRequest_SET_DESCRIPTOR		(0x07)
#define USB_Device___bRequest_GET_CONFIGURATION		(0x08)
#define USB_Device___bRequest_SET_CONFIGURATION		(0x09)
#define USB_Device___bRequest_GET_INTERFACE			(0x0A)
#define USB_Device___bRequest_SET_INTERFACE			(0x0B)
#define USB_Device___NO_KEY		                    (0xffff)

#define USB_Device___CONTROL_TRANSFER_CALLBACK_PARAMETERS	uint8_t port_Number, uint8_t endpoint_Number, USB_Device___Setup_Packet_TypeDef setup_Packet, uint8_t* data, uint16_t data_Size
#define USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS   uint8_t port_Number, uint8_t endpoint_Number, struct USB_Device___Control_Solution* solution, USB_Device___Setup_Packet_TypeDef setup_Packet, uint8_t* data, uint16_t data_Size


typedef struct __attribute__((packed)) USB_Device___Setup_Packet
{
	uint8_t 								bmRequestType;
	uint8_t 								bRequest;
	uint16_t 								wValue;
	uint16_t 								wIndex;
	uint16_t 								wLength;
} USB_Device___Setup_Packet_TypeDef;

typedef struct USB_Device___Control_Transfer
{
	struct USB_Device___Setup_Packet 		Setup_Packet;
	uint8_t*								data;
	uint16_t								data_Size;
	void (*callback)(USB_Device___CONTROL_TRANSFER_CALLBACK_PARAMETERS);
}USB_Device___Control_Transfer_TypeDef;

struct USB_Device___Endpoint
{
	uint8_t 								Endpoint_Type;
	uint8_t 								Endpoint_Direction;
	struct USB_Device___Control_Transfer 	control_Transfer;
	uint16_t 								Max_Packet_Size;
	uint8_t 								Interval;
};

typedef struct USB_Device___Control_Solution
{
	struct USB_Device___Setup_Packet 		Setup_Packet_Key;
	uint8_t 								is_Key;
	uint8_t* 								buffer;
	uint16_t 								buffer_Size;
	uint8_t (*callback)(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS);
} USB_Device___Control_Solution_TypeDef;

void USB_Device___Init(uint8_t port_Number);
void USB_Device___Set_Control_Transfer_Callback(uint8_t port_Number, uint8_t endpoint_Number, void callback(USB_Device___CONTROL_TRANSFER_CALLBACK_PARAMETERS));
void USB_Device___Set_Conrol_Solutions(uint8_t port_Number, uint8_t endpoint_Number, struct USB_Device___Control_Solution *control_Solution, uint8_t num_Solutions);
uint8_t USB_Device___Set_Address_Solution_Callback(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS);
void USB_Device___Initialize_Endpoint(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction, uint8_t endpoint_Type, uint16_t max_Packet_Size);
void USB_Device___Set_Nak(uint8_t port_Number, uint8_t endpoint_Number, uint8_t direction);
void USB_Device___Transfer_In(uint8_t port_Number, uint8_t endpoint_Number, uint8_t *data, uint16_t length);

#endif /* CORE_INC_USB_DEVICE_H_ */
