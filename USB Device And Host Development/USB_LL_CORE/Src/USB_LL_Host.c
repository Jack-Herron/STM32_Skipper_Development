/*
 * USB_LL_Host.c
 *
 *  Created on: Jun 26, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>
#include <USB_LL.h>
#include <USB_LL.h>
#include <STDBool.h>
#include <stdio.h>

#include "../Inc/USB_LL_Definitions.h"
#include "../Inc/USB_LL_Host.h"

static USB_LL_Host___Status_TypeDef USB_LL_Host___Status[USB_LL_Definitions___NUMBER_OF_PORTS];

// ---------------------------- USB HOST CONTROL SECTION -----------------------------


void USB_LL_Host___Reset_Port(uint8_t port_Number)
{
	USB_OTG_HostPortTypeDef* USB_Host_Port = USB_LL___Get_USB_Host_Port(port_Number);
	USB_Host_Port -> HPRT = (USB_Host_Port -> HPRT & ~(USB_LL_Host___HPRT_RC_W1_BITS)) | (USB_OTG_HPRT_PRST);
	Skipper_Clock___Delay_ms(USB_LL_Host___HOST_RESET_DELAY);
	USB_Host_Port -> HPRT = (USB_Host_Port -> HPRT & ~(USB_LL_Host___HPRT_RC_W1_BITS)) & ~(USB_OTG_HPRT_PRST);;
}

void USB_LL_Host___Set_FIFO_Size(uint8_t port_Number, uint32_t RX_FIFO_Depth, uint32_t non_Periodic_TX_FIFO_Depth, uint32_t periodic_TX_FIFO_Depth)
{
	USB_OTG_GlobalTypeDef* USB = USB_LL___Get_USB(port_Number);
	uint32_t Start_Address = 0;

	USB -> GRXFSIZ = (RX_FIFO_Depth);

	Start_Address += RX_FIFO_Depth;

	USB -> DIEPTXF0_HNPTXFSIZ = ((non_Periodic_TX_FIFO_Depth << USB_OTG_NPTXFD_Pos) | (Start_Address << USB_OTG_NPTXFSA_Pos));

	Start_Address += non_Periodic_TX_FIFO_Depth;

	USB -> HPTXFSIZ = ((periodic_TX_FIFO_Depth << USB_OTG_HPTXFSIZ_PTXFD_Pos) | (Start_Address << USB_OTG_HPTXFSIZ_PTXSA_Pos));
}

uint32_t USB_LL_Host___Get_FIFO_Space_Available(uint8_t port_Number, uint8_t FIFO_Type)
{
	switch(FIFO_Type)
	{
	case USB_LL_Host___NON_PERIODIC_TX_FIFO:
	{
		USB_OTG_GlobalTypeDef* USB = USB_LL___Get_USB(port_Number);
		return(USB_LL___GET_BIT_SEGMENT(USB -> HNPTXSTS, USB_OTG_GNPTXSTS_NPTXFSAV_Msk, USB_OTG_GNPTXSTS_NPTXFSAV_Pos));
		break;
	}
	case USB_LL_Host___PERIODIC_TX_FIFO:
	{
		USB_OTG_HostTypeDef* USB_Host = USB_LL___Get_USB_Host(port_Number);
		return(USB_LL___GET_BIT_SEGMENT(USB_Host -> HPTXSTS, USB_OTG_HPTXSTS_PTXFSAVL_Msk, USB_OTG_HPTXSTS_PTXFSAVL_Pos));
		break;
	}
	}
	return(0);
}

uint16_t USB_LL_Host___Host_Get_Frame_Number(uint8_t port_Number)
{
	USB_OTG_HostTypeDef* USB_Host = USB_LL___Get_USB_Host(port_Number);
	return(USB_Host->HFNUM & USB_OTG_HFNUM_FRNUM_Msk);
}

// -----------------------------------------------------------------------------------

// ---------------------------- USB CHANNEL_CONTROL_SECTION --------------------------

//
void USB_LL_Host___Channel_Set_Retries_Remaining(uint8_t port_Number, uint8_t channel_Number, uint16_t number_Of_Retries)
{
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].retries_Remaining = number_Of_Retries;
}

uint32_t USB_LL_Host___Channel_Get_Transfer_Size(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Size);
}

void USB_LL_Host___Channel_Set_Transfer_Size(uint8_t port_Number, uint8_t channel_Number, uint32_t transfer_Size)
{
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Size =	transfer_Size;
}

uint32_t USB_LL_Host___Channel_Get_Transfer_Progress(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Progress);
}

void USB_LL_Host___Channel_Reset_Transfer_Progress(uint8_t port_Number, uint8_t channel_Number)
{
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Progress =	0;

}

void USB_LL_Host___Channel_Reset_Buffer_Fill_Level(uint8_t port_Number, uint8_t channel_Number)
{
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].buffer_Fill_Level =	0;

}
uint32_t USB_LL_Host___Channel_Get_Transfer_Size_Remaining(uint8_t port_Number, uint8_t channel_Number)
{
	uint32_t transfer_Size = USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Size;
	uint32_t transfer_Progress = USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Progress;

	return (transfer_Size - transfer_Progress);
}

uint8_t* USB_LL_Host___Channel_Get_Buffer_Pointer(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Status[port_Number].channel_Status[channel_Number].p_Buffer);
}

void USB_LL_Host___Channel_Set_Buffer_Pointer(uint8_t port_Number, uint8_t channel_Number, uint8_t *buffer_Pointer)
{
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].p_Buffer = buffer_Pointer;
}

uint16_t USB_LL_Host___Channel_Get_Packet_Size(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Status[port_Number].channel_Status[channel_Number].packet_Size);
}

uint8_t USB_LL_Host___Channel_Get_Transfer_Direction(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Direction);
}

void USB_LL_Host___Flush_RX_FIFO(uint8_t port_Number)
{
	USB_OTG_GlobalTypeDef *USB = USB_LL___Get_USB(port_Number);
	USB -> GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;
	while(USB -> GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH);
}

int8_t USB_LL_Host___Channel_RX_POP(uint8_t port_Number, uint8_t channel_Number, uint8_t* p_Buffer, uint32_t RX_Status)
{
	uint32_t 	USB_offset 				= USB_LL___Get_USB_BASE(port_Number);
	uint32_t 	transfer_Size 			= USB_LL___GET_BIT_SEGMENT(RX_Status,USB_OTG_GRXSTSP_BCNT_Msk,USB_OTG_GRXSTSP_BCNT_Pos);
	uint32_t* 	fifo 					= (uint32_t*)(USB_offset + USB_OTG_FIFO_BASE + (USB_OTG_FIFO_SIZE * channel_Number));

	if((USB_LL_Host___Status[port_Number].channel_Status[channel_Number].buffer_Fill_Level + transfer_Size) <= USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Size)
	{
		USB_LL___FIFO_Transfer_Out(fifo, p_Buffer, transfer_Size);
		USB_LL_Host___Status[port_Number].channel_Status[channel_Number].buffer_Fill_Level += transfer_Size;
	}
	else
	{
		return(USB_LL_Host___EXIT_FAILURE_BUFFER_OVERFLOW);
	}

	return(USB_LL_Host___EXIT_SUCCESS);
}

uint32_t* USB_LL_Host___Channel_Get_Fifo_Pointer(uint8_t port_Number, uint8_t channel_Number)
{
	uint32_t USB_offset = USB_LL___Get_USB_BASE(port_Number);
	return((uint32_t*)(USB_offset + USB_OTG_FIFO_BASE + (USB_OTG_FIFO_SIZE * channel_Number)));
}

void USB_LL_Host___Channel_Halt(uint8_t port_Number, uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel 	= USB_LL___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_OTG_HostTypeDef* 		USB_Host 			= USB_LL___Get_USB_Host(port_Number);

	if (!(USB_Host->HPTXSTS & USB_OTG_HPTXSTS_PTXQSAV_Msk))
	{
		printf("this should not happen\n");
		USB_Host_Channel->HCCHAR |= USB_OTG_HCCHAR_CHDIS;
		USB_Host_Channel->HCCHAR |= USB_OTG_HCCHAR_CHENA;
	}

	USB_Host_Channel -> HCCHAR |= (USB_OTG_HCCHAR_CHDIS | USB_OTG_HCCHAR_CHENA);
}

uint8_t USB_LL_Host___Channel_Get_Current_Packet_ID(uint8_t port_Number, uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel = USB_LL___Get_USB_Host_Channel(port_Number, channel_Number);
	return(USB_LL___GET_BIT_SEGMENT(USB_Host_Channel -> HCTSIZ, USB_OTG_HCTSIZ_DPID_Msk, USB_OTG_HCTSIZ_DPID_Pos));
}

void USB_LL_Host___Channel_Set_Interrupts(uint8_t port_Number,uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel 	= USB_LL___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_OTG_HostTypeDef*		USB_Host 			= USB_LL___Get_USB_Host(port_Number);
	USB_Host 			-> HAINTMSK |= (1 << channel_Number);
	USB_Host_Channel 	-> HCINTMSK  = USB_LL_Host___CHANNEL_INTERRUPTS_MASK;
}

void USB_LL_Host___Setup_Channel
			(
			uint8_t port_Number,
			uint8_t channel_Number,
			uint16_t max_Packet_Size,
			uint8_t endpoint_Number,
			uint8_t endpoint_Direction,
			uint8_t low_Speed_Device,
			uint8_t endpoint_Type,
			uint8_t multi_Count,
			uint8_t device_Address,
			uint8_t odd_Frame,
			uint32_t transfer_Size,
			uint32_t packet_Count,
			uint8_t packet_ID
			)
{

	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].packet_Size 			= max_Packet_Size;
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Direction 	= endpoint_Direction;
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Complete		= false;
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Complete_Flag = false;
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].busy 					= true;

	USB_OTG_HostChannelTypeDef* USB_Host_Channel = USB_LL___Get_USB_Host_Channel(port_Number, channel_Number);

	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].retry_After_Halt = 0;

	if(transfer_Size < max_Packet_Size)
	{
		max_Packet_Size = transfer_Size;
	}

	uint32_t channel_Characteristics =
			(
			max_Packet_Size 	<< USB_OTG_HCCHAR_MPSIZ_Pos |
			endpoint_Number 	<< USB_OTG_HCCHAR_EPNUM_Pos |
			endpoint_Direction 	<< USB_OTG_HCCHAR_EPDIR_Pos |
			low_Speed_Device 	<< USB_OTG_HCCHAR_LSDEV_Pos |
			endpoint_Type 		<< USB_OTG_HCCHAR_EPTYP_Pos |
			multi_Count 		<< USB_OTG_HCCHAR_MC_Pos 	|
			device_Address 		<< USB_OTG_HCCHAR_DAD_Pos
			);

	USB_Host_Channel -> HCTSIZ = (transfer_Size << USB_OTG_HCTSIZ_XFRSIZ_Pos | packet_Count << USB_OTG_HCTSIZ_PKTCNT_Pos | packet_ID << USB_OTG_HCTSIZ_DPID_Pos);
	//printf("S%d\n", channel_Number);
	USB_Host_Channel -> HCCHAR = channel_Characteristics;

}

void USB_LL_Host___Channel_Setup_Buffer(uint8_t port_Number, uint8_t channel_Number, uint8_t *buffer_Pointer, uint32_t transfer_Size)
{
	USB_LL_Host___Channel_Set_Buffer_Pointer(port_Number, channel_Number, buffer_Pointer);
	USB_LL_Host___Channel_Set_Transfer_Size(port_Number, channel_Number, transfer_Size);
	USB_LL_Host___Channel_Reset_Transfer_Progress(port_Number, channel_Number);
	USB_LL_Host___Channel_Reset_Buffer_Fill_Level(port_Number, channel_Number);
}

void USB_LL_Host___Channel_Load_HCTSIZ(uint8_t port_Number, uint8_t channel_Number, uint32_t transfer_Size_In_Bytes, uint32_t packet_Count, uint8_t packet_ID)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel = USB_LL___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_Host_Channel -> HCTSIZ = (transfer_Size_In_Bytes << USB_OTG_HCTSIZ_XFRSIZ_Pos | packet_Count << USB_OTG_HCTSIZ_PKTCNT_Pos | packet_ID << USB_OTG_HCTSIZ_DPID_Pos);
}

void USB_LL_Host___Transfer_Next_Packet(uint8_t port_Number, uint8_t channel_Number)
{
	uint32_t* FIFO_Pointer 				= USB_LL_Host___Channel_Get_Fifo_Pointer(port_Number, channel_Number);
	uint32_t  transfer_Size_Remaining 	= USB_LL_Host___Channel_Get_Transfer_Size(port_Number, channel_Number) - USB_LL_Host___Channel_Get_Transfer_Progress(port_Number, channel_Number);
	uint16_t  transfer_Size 			= USB_LL_Host___GET_MIN(transfer_Size_Remaining, USB_LL_Host___Channel_Get_Packet_Size(port_Number, channel_Number));
	uint8_t*  buffer_Pointer			= USB_LL_Host___Channel_Get_Buffer_Pointer(port_Number, channel_Number);

	USB_LL___FIFO_Transfer_In(buffer_Pointer, FIFO_Pointer, transfer_Size);
}

void USB_LL_Host___Channel_Begin_Transfer_In(uint8_t port_Number, uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef *USB_Host_Channel = USB_LL___Get_USB_Host_Channel(port_Number, channel_Number);

	USB_Host_Channel -> HCCHAR &= ~(USB_OTG_HCCHAR_ODDFRM_Msk);
	USB_Host_Channel -> HCCHAR |= !((USB_LL_Host___Host_Get_Frame_Number(port_Number) % 2)) << USB_OTG_HCCHAR_ODDFRM_Pos;

	/*if((USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Size-USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Progress) < USB_LL_Host___Status[port_Number].channel_Status[channel_Number].packet_Size)
	{
		USB_Host_Channel -> HCCHAR &= ~USB_OTG_HCCHAR_MPSIZ_Msk;
		USB_Host_Channel -> HCCHAR |= (USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Size-USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Progress) << USB_OTG_HCCHAR_MPSIZ_Pos;
		//printf("adjusted %d to %ld\n", USB_LL_Host___Status[port_Number].channel_Status[channel_Number].packet_Size, USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Size-USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Progress);
	}*/

	USB_Host_Channel->HCCHAR |= USB_OTG_HCCHAR_CHENA;
}

