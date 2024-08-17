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

#include "../Inc/USB_LL_Hardware.h"
#include "../Inc/USB_LL_Definitions.h"
#include "../Inc/USB_LL_Hardware.h"
#include "../Inc/USB_LL_Interrupts.h"
#include "../Inc/USB_LL_Host.h"

static USB_LL_Host___Host_Port_TypeDef USB_LL_Host___Host_Port[USB_LL_Definitions___NUMBER_OF_PORTS];

// ---------------------------- USB HOST CONTROL SECTION -----------------------------

void USB_LL_Host___Reset_Port(uint8_t port_Number)
{
	USB_OTG_HostPortTypeDef* USB_Host_Port = USB_LL_Hardware___Get_USB_Host_Port(port_Number);
	USB_Host_Port -> HPRT = (USB_Host_Port -> HPRT & ~(USB_LL_Host___HPRT_RC_W1_BITS)) | (USB_OTG_HPRT_PRST);
	Skipper_Clock___Delay_ms(USB_LL_Host___HOST_RESET_DELAY);
	USB_Host_Port -> HPRT = (USB_Host_Port -> HPRT & ~(USB_LL_Host___HPRT_RC_W1_BITS)) & ~(USB_OTG_HPRT_PRST);;
}

void USB_LL_Host___Set_FIFO_Size(uint8_t port_Number, uint32_t RX_FIFO_Depth, uint32_t non_Periodic_TX_FIFO_Depth, uint32_t periodic_TX_FIFO_Depth)
{
	USB_OTG_GlobalTypeDef* USB = USB_LL_Hardware___Get_USB(port_Number);
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
		USB_OTG_GlobalTypeDef* USB = USB_LL_Hardware___Get_USB(port_Number);
		return(USB_LL_Hardware___GET_BIT_SEGMENT(USB -> HNPTXSTS, USB_OTG_GNPTXSTS_NPTXFSAV_Msk, USB_OTG_GNPTXSTS_NPTXFSAV_Pos));
		break;
	}
	case USB_LL_Host___PERIODIC_TX_FIFO:
	{
		USB_OTG_HostTypeDef* USB_Host = USB_LL_Hardware___Get_USB_Host(port_Number);
		return(USB_LL_Hardware___GET_BIT_SEGMENT(USB_Host -> HPTXSTS, USB_OTG_HPTXSTS_PTXFSAVL_Msk, USB_OTG_HPTXSTS_PTXFSAVL_Pos));
		break;
	}
	}
	return(0);
}

uint16_t USB_LL_Host___Get_Frame_Number(uint8_t port_Number)
{
	USB_OTG_HostTypeDef* USB_Host = USB_LL_Hardware___Get_USB_Host(port_Number);
	return(USB_Host->HFNUM & USB_OTG_HFNUM_FRNUM_Msk);
}

// -----------------------------------------------------------------------------------

// ---------------------------- USB CHANNEL_CONTROL_SECTION --------------------------

uint16_t USB_LL_Host___Channel_Get_Retries_Remaining(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].retries_Remaining);
}

uint8_t USB_LL_Host___Channel_Get_Retry_After_Halt(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].retry_After_Halt);
}

uint32_t USB_LL_Host___Channel_Get_Transfer_Size(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Size);
}

void USB_LL_Host___Channel_Set_Transfer_Size(uint8_t port_Number, uint8_t channel_Number, uint32_t transfer_Size)
{
	USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Size =	transfer_Size;
}

uint32_t USB_LL_Host___Channel_Get_Transfer_Progress(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Progress);
}

void USB_LL_Host___Channel_Reset_Transfer_Progress(uint8_t port_Number, uint8_t channel_Number)
{
	USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Progress =	0;

}

uint32_t USB_LL_Host___Channel_Get_Transfer_Size_Remaining(uint8_t port_Number, uint8_t channel_Number)
{
	uint32_t transfer_Size = USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Size;
	uint32_t transfer_Progress = USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Progress;

	return (transfer_Size - transfer_Progress);
}

uint8_t* USB_LL_Host___Channel_Get_Buffer_Pointer(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].p_Buffer);
}

