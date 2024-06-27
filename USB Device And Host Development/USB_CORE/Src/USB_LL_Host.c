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
#include "../Inc/USB_LL_Definitions.h"
#include "../Inc/USB_LL_Interrupts.h"
#include "../Inc/USB_LL_Hardware.h"
#include "../Inc/USB_LL_Host.h"

// ---------------------------- USB HOST CONTROL SECTION -----------------------------

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

void USB_LL_Host___Channel_Halt_And_Wait(uint8_t port_Number, uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel = USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_Host_Channel -> HCCHAR |= (USB_OTG_HCCHAR_CHDIS);
	while(USB_Host_Channel -> HCCHAR & USB_OTG_HCCHAR_CHENA);
}

uint8_t USB_LL_Host___Channel_Get_Current_PID(uint8_t port_Number, uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel = USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	return(USB_LL_Hardware___GET_BIT_SEGMENT(USB_Host_Channel -> HCTSIZ, USB_OTG_HCTSIZ_DPID_Msk, USB_OTG_HCTSIZ_DPID_Pos));
}

void USB_LL_Host___Channel_Set_Interrupts(uint8_t port_Number,uint8_t channel_Number)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel 	= USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_OTG_HostTypeDef*		USB_Host 			= USB_LL_Hardware___Get_USB_Host(port_Number);
	USB_Host 			-> HAINTMSK |= (1 << channel_Number);
	USB_Host_Channel 	-> HCINTMSK  = USB_LL_Interrupts___CHANNEL_INTERRUPTS_MASK;
}

void USB_LL_Host___Channel_Set_Characteristics(uint8_t port_Number, uint8_t channel_Number, uint32_t channel_Characteristics)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel = USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_Host_Channel -> HCCHAR = channel_Characteristics;
}

void USB_LL_Host___Channel_Load_HCTSIZ(uint8_t port_Number, uint8_t channel_Number, uint32_t transfer_Size_In_Bytes, uint32_t packet_Count, uint8_t packet_ID)
{
	USB_OTG_HostChannelTypeDef* USB_Host_Channel = USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	USB_Host_Channel -> HCTSIZ = (transfer_Size_In_Bytes << USB_OTG_HCTSIZ_XFRSIZ_Pos | packet_Count << USB_OTG_HCTSIZ_PKTCNT_Pos | packet_ID << USB_OTG_HCTSIZ_DPID_Pos);
}

// -----------------------------------------------------------------------------------