void USB_LL_Host___Channel_Begin_Transfer_Out(uint8_t port_Number, uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef *USB_Host_Channel = USB_LL___Get_USB_Host_Channel(port_Number, channel_Number);

	USB_Host_Channel -> HCCHAR &= ~(USB_OTG_HCCHAR_ODDFRM_Msk);
	USB_Host_Channel -> HCCHAR |= !((USB_LL_Host___Host_Get_Frame_Number(port_Number) % 2)) << USB_OTG_HCCHAR_ODDFRM_Pos;

	USB_Host_Channel->HCCHAR |= USB_OTG_HCCHAR_CHENA;

	USB_LL_Host___Transfer_Next_Packet(port_Number, channel_Number);
}

void USB_LL_Host___Channel_Out_Packet_Acknowledged(uint8_t port_Number, uint8_t channel_Number)
{
	uint16_t  packet_Size 				= USB_LL_Host___Channel_Get_Packet_Size(port_Number, channel_Number);
	uint16_t  size_Transfered 			= USB_LL_Host___GET_MIN(packet_Size, (USB_LL_Host___Channel_Get_Transfer_Size(port_Number, channel_Number) - USB_LL_Host___Channel_Get_Transfer_Progress(port_Number, channel_Number)));

	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Progress += size_Transfered;

	uint16_t  transfer_Size_Remaining 	= USB_LL_Host___Channel_Get_Transfer_Size(port_Number, channel_Number) - USB_LL_Host___Channel_Get_Transfer_Progress(port_Number, channel_Number);

	if(transfer_Size_Remaining > 0)
	{
		USB_LL_Host___Transfer_Next_Packet(port_Number, channel_Number);
	}
}

