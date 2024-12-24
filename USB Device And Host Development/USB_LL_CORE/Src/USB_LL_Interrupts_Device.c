/*
 * USB_LL_Interrupts_Device.c
 *
 *  Created on: Jun 27, 2024
 *      Author: Jack Herron
 */
#include <stdint.h>
#include <stm32f4xx.h>				// include MCU specific definitions
#include <stdio.h>
#include "../Inc/USB_LL_Interrupts_Device.h"
#include "../Inc/USB_LL_Hardware.h"
#include "../Inc/USB_LL_Device.h"

void USB_LL_Interrupts_Device___Set_RX_Callback(uint8_t port_Number, void (*callback)(USB_LL_Interrupts_Device___RX_CALLBACK_PARAMETERS))
{

}

void USB_LL_Interrupts_Device___Packet_Received(uint8_t port_Number)
{
	USB_OTG_GlobalTypeDef* 		USB = USB_LL_Hardware___Get_USB(port_Number);

	uint32_t 					RX_Status        	= USB -> GRXSTSP;
	uint8_t 					endpoint_Number     = USB_LL_Hardware___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_EPNUM_Msk, USB_OTG_GRXSTSP_EPNUM_Pos);
	uint8_t 					packet_Status 		= USB_LL_Hardware___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_PKTSTS_Msk, USB_OTG_GRXSTSP_PKTSTS_Pos);
	uint8_t 					byte_Count			= USB_LL_Hardware___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_BCNT_Msk, USB_OTG_GRXSTSP_BCNT_Pos);

	if(packet_Status == USB_LL_Interrupts_Device___RX_PACKET_STATUS_SETUP_PACKET_RECIEVED)
	{
		if(byte_Count > 0)
		{
			USB_LL_Device___Receive_Setup_Packet(port_Number, endpoint_Number, byte_Count);
		}
	}
	else if(packet_Status == USB_LL_Interrupts_Device___RX_PACKET_STATUS_DATA_PACKET_RECIEVED)
	{
		if(byte_Count > 0)
		{
			USB_LL_Device___Receive_Data_Packet(port_Number, endpoint_Number, byte_Count);
		}
	}
}

void USB_LL_Interrupts_Device___USB_Suspend(uint8_t port_Number)
{
	printf("USB Suspended\n");
}

void USB_LL_Interrupts_Device___Host_Enumerated(uint8_t port_Number)
{
	printf("Host attempting to connect\n");
}