void USB_LL_Host___Channel_Set_Buffer_Pointer(uint8_t port_Number, uint8_t channel_Number, uint8_t *buffer_Pointer)
{
	USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].p_Buffer = buffer_Pointer;
}

uint16_t USB_LL_Host___Channel_Get_Packet_Size(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].packet_Size);
}

uint8_t USB_LL_Host___Channel_Get_Transfer_Direction(uint8_t port_Number, uint8_t channel_Number)
{
	return (USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Direction);
}

int8_t USB_LL_Host___Channel_RX_POP(uint8_t port_Number, uint8_t channel_Number, uint32_t RX_Status)
{
	uint32_t 	USB_offset 				= USB_LL_Hardware___Get_USB_BASE(port_Number);
	uint32_t 	transfer_Size 			= USB_LL_Hardware___GET_BIT_SEGMENT(RX_Status,USB_OTG_GRXSTSP_BCNT_Msk,USB_OTG_GRXSTSP_BCNT_Pos);
	uint32_t* 	fifo 					= (uint32_t*)(USB_offset + USB_OTG_FIFO_BASE + (USB_OTG_FIFO_SIZE * channel_Number));
	uint8_t* 	p_Buffer	  			= USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].p_Buffer + USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Progress;

	if((USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Progress + transfer_Size) <= USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Size)
	{
		USB_LL_Hardware___FIFO_Transfer_Out(fifo, p_Buffer, transfer_Size);
		//USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Progress += transfer_Size;
	}
	else
	{
		return(USB_LL_Host___EXIT_FAILURE_BUFFER_OVERFLOW);
	}

	return(USB_LL_Host___EXIT_SUCCESS);
}

uint32_t* USB_LL_Host___Channel_Get_Fifo_Pointer(uint8_t port_Number, uint8_t channel_Number)
{
	uint32_t USB_offset = USB_LL_Hardware___Get_USB_BASE(port_Number);
	return((uint32_t*)(USB_offset + USB_OTG_FIFO_BASE + (USB_OTG_FIFO_SIZE * channel_Number)));
}

void USB_LL_Host___Channel_Halt(uint8_t port_Number, uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel = USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_Host_Channel -> HCCHAR |= (USB_OTG_HCCHAR_CHDIS | USB_OTG_HCCHAR_CHENA);
}

uint8_t USB_LL_Host___Channel_Get_Current_Packet_ID(uint8_t port_Number, uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel = USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	return(USB_LL_Hardware___GET_BIT_SEGMENT(USB_Host_Channel -> HCTSIZ, USB_OTG_HCTSIZ_DPID_Msk, USB_OTG_HCTSIZ_DPID_Pos));
}

void USB_LL_Host___Channel_Set_Interrupts(uint8_t port_Number,uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel 	= USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_OTG_HostTypeDef*		USB_Host 			= USB_LL_Hardware___Get_USB_Host(port_Number);
	USB_Host 			-> HAINTMSK |= (1 << channel_Number);
	USB_Host_Channel 	-> HCINTMSK  = USB_LL_Host___CHANNEL_INTERRUPTS_MASK;
}

void USB_LL_Host___Channel_Set_Number_Of_Retries(uint8_t port_Number, uint8_t channel_Number, uint16_t number_Of_Retries)
{
	USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].retries_Remaining = number_Of_Retries;
}

void USB_LL_Host___Channel_Set_Characteristics
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
			uint8_t odd_Frame
			)
{

	USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].packet_Size 		= max_Packet_Size;
	USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Direction 	= endpoint_Direction;

	USB_OTG_HostChannelTypeDef* USB_Host_Channel = USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);

	uint32_t channel_Characteristics =
			(
			max_Packet_Size 	<< USB_OTG_HCCHAR_MPSIZ_Pos |
			endpoint_Number 	<< USB_OTG_HCCHAR_EPNUM_Pos |
			endpoint_Direction 	<< USB_OTG_HCCHAR_EPDIR_Pos |
			low_Speed_Device 	<< USB_OTG_HCCHAR_LSDEV_Pos |
			endpoint_Type 		<< USB_OTG_HCCHAR_EPTYP_Pos |
			multi_Count 		<< USB_OTG_HCCHAR_MC_Pos 	|
			device_Address 		<< USB_OTG_HCCHAR_DAD_Pos 	|
			odd_Frame 			<< USB_OTG_HCCHAR_ODDFRM_Pos
			);
	USB_Host_Channel -> HCCHAR = channel_Characteristics;
}