void USB_LL_Host___Channel_In_Packet_Acknowledged(uint8_t port_Number, uint8_t channel_Number)
{
	uint16_t  packet_Size 				= USB_LL_Host___Channel_Get_Packet_Size(port_Number, channel_Number);
	uint16_t  size_Transfered 			= USB_LL_Host___GET_MIN(packet_Size, (USB_LL_Host___Channel_Get_Transfer_Size(port_Number, channel_Number) - USB_LL_Host___Channel_Get_Transfer_Progress(port_Number, channel_Number)));

	//USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Progress += size_Transfered;

	uint16_t  transfer_Size_Remaining 	= USB_LL_Host___Channel_Get_Transfer_Size(port_Number, channel_Number) - USB_LL_Host___Channel_Get_Transfer_Progress(port_Number, channel_Number);

	if(transfer_Size_Remaining > 0)
	{
		//USB_LL_Host___Channel_Begin_Transfer_In(port_Number, channel_Number);
	}
}

void USB_LL_Host___Channel_Set_Retry_After_Halt(uint8_t port_Number, uint8_t channel_Number, uint8_t value)
{
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].retry_After_Halt = value;
}

void USB_LL_Host___Channel_Retry_Transfer_Out(uint8_t port_Number, uint8_t channel_Number)
{
	USB_LL_Host___Channel_Begin_Transfer_Out(port_Number, channel_Number);
}

