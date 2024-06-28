/*
 * USB_LL_Interrupts_Host.c
 *
 *  Created on: Jun 27, 2024
 *      Author: Jack Herron
 */

#include <stdint.h>					// Include C library for fixed-width integer types
#include <stm32f4xx.h>				// include MCU specific definitions
#include "../Inc/USB_LL_Definitions.h"
#include "../Inc/USB_LL_Hardware.h"
#include "../Inc/USB_LL_Interrupts.h"
#include "../Inc/USB_LL_Interrupts_Host.h"

static USB_LL_Interrupts_Host___Status_TypeDef host_Status[USB_LL_Definitions___NUMBER_OF_PORTS];

USB_LL_Interrupts_Host___Status_TypeDef* USB_LL_Interrupts_Host___Get_Host_Status(uint8_t port_Number)
{
	return(&host_Status[port_Number]);
}

void USB_LL_Interrupts_Host___Port_Interrupt_Handler(uint8_t port_Number)
{
	USB_OTG_HostPortTypeDef* USB_Host_Port = USB_LL_Hardware___Get_USB_Host_Port(port_Number);

	while((USB_Host_Port -> HPRT) & USB_LL_Interrupts_Host___PORT_INTERRUPTS_MASK)
	{
		switch(POSITION_VAL((USB_Host_Port -> HPRT) & USB_LL_Interrupts_Host___PORT_INTERRUPTS_MASK))
		{
		case(USB_OTG_HPRT_PCDET_Pos):
			USB_Host_Port -> HPRT |= (USB_OTG_HPRT_PCDET_Msk);
			//f_USB_Hardware___Reset_Host(port_Number);
			break;

		case(USB_OTG_HPRT_PENCHNG_Pos):
			USB_Host_Port -> HPRT = (USB_Host_Port -> HPRT & ~(USB_LL_Interrupts_Host___HPRT_RC_W1_BITS)) | (USB_OTG_HPRT_PENCHNG_Msk);
			if(USB_Host_Port -> HPRT & USB_OTG_HPRT_PCSTS)
			{
				//f_USB_Hardware___Host_Port_Device_Connect_Detected(port_Number);
			}
			else
			{
				//f_USB_Hardware___Host_Port_Device_Disconnect_Detected(port_Number);
			}
			break;
		}
	}
}

void USB_LL_Interrupts_Host___Channel_Interrupt_Handler(uint8_t port_Number){
	USB_OTG_HostTypeDef*		USB_Host 		= USB_LL_Hardware___Get_USB_Host(port_Number);
	uint8_t 					channel_Number 	= POSITION_VAL(USB_Host -> HAINT);
	USB_OTG_HostChannelTypeDef* USB_Host_Ch 	= USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	uint8_t 					device_Address 	= USB_LL_Hardware___GET_BIT_SEGMENT(USB_Host_Ch -> HCCHAR, USB_OTG_HCCHAR_DAD_Msk, USB_OTG_HCCHAR_DAD_Pos);

	while((USB_Host_Ch -> HCINT) & USB_LL_Interrupts_Host___CHANNEL_INTERRUPTS_MASK)
	{
		switch(POSITION_VAL((USB_Host_Ch -> HCINT) & USB_LL_Interrupts_Host___CHANNEL_INTERRUPTS_MASK))
		{
		case USB_OTG_HCINT_XFRC_Pos: 								// XFER Complete received
			break;

		case USB_OTG_HCINT_CHH_Pos: 								// channel halted
			break;

		case USB_OTG_HCINT_STALL_Pos: 								// channel Stall received
			break;

		case USB_OTG_HCINT_NAK_Pos: 								// NAK received
			break;

		case USB_OTG_HCINT_TXERR_Pos: 								// TX ERROR received
			break;

		case USB_OTG_HCINT_FRMOR_Pos: 								// Frame Error received
			break;
		}
	}
}