void USB_LL_Host___Channel_Setup_Buffer(uint8_t port_Number, uint8_t channel_Number, uint8_t *buffer_Pointer, uint32_t transfer_Size)
{
	USB_LL_Host___Channel_Set_Buffer_Pointer(port_Number, channel_Number, buffer_Pointer);
	USB_LL_Host___Channel_Set_Transfer_Size(port_Number, channel_Number, transfer_Size);
	USB_LL_Host___Channel_Reset_Transfer_Progress(port_Number, channel_Number);
}

void USB_LL_Host___Channel_Load_HCTSIZ(uint8_t port_Number, uint8_t channel_Number, uint32_t transfer_Size_In_Bytes, uint32_t packet_Count, uint8_t packet_ID)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel = USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_Host_Channel -> HCTSIZ = (transfer_Size_In_Bytes << USB_OTG_HCTSIZ_XFRSIZ_Pos | packet_Count << USB_OTG_HCTSIZ_PKTCNT_Pos | packet_ID << USB_OTG_HCTSIZ_DPID_Pos);
}

void USB_LL_Host___Transfer_Next_Packet(uint8_t port_Number, uint8_t channel_Number)
{
	uint32_t* FIFO_Pointer 				= USB_LL_Host___Channel_Get_Fifo_Pointer(port_Number, channel_Number);
	uint32_t  transfer_Size_Remaining 	= USB_LL_Host___Channel_Get_Transfer_Size(port_Number, channel_Number) - USB_LL_Host___Channel_Get_Transfer_Progress(port_Number, channel_Number);
	uint16_t  transfer_Size 			= USB_LL_Host___GET_MIN(transfer_Size_Remaining, USB_LL_Host___Channel_Get_Packet_Size(port_Number, channel_Number));
	uint8_t*  buffer_Pointer			= USB_LL_Host___Channel_Get_Buffer_Pointer(port_Number, channel_Number);

	USB_LL_Hardware___FIFO_Transfer_In(buffer_Pointer, FIFO_Pointer, transfer_Size);
}

void USB_LL_Host___Channel_Begin_Transfer_In(uint8_t port_Number, uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef *USB_Host_Channel = USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_Host_Channel->HCCHAR |= USB_OTG_HCCHAR_CHENA;
}


void USB_LL_Host___Channel_Begin_Transfer_Out(uint8_t port_Number, uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef *USB_Host_Channel = USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_Host_Channel->HCCHAR |= USB_OTG_HCCHAR_CHENA;

	USB_LL_Host___Transfer_Next_Packet(port_Number, channel_Number);
}

void USB_LL_Host___Channel_Out_Packet_Acknowledged(uint8_t port_Number, uint8_t channel_Number)
{
	uint16_t  packet_Size 				= USB_LL_Host___Channel_Get_Packet_Size(port_Number, channel_Number);
	uint16_t  size_Transfered 			= USB_LL_Host___GET_MIN(packet_Size, (USB_LL_Host___Channel_Get_Transfer_Size(port_Number, channel_Number) - USB_LL_Host___Channel_Get_Transfer_Progress(port_Number, channel_Number)));

	USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Progress += size_Transfered;

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

	USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].transfer_Progress += size_Transfered;

	uint16_t  transfer_Size_Remaining 	= USB_LL_Host___Channel_Get_Transfer_Size(port_Number, channel_Number) - USB_LL_Host___Channel_Get_Transfer_Progress(port_Number, channel_Number);

	if(transfer_Size_Remaining > 0)
	{
		USB_LL_Host___Channel_Begin_Transfer_In(port_Number, channel_Number);
	}
}

void USB_LL_Host___Channel_Set_Retry_After_Halt(uint8_t port_Number, uint8_t channel_Number, uint8_t value)
{
	USB_LL_Host___Host_Port[port_Number].channel_Buffer[channel_Number].retry_After_Halt = value;
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