void USB_LL_Host___Channel_Retry_Transfer_In(uint8_t port_Number, uint8_t channel_Number)
{
	USB_LL_Host___Channel_Begin_Transfer_In(port_Number, channel_Number);
}

// -----------------------------------------------------------------------------------

uint8_t USB_LL_Host___Is_Start_Of_Frame(uint8_t port_Number)
{
	return(USB_LL_Host___Status[port_Number].is_Start_Of_Frame);
}

void USB_LL_Host___Clear_Start_Of_Frame(uint8_t port_Number)
{
	USB_LL_Host___Status[port_Number].is_Start_Of_Frame = false;
}

uint8_t USB_LL_Host___Get_Root_Device_Speed(uint8_t port_Number)
{
	uint8_t return_Value = USB_LL_Host___Status[port_Number].root_Device_Speed;
	return(return_Value);
}

void USB_LL_Host___Clear_Connection_Status_Change(uint8_t port_Number)
{
	USB_LL_Host___Status[port_Number].is_Root_Device_Connection_Status_Change = false;
}

uint8_t USB_LL_Host___Is_Root_Device_Connection_Status_Change(uint8_t port_Number)
{
	uint8_t return_Value = USB_LL_Host___Status[port_Number].is_Root_Device_Connection_Status_Change;
	return(return_Value);
}

