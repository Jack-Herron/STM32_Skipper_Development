/*
 * USB_LL_Interrupts_Host.c
 *
 *  Created on: Jun 27, 2024
 *      Author: Jack Herron
 */

#include <stdint.h>					// Include C library for fixed-width integer types
#include <stm32f4xx.h>				// include MCU specific definitions
#include <Skipper_Clock.h>
#include <stdbool.h>

#include "../Inc/USB_LL_Definitions.h"
#include "../Inc/USB_LL_Hardware.h"
#include "../Inc/USB_LL_Host.h"
#include "../Inc/USB_LL_Interrupts.h"
#include "../Inc/USB_LL_Interrupts_Host.h"

static USB_LL_Interrupts_Host___Status_TypeDef host_Status[USB_LL_Definitions___NUMBER_OF_PORTS];

uint8_t USB_LL_Interrupts_Host___Get_Root_Device_Speed(uint8_t port_Number)
{
	uint8_t return_Value = host_Status[port_Number].root_Device_Speed;
	return(return_Value);
}

void USB_LL_Interrupts_Host___Clear_Connection_Status_Change(uint8_t port_Number)
{
	host_Status[port_Number].is_Root_Device_Connection_Status_Change = false;
}

uint8_t USB_LL_Interrupts_Host___Is_Root_Device_Connection_Status_Change(uint8_t port_Number)
{
	uint8_t return_Value = host_Status[port_Number].is_Root_Device_Connection_Status_Change;
	return(return_Value);
}

uint8_t USB_LL_Interrupts_Host___Is_Root_Device_Connected(uint8_t port_Number)
{
	return(host_Status[port_Number].is_Root_Device_Connected);
}

uint8_t USB_LL_Interrupts_Host___Is_Root_Device_Disconnected(uint8_t port_Number)
{
	return(host_Status[port_Number].is_Root_Device_Disconnected);
}

uint8_t USB_LL_Interrupts_Host___Get_All_Channels_Status_Change_Flag(uint8_t port_Number)
{
	return (host_Status[port_Number].all_Channels_Status_Change_Flag);
}

void USB_LL_Interrupts_Host___Clear_All_Channels_Status_Change_Flag(uint8_t port_Number)
{
	host_Status[port_Number].all_Channels_Status_Change_Flag = false;
}

uint8_t USB_LL_Interrupts_Host___Get_Channel_Status(uint8_t port_Number, uint8_t channel_Number)
{
	return(host_Status[port_Number].channel_Status[channel_Number].status);
}

uint8_t USB_LL_Interrupts_Host___Get_Channel_Status_Change_Flag(uint8_t port_Number, uint8_t channel_Number)
{
	return (host_Status[port_Number].channel_Status[channel_Number].status_Change_Flag);
}

void USB_LL_Interrupts_Host___Clear_Channel_Status_Change_Flag(uint8_t port_Number,  uint8_t channel_Number)
{
	host_Status[port_Number].channel_Status[channel_Number].status_Change_Flag = false;
}

void USB_LL_Interrupts_Host___Set_Channel_Status_Change_Flag(uint8_t port_Number, uint8_t channel_Number)
{
	host_Status[port_Number].channel_Status[channel_Number].status_Change_Flag = true;
	host_Status[port_Number].all_Channels_Status_Change_Flag = true;
}

void USB_LL_Interrupts_Host___Packet_Received(uint8_t port_Number)
{
	USB_OTG_GlobalTypeDef* 		USB 				= USB_LL_Hardware___Get_USB(port_Number);

	uint32_t 					RX_Status        	= USB -> GRXSTSP;
	uint8_t 					channel_Number     	= USB_LL_Hardware___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_EPNUM_Msk, USB_OTG_GRXSTSP_EPNUM_Pos);

	USB_OTG_HostChannelTypeDef* USB_Host_Channel 	= USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);

	uint8_t						packets_Remaining 	= USB_LL_Hardware___GET_BIT_SEGMENT(USB_Host_Channel -> HCTSIZ, USB_OTG_HCTSIZ_PKTCNT_Msk, USB_OTG_HCTSIZ_PKTCNT_Pos);
	uint8_t 					packet_Status 		= USB_LL_Hardware___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_PKTSTS_Msk, USB_OTG_GRXSTSP_PKTSTS_Pos);
	uint8_t 					byte_Count			= USB_LL_Hardware___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_BCNT_Msk, USB_OTG_GRXSTSP_BCNT_Pos);

	if(packet_Status == USB_LL_Host___RX_PACKET_STATUS_DATA_PACKET_RECIEVED)
	{
		if(byte_Count > 0)
		{

			(void)USB_LL_Host___Channel_RX_POP(port_Number, channel_Number, RX_Status);

			if(packets_Remaining > 0)
			{
				USB_Host_Channel -> HCCHAR |= (USB_OTG_HCCHAR_CHENA);
			}
		}
	}
}


