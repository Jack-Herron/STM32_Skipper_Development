/*
 * USB_LL_Device.h
 *
 *  Created on: Jun 26, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_DEVICE_H_
#define INC_USB_LL_DEVICE_H_

#include <stdint.h>

#define USB_LL_Device___PORT_ZERO 					0
#define USB_LL_Device___PORT_ONE 					0
#define USB_LL_Device___PORT_ZERO_NUM_ENDPOINTS 	4
#define USB_LL_Device___PORT_ONE_NUM_ENDPOINTS 		6

typedef struct
{
	uint16_t RX_FIFO_Depth;
	uint16_t Endpoint_TX_FIFO_Depth[6];
} USB_LL_Device___FIFO_Config_TypeDef;

void USB_LL_Device___Set_FIFO_Size(uint8_t port_Number, USB_LL_Device___FIFO_Config_TypeDef FIFO_Config);
void USB_LL_Device___Receive_Setup_Packet(uint8_t port_Number, uint8_t endpoint_Number, uint32_t packet_Size);
void USB_LL_Device___Receive_Data_Packet(uint8_t port_Number, uint8_t endpoint_Number, uint32_t packet_Size);

#define USB_LL_Device___RX_CALLBACK_PARAMETERS  uint8_t port_Number, uint8_t *data, uint16_t length
#define USB_LL_Device___RX_PACKET_STATUS_DATA_PACKET_RECIEVED	0x02
#define USB_LL_Device___RX_PACKET_STATUS_SETUP_PACKET_RECIEVED	0x06

void USB_LL_Device___Set_RX_Callback(uint8_t port_Number, void (*callback)(USB_LL_Device___RX_CALLBACK_PARAMETERS));
void USB_LL_Device___Packet_Received(uint8_t port_Number);
void USB_LL_Device___Host_Enumerated(uint8_t port_Number);
void USB_LL_Device___USB_Suspend(uint8_t port_Number);

#endif /* INC_USB_LL_DEVICE_H_ */