uint8_t USB_LL_Host___Is_Root_Device_Connected(uint8_t port_Number)
{
	return(USB_LL_Host___Status[port_Number].is_Root_Device_Connected);
}

uint8_t USB_LL_Host___Is_Root_Device_Disconnected(uint8_t port_Number)
{
	return(USB_LL_Host___Status[port_Number].is_Root_Device_Disconnected);
}

uint8_t USB_LL_Host___Get_All_Channels_Status_Change_Flag(uint8_t port_Number)
{
	return (USB_LL_Host___Status[port_Number].all_Channels_Status_Change_Flag);
}

void USB_LL_Host___Clear_All_Channels_Status_Change_Flag(uint8_t port_Number)
{
	USB_LL_Host___Status[port_Number].all_Channels_Status_Change_Flag = false;
}

uint8_t USB_LL_Host___Get_Channel_Status(uint8_t port_Number, uint8_t channel_Number)
{
	return(USB_LL_Host___Status[port_Number].channel_Status[channel_Number].exit_Status);
}

uint8_t USB_LL_Host___Get_Channel_Status_Change_Flag(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Complete);
}

uint8_t USB_LL_Host___Channel_Is_Busy(uint8_t port_Number, uint8_t channel_Number)
{
	return(USB_LL_Host___Status[port_Number].channel_Status[channel_Number].busy);
}

void USB_LL_Host___Transfer_Complete(uint8_t port_Number, uint8_t channel_Number, uint8_t status)
{
	//printf("E%d\n", channel_Number);
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Complete 			= true;
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Complete_Flag 	= true;
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].exit_Status 				= status;
}

void USB_LL_Host___Channel_Clear_Request_Queue(uint8_t port_Number, uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel 	= USB_LL___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_OTG_GlobalTypeDef* USB = USB_LL___Get_USB(port_Number);

	//USB->GRSTCTL |= (USB_OTG_GRSTCTL_RXFFLSH);
	//while(USB->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH);
}

void USB_LL_Host___Channel_Receive_Overflow(uint8_t port_Number, uint8_t channel_Number)
{
	USB_LL_Host___Channel_Clear_Request_Queue(port_Number, channel_Number);
	//USB_LL_Host___Channel_Halt(port_Number, channel_Number);
	printf("OVERFLOW\n");
	USB_LL_Host___Transfer_Complete(port_Number, channel_Number, USB_LL_Host___CHANNEL_STATUS_TRANSFER_FAILED_ERROR);
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].busy = false;
}