void USB_LL_Interrupts_Host___Device_Connect_Detected(uint8_t port_Number)
{
	USB_OTG_HostPortTypeDef*	USB_Host_Port 	= USB_LL_Hardware___Get_USB_Host_Port(port_Number);
	USB_OTG_HostTypeDef* 		USB_Host 		= USB_LL_Hardware___Get_USB_Host(port_Number);
	if(!(USB_Host -> HCFG & USB_OTG_HCFG_FSLSPCS))
	{
		USB_Host -> HFIR  = (USB_LL_HOST___CLOCK_CYCLES_IN_ONE_MILLISECOND_FRAME);
		USB_Host -> HCFG |= (USB_OTG_HCFG_FSLSPCS_0);
		USB_LL_Host___Reset_Port(port_Number);
	}
	else
	{
		uint8_t host_Speed = USB_LL_Hardware___GET_BIT_SEGMENT(USB_Host_Port -> HPRT, USB_OTG_HPRT_PSPD_Msk, USB_OTG_HPRT_PSPD_Pos);
		uint8_t speed;

		if(host_Speed == USB_LL_Host___HPRT_LOW_SPEED_VALUE)
		{
			speed = USB_LL_Interrupts_Host___LOW_SPEED_VALUE;
		}
		else if(host_Speed == USB_LL_Host___HPRT_FULL_SPEED_VALUE)
		{
			speed = USB_LL_Interrupts_Host___FULL_SPEED_VALUE;
		}
		else if(host_Speed == USB_LL_Host___HPRT_HIGH_SPEED_VALUE)
		{
			speed = USB_LL_Interrupts_Host___HIGH_SPEED_VALUE;
		}

		host_Status[port_Number].is_Root_Device_Connection_Status_Change 	= true;
		host_Status[port_Number].is_Root_Device_Connected 					= true;
		host_Status[port_Number].is_Root_Device_Disconnected 				= false;
		host_Status[port_Number].root_Device_Speed 							= speed;
	}
}


void USB_LL_Interrupts_Host___Device_Disconnect_Detected(uint8_t port_Number)
{
	host_Status[port_Number].is_Root_Device_Connection_Status_Change 	= true;
	host_Status[port_Number].is_Root_Device_Connected 					= 	false;
	host_Status[port_Number].is_Root_Device_Disconnected 				= 	true;
}

void USB_LL_Interrupts_Host___Port_Interrupt_Handler(uint8_t port_Number)
{
	USB_OTG_HostPortTypeDef* USB_Host_Port = USB_LL_Hardware___Get_USB_Host_Port(port_Number);

	while((USB_Host_Port -> HPRT) & USB_LL_Host___PORT_INTERRUPTS_MASK)
	{
		switch(POSITION_VAL((USB_Host_Port -> HPRT) & USB_LL_Host___PORT_INTERRUPTS_MASK))
		{
		case(USB_OTG_HPRT_PCDET_Pos):
			USB_Host_Port -> HPRT |= (USB_OTG_HPRT_PCDET_Msk);
			USB_LL_Host___Reset_Port(port_Number);
			break;

		case(USB_OTG_HPRT_PENCHNG_Pos):
			USB_Host_Port -> HPRT = (USB_Host_Port -> HPRT & ~(USB_LL_Host___HPRT_RC_W1_BITS)) | (USB_OTG_HPRT_PENCHNG_Msk);
			if(USB_Host_Port -> HPRT & USB_OTG_HPRT_PCSTS)
			{
				USB_LL_Interrupts_Host___Device_Connect_Detected(port_Number);
			}
			else
			{
				USB_LL_Interrupts_Host___Device_Disconnect_Detected(port_Number);
			}
			break;
		}
	}
}

