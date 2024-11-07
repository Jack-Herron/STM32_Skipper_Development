/*
 * USB_LL_Interrupts_Device.c
 *
 *  Created on: Jun 27, 2024
 *      Author: Jack Herron
 */
#include <stdint.h>
#include <stm32f4xx.h>				// include MCU specific definitions

#include "../Inc/USB_LL_Interrupts_Device.h"
#include "../Inc/USB_LL_Hardware.h"

void USB_LL_Interrupts_Device___Set_RX_Callback(uint8_t port_Number, void (*callback)(USB_LL_Interrupts_Device___RX_CALLBACK_PARAMETERS))
{

}

void USB_LL_Interrupts_Device___Packet_Received(uint8_t port_Number)
{
	USB_OTG_GlobalTypeDef* 		USB = USB_LL_Hardware___Get_USB(port_Number);

	uint32_t 					RX_Status        	= USB -> GRXSTSP;
	uint8_t 					endpoint_Number     = USB_LL_Hardware___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_EPNUM_Msk, USB_OTG_GRXSTSP_EPNUM_Pos);

	USB_OTG_OUTEndpointTypeDef* USB_Out_Endpoint 	= USB_LL_Hardware___Get_USB_Device_OUT(port_Number, endpoint_Number);

	uint8_t						packets_Remaining 	= USB_LL_Hardware___GET_BIT_SEGMENT(USB_Out_Endpoint -> DOEPTSIZ, USB_OTG_HCTSIZ_PKTCNT_Msk, USB_OTG_HCTSIZ_PKTCNT_Pos);
	uint8_t 					packet_Status 		= USB_LL_Hardware___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_PKTSTS_Msk, USB_OTG_GRXSTSP_PKTSTS_Pos);
	uint8_t 					byte_Count			= USB_LL_Hardware___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_BCNT_Msk, USB_OTG_GRXSTSP_BCNT_Pos);

	if(packet_Status == USB_LL_Interrupts_Device___RX_PACKET_STATUS_DATA_PACKET_RECIEVED || packet_Status == USB_LL_Interrupts_Device___RX_PACKET_STATUS_SETUP_PACKET_RECIEVED)
	{
		if(byte_Count > 0)
		{
			/*if((Port[port].Device_Endpoint_Host_Channel_RX_Buffer_Fill_Level[endpoint] + byte_Count) <= Port[port].Device_Endpoint_Host_Channel_RX_Buffer_Size[endpoint])
			{
				USB_RX_POP(port, endpoint, USB_Buffer_Ptr, RX_Status);
				Port[port].Device_Endpoint_Host_Channel_RX_Buffer_Fill_Level[endpoint] += byte_Count;
			}*/
		}
	}
}