void USB_LL_Host___Packet_Received(uint8_t port_Number)
{
	USB_OTG_GlobalTypeDef* 		USB 				= USB_LL___Get_USB(port_Number);

	uint32_t 					RX_Status        	= USB -> GRXSTSP;
	uint8_t 					channel_Number     	= USB_LL___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_EPNUM_Msk, USB_OTG_GRXSTSP_EPNUM_Pos);

	USB_OTG_HostChannelTypeDef* USB_Host_Channel 	= USB_LL___Get_USB_Host_Channel(port_Number, channel_Number);

	uint8_t						packets_Remaining 	= USB_LL___GET_BIT_SEGMENT(USB_Host_Channel -> HCTSIZ, USB_OTG_HCTSIZ_PKTCNT_Msk, USB_OTG_HCTSIZ_PKTCNT_Pos);
	uint8_t 					packet_Status 		= USB_LL___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_PKTSTS_Msk, USB_OTG_GRXSTSP_PKTSTS_Pos);
	uint8_t 					byte_Count			= USB_LL___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_BCNT_Msk, USB_OTG_GRXSTSP_BCNT_Pos);

	uint8_t* 					p_Channel_Buffer    = USB_LL_Host___Status[port_Number].channel_Status[channel_Number].p_Buffer + USB_LL_Host___Status[port_Number].channel_Status[channel_Number].buffer_Fill_Level;

	if(packet_Status == USB_LL_Host___RX_PACKET_STATUS_DATA_PACKET_RECIEVED)
	{
		if(byte_Count + USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Progress > USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Size)
		{
			uint8_t temp[byte_Count];

			byte_Count = 0;

			(void)USB_LL_Host___Channel_RX_POP(port_Number, channel_Number, temp, RX_Status);

			USB_LL_Host___Channel_Receive_Overflow(port_Number, channel_Number);
		}

		USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Progress += byte_Count;

		if(byte_Count > 0)
		{
			(void)USB_LL_Host___Channel_RX_POP(port_Number, channel_Number, p_Channel_Buffer, RX_Status);

			if(packets_Remaining > 0)
			{
				USB_Host_Channel -> HCCHAR |= (USB_OTG_HCCHAR_CHENA);
			}
		}
	}
}


void USB_LL_Host___Device_Connect_Detected(uint8_t port_Number)
{
	USB_OTG_HostPortTypeDef*	USB_Host_Port 	= USB_LL___Get_USB_Host_Port(port_Number);
	USB_OTG_HostTypeDef* 		USB_Host 		= USB_LL___Get_USB_Host(port_Number);
	if(!(USB_Host -> HCFG & USB_OTG_HCFG_FSLSPCS))
	{
		USB_Host -> HFIR  = (USB_LL_HOST___CLOCK_CYCLES_IN_ONE_MILLISECOND_FRAME);
		USB_Host -> HCFG |= (USB_OTG_HCFG_FSLSPCS_0);
		USB_LL_Host___Reset_Port(port_Number);
	}
	else
	{
		uint8_t host_Speed = USB_LL___GET_BIT_SEGMENT(USB_Host_Port -> HPRT, USB_OTG_HPRT_PSPD_Msk, USB_OTG_HPRT_PSPD_Pos);
		uint8_t speed;

		if(host_Speed == USB_LL_Host___HPRT_LOW_SPEED_VALUE)
		{
			speed = USB_LL_Host___LOW_SPEED_VALUE;
		}
		else if(host_Speed == USB_LL_Host___HPRT_FULL_SPEED_VALUE)
		{
			speed = USB_LL_Host___FULL_SPEED_VALUE;
		}
		else if(host_Speed == USB_LL_Host___HPRT_HIGH_SPEED_VALUE)
		{
			speed = USB_LL_Host___HIGH_SPEED_VALUE;
		}

		USB_LL_Host___Status[port_Number].is_Root_Device_Connection_Status_Change 	= true;
		USB_LL_Host___Status[port_Number].is_Root_Device_Connected 					= true;
		USB_LL_Host___Status[port_Number].is_Root_Device_Disconnected 				= false;
		USB_LL_Host___Status[port_Number].root_Device_Speed 							= speed;
	}
}


void USB_LL_Host___Device_Disconnect_Detected(uint8_t port_Number)
{
	USB_LL_Host___Status[port_Number].is_Root_Device_Connection_Status_Change 	= true;
	USB_LL_Host___Status[port_Number].is_Root_Device_Connected 					= 	false;
	USB_LL_Host___Status[port_Number].is_Root_Device_Disconnected 				= 	true;
}

void USB_LL_Host___Start_Of_Frame_Interrupt_Received(uint8_t port_Number)
{
	USB_LL_Host___Status[port_Number].is_Start_Of_Frame = true;
}

void USB_LL_Host___Port_Interrupt_Handler(uint8_t port_Number)
{
	USB_OTG_HostPortTypeDef* USB_Host_Port = USB_LL___Get_USB_Host_Port(port_Number);

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
				USB_LL_Host___Device_Connect_Detected(port_Number);
			}
			else
			{
				USB_LL_Host___Device_Disconnect_Detected(port_Number);
			}
			break;
		}
	}
}