void USB_LL_Interrupts_Host___Channel_Interrupt_Handler(uint8_t port_Number)
{
	USB_OTG_HostTypeDef*		USB_Host 		= USB_LL_Hardware___Get_USB_Host(port_Number);
	uint8_t 					channel_Number 	= POSITION_VAL(USB_Host -> HAINT);
	USB_OTG_HostChannelTypeDef* USB_Host_Ch 	= USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	uint8_t 					device_Address 	= USB_LL_Hardware___GET_BIT_SEGMENT(USB_Host_Ch -> HCCHAR, USB_OTG_HCCHAR_DAD_Msk, USB_OTG_HCCHAR_DAD_Pos);

	while((USB_Host_Ch -> HCINT) & USB_LL_Host___CHANNEL_INTERRUPTS_MASK)
	{
		switch(POSITION_VAL((USB_Host_Ch -> HCINT) & USB_LL_Host___CHANNEL_INTERRUPTS_MASK))
		{
		case USB_OTG_HCINT_XFRC_Pos: 								// XFER Complete received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_XFRC_Msk;
			USB_LL_Interrupts_Host___Set_Channel_Status_Change_Flag(port_Number, channel_Number);
			host_Status[port_Number].channel_Status[channel_Number].device_Address = device_Address;
			host_Status[port_Number].channel_Status[channel_Number].status = USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_COMPLETE;
			break;

		case USB_OTG_HCINT_CHH_Pos: 								// channel halted
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_CHH_Msk;
			USB_LL_Interrupts_Host___Set_Channel_Status_Change_Flag(port_Number, channel_Number);
			host_Status[port_Number].channel_Status[channel_Number].device_Address = device_Address;
			host_Status[port_Number].channel_Status[channel_Number].status = USB_LL_Interrupts_Host___CHANNEL_STATUS_CHANNEL_HALTED;
			break;

		case USB_OTG_HCINT_STALL_Pos: 								// channel Stall received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_STALL_Msk;
			USB_LL_Interrupts_Host___Set_Channel_Status_Change_Flag(port_Number, channel_Number);
			host_Status[port_Number].channel_Status[channel_Number].device_Address = device_Address;
			host_Status[port_Number].channel_Status[channel_Number].status = USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_FAILED_STALL;
			break;

		case USB_OTG_HCINT_NAK_Pos: 								// NAK received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_NAK_Msk;
			if (USB_LL_Host___Channel_Get_Retries_Remaining(port_Number, channel_Number) > 0)
			{
				if(USB_LL_Host___Channel_Get_Transfer_Direction(port_Number, channel_Number) == USB_LL_Host___TRANSFER_DIRECTION_OUT)
				{
					USB_LL_Host___Channel_Retry_Transfer_Out(port_Number, channel_Number);
				}
				else
				{
					USB_LL_Host___Channel_Retry_Transfer_In(port_Number, channel_Number);
				}
			}
			else
			{
				USB_LL_Interrupts_Host___Set_Channel_Status_Change_Flag(port_Number, channel_Number);
				host_Status[port_Number].channel_Status[channel_Number].device_Address = device_Address;
				host_Status[port_Number].channel_Status[channel_Number].status = USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_FAILED_NAK;
			}
			break;

		case USB_OTG_HCINT_ACK_Pos: 										// ACK received
			USB_Host_Ch->HCINT = USB_OTG_HCINT_ACK_Msk;
			if(USB_LL_Host___Channel_Get_Transfer_Direction(port_Number, channel_Number) == USB_LL_Host___TRANSFER_DIRECTION_OUT)
			{
				USB_LL_Host___Channel_Out_Packet_Acknowledged(port_Number, channel_Number);
			}
			else
			{
				USB_LL_Host___Channel_In_Packet_Acknowledged(port_Number, channel_Number);
			}
			break;

		case USB_OTG_HCINT_TXERR_Pos: 								// TX ERROR received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_TXERR_Msk;
			USB_LL_Interrupts_Host___Set_Channel_Status_Change_Flag(port_Number, channel_Number);
			host_Status[port_Number].channel_Status[channel_Number].device_Address = device_Address;
			host_Status[port_Number].channel_Status[channel_Number].status = USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_FAILED_ERROR;
			break;

		case USB_OTG_HCINT_FRMOR_Pos: 								// Frame Error received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_FRMOR_Msk;
			USB_LL_Interrupts_Host___Set_Channel_Status_Change_Flag(port_Number, channel_Number);
			host_Status[port_Number].channel_Status[channel_Number].device_Address = device_Address;
			host_Status[port_Number].channel_Status[channel_Number].status = USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_FAILED_ERROR;
			break;
		}
	}
}