uint8_t USB_LL_Host___Get_Transfer_Complete_Flag(uint8_t port_Number, uint8_t channel_Number)
{
	return(USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Complete_Flag);
}

void USB_LL_Host___Clear_Transfer_Complete_Flag(uint8_t port_Number, uint8_t channel_Number)
{
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Complete_Flag = false;
}

// returns true if retransmission was successful, false if no retries remaining
uint8_t USB_LL_Host___Channel_Attempt_Retransmission(uint8_t port_Number, uint8_t channel_Number, uint8_t always_Halt)
{
	uint16_t retries_Remaining = USB_LL_Host___Status[port_Number].channel_Status[channel_Number].retries_Remaining;

	if (retries_Remaining > 0)
	{
		if(USB_LL_Host___Channel_Get_Transfer_Direction(port_Number, channel_Number) == USB_LL_Host___TRANSFER_DIRECTION_OUT)
		{
			USB_LL_Host___Channel_Halt(port_Number, channel_Number);
			USB_LL_Host___Channel_Set_Retry_After_Halt(port_Number, channel_Number, true);
		}
		else
		{
			if(always_Halt)
			{
				USB_LL_Host___Channel_Halt(port_Number, channel_Number);
			}
			USB_LL_Host___Channel_Retry_Transfer_In(port_Number, channel_Number);
		}

		USB_LL_Host___Channel_Set_Retries_Remaining(port_Number, channel_Number, retries_Remaining - 1);

		return(true);
	}

	return(false);
}

void USB_LL_Host___Channel_Interrupt_Transfer_Complete(uint8_t port_Number, uint8_t channel_Number)
{
	USB_LL_Host___Transfer_Complete(port_Number, channel_Number, USB_LL_Host___CHANNEL_STATUS_TRANSFER_COMPLETE);
	USB_LL_Host___Status[port_Number].channel_Status[channel_Number].busy = false;
}

void USB_LL_Host___Channel_Interrupt_Channel_Halted(uint8_t port_Number, uint8_t channel_Number)
{
	if(USB_LL_Host___Status[port_Number].channel_Status[channel_Number].retry_After_Halt)
	{
		if (USB_LL_Host___Channel_Get_Transfer_Direction(port_Number, channel_Number) == USB_LL_Host___TRANSFER_DIRECTION_OUT)
		{
			USB_LL_Host___Channel_Retry_Transfer_Out(port_Number, channel_Number);
		}
		else
		{
			USB_LL_Host___Channel_Retry_Transfer_In(port_Number, channel_Number);
		}

		USB_LL_Host___Channel_Set_Retry_After_Halt(port_Number, channel_Number, false);
	}
	else
	{
		if(!USB_LL_Host___Status[port_Number].channel_Status[channel_Number].transfer_Complete)
		{
			USB_LL_Host___Transfer_Complete(port_Number, channel_Number, USB_LL_Host___CHANNEL_STATUS_TRANSFER_FAILED_ERROR);
		}
		USB_LL_Host___Status[port_Number].channel_Status[channel_Number].busy = false;
	}
}

void USB_LL_Host___Channel_Interrupt_Channel_Stalled(uint8_t port_Number, uint8_t channel_Number)
{

	USB_LL_Host___Transfer_Complete(port_Number, channel_Number, USB_LL_Host___CHANNEL_STATUS_TRANSFER_FAILED_STALL);
}

void USB_LL_Host___Channel_Interrupt_No_Acknowledgment(uint8_t port_Number, uint8_t channel_Number)
{
	if(!USB_LL_Host___Channel_Attempt_Retransmission(port_Number, channel_Number, false))
	{
		USB_LL_Host___Channel_Halt(port_Number, channel_Number);
		USB_LL_Host___Transfer_Complete(port_Number, channel_Number, USB_LL_Host___CHANNEL_STATUS_TRANSFER_FAILED_NAK);
	}
}

void USB_LL_Host___Channel_Interrupt_Channel_Acknowledged(uint8_t port_Number, uint8_t channel_Number)
{
	if(USB_LL_Host___Channel_Get_Transfer_Direction(port_Number, channel_Number) == USB_LL_Host___TRANSFER_DIRECTION_OUT)
	{
		USB_LL_Host___Channel_Out_Packet_Acknowledged(port_Number, channel_Number);
	}
	else
	{
		USB_LL_Host___Channel_In_Packet_Acknowledged(port_Number, channel_Number);
	}
}

void USB_LL_Host___Channel_Interrupt_Channel_TX_Error(uint8_t port_Number, uint8_t channel_Number)
{
	if(!USB_LL_Host___Channel_Attempt_Retransmission(port_Number, channel_Number, true))
	{
		USB_LL_Host___Channel_Halt(port_Number, channel_Number);
		USB_LL_Host___Transfer_Complete(port_Number, channel_Number, USB_LL_Host___CHANNEL_STATUS_TRANSFER_FAILED_ERROR);
	}
}

void USB_LL_Host___Channel_Interrupt_Frame_Error(uint8_t port_Number, uint8_t channel_Number)
{
	if(!USB_LL_Host___Channel_Attempt_Retransmission(port_Number, channel_Number, true))
	{
		USB_LL_Host___Channel_Halt(port_Number, channel_Number);
		USB_LL_Host___Transfer_Complete(port_Number, channel_Number, USB_LL_Host___CHANNEL_STATUS_TRANSFER_FAILED_ERROR);
	}
}

void USB_LL_Host___Channel_Interrupt_Data_Toggle_Error(uint8_t port_Number, uint8_t channel_Number)
{
	USB_LL_Host___Channel_Halt(port_Number, channel_Number);
	USB_LL_Host___Transfer_Complete(port_Number, channel_Number, USB_LL_Host___CHANNEL_STATUS_TRANSFER_FAILED_ERROR);
}

void USB_LL_Host___Channel_Interrupt_Handler(uint8_t port_Number)
{
	USB_OTG_HostTypeDef*		USB_Host 		= USB_LL___Get_USB_Host(port_Number);
	uint8_t 					channel_Number 	= POSITION_VAL(USB_Host -> HAINT);
	USB_OTG_HostChannelTypeDef* USB_Host_Ch 	= USB_LL___Get_USB_Host_Channel(port_Number, channel_Number);

	while((USB_Host_Ch -> HCINT) & USB_LL_Host___CHANNEL_INTERRUPTS_MASK)
	{
		switch(POSITION_VAL((USB_Host_Ch -> HCINT) & USB_LL_Host___CHANNEL_INTERRUPTS_MASK))
		{
		case USB_OTG_HCINT_XFRC_Pos: 								// XFER Complete received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_XFRC_Msk;

			USB_LL_Host___Channel_Interrupt_Transfer_Complete(port_Number, channel_Number);
			break;

		case USB_OTG_HCINT_CHH_Pos: 								// channel halted
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_CHH_Msk;

			USB_LL_Host___Channel_Interrupt_Channel_Halted(port_Number, channel_Number);

			break;

		case USB_OTG_HCINT_STALL_Pos: 								// channel Stall received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_STALL_Msk;

			USB_LL_Host___Channel_Interrupt_Channel_Stalled(port_Number, channel_Number);

			break;

		case USB_OTG_HCINT_NAK_Pos: 								// NAK received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_NAK_Msk;
			//printf("NAK %ld\n", (USB_Host_Ch->HCCHAR & USB_OTG_HCCHAR_DAD_Msk) >> USB_OTG_HCCHAR_DAD_Pos);
			USB_LL_Host___Channel_Interrupt_No_Acknowledgment(port_Number, channel_Number);

			break;

		case USB_OTG_HCINT_ACK_Pos: 										// ACK received
			USB_Host_Ch->HCINT = USB_OTG_HCINT_ACK_Msk;

			USB_LL_Host___Channel_Interrupt_Channel_Acknowledged(port_Number, channel_Number);

			break;

		case USB_OTG_HCINT_TXERR_Pos: 								// TX ERROR received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_TXERR_Msk;
			USB_LL_Host___Channel_Interrupt_Channel_TX_Error(port_Number, channel_Number);

			break;

		case USB_OTG_HCINT_FRMOR_Pos: 								// Frame Error received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_FRMOR_Msk;
			printf("FRMOR\n");
			USB_LL_Host___Channel_Interrupt_Frame_Error(port_Number, channel_Number);

			break;

		case USB_OTG_HCINT_BBERR_Pos: 								// Frame Error received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_BBERR_Msk;
			printf("BBERR%d\n", channel_Number);
			USB_LL_Host___Channel_Interrupt_Frame_Error(port_Number, channel_Number);

			break;

		case USB_OTG_HCINT_DTERR_Pos: 								// Frame Error received
			USB_Host_Ch -> HCINT = USB_OTG_HCINT_DTERR_Msk;
			USB_LL_Host___Channel_Interrupt_Data_Toggle_Error(port_Number, channel_Number);

			break;
		}
	}
}
