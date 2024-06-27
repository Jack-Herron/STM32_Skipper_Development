/*
 * USB_LL_Interrupts.cpp
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#include <stdint.h>					// Include C library for fixed-width integer types
#include <stm32f4xx.h>				// include MCU specific definitions
#include "../Inc/USB_LL_Definitions.h"
#include "../Inc/USB_LL_Hardware.h"
#include "../Inc/USB_LL_Interrupts.h"

static USB_LL_Interrupts___Status_TypeDef port_Status[USB_LL_Definitions___NUMBER_OF_PORTS];

USB_LL_Interrupts___Status_TypeDef* USB_LL_Interrupts___Get_Status(uint8_t port_Number)
{
	return(&port_Status[port_Number]);
}

void USB_LL_Interrupts___Host_Port_Interrupt_Handler(uint8_t port_Number)
{
	USB_OTG_HostPortTypeDef* USB_Host_Port = USB_LL_Hardware___Get_USB_Host_Port(port_Number);

	while((USB_Host_Port -> HPRT) & USB_LL_Interrupts___HOST_PORT_INTERRUPTS_MASK)
	{
		switch(POSITION_VAL((USB_Host_Port -> HPRT) & USB_LL_Interrupts___HOST_PORT_INTERRUPTS_MASK))
		{
		case(USB_OTG_HPRT_PCDET_Pos):
			USB_Host_Port -> HPRT |= (USB_OTG_HPRT_PCDET_Msk);
			//f_USB_Hardware___Reset_Host(port_Number);
			break;

		case(USB_OTG_HPRT_PENCHNG_Pos):
			USB_Host_Port -> HPRT = (USB_Host_Port -> HPRT & ~(USB_LL_Interrupts___HPRT_RC_W1_BITS)) | (USB_OTG_HPRT_PENCHNG_Msk);
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

void f_USB_Hardware___Host_Channel_Interrupt_Handler(uint8_t port_Number){
	USB_OTG_HostTypeDef*		USB_Host 		= USB_LL_Hardware___Get_USB_Host(port_Number);
	uint8_t 					channel_Number 	= POSITION_VAL(USB_Host -> HAINT);
	USB_OTG_HostChannelTypeDef* USB_Host_Ch 	= USB_LL_Hardware___Get_USB_Host_Channel(port_Number, channel_Number);
	uint8_t 					device_Address 	= USB_LL_Hardware___GET_BIT_SEGMENT(USB_Host_Ch -> HCCHAR, USB_OTG_HCCHAR_DAD_Msk, USB_OTG_HCCHAR_DAD_Pos);

	while((USB_Host_Ch -> HCINT) & USB_LL_Interrupts___CHANNEL_INTERRUPTS_MASK)
	{
		switch(POSITION_VAL((USB_Host_Ch -> HCINT) & USB_LL_Interrupts___CHANNEL_INTERRUPTS_MASK))
		{
		case USB_OTG_HCINT_XFRC_Pos: 								// XFER Complete received
			break;

		case USB_OTG_HCINT_CHH_Pos: 								// channel halted
			break;

		case USB_OTG_HCINT_STALL_Pos: 								// channel Stall received
			break;

		case USB_OTG_HCINT_NAK_Pos: 								// NAK received
			break;

		case USB_OTG_HCINT_TXERR_Pos: 								// NAK received
			break;

		case USB_OTG_HCINT_FRMOR_Pos: 								// NAK received
			break;
		}
	}
}

void USB_LL_Interrupts___Interrupt_Handler(uint8_t port_Number)
{
	USB_OTG_GlobalTypeDef* USB = USB_LL_Hardware___Get_USB(port_Number);

	while((USB -> GINTSTS) & USB_LL_Interrupts___GLOBAL_INTERRUPTS_MASK)
	{
		switch(POSITION_VAL((USB -> GINTSTS) & USB_LL_Interrupts___GLOBAL_INTERRUPTS_MASK))
		{
		case USB_OTG_GINTSTS_SOF_Pos:											// SOF Received/sent Interrupt
			break;

		case USB_OTG_GINTSTS_RXFLVL_Pos:										// RXFIFO nonempty (data ready to be read)

			break;

		case USB_OTG_GINTSTS_NPTXFE_Pos:										// non-periodic TX-FIFO empty / half empty (ready to be filled with new data)
			break;

		case USB_OTG_GINTSTS_GINAKEFF_Pos:										// Global IN non-periodic NAK effective Interrupt
			break;

		case USB_OTG_GINTSTS_BOUTNAKEFF_Pos:									// Global OUT NAK effective Interrupt
			break;

		case USB_OTG_GINTSTS_ESUSP_Pos:											// Early suspend (USB has been in idle state for 3ms)

			break;

		case USB_OTG_GINTSTS_USBSUSP_Pos:										// USB suspend (no data activity for 3ms)

			break;

		case USB_OTG_GINTSTS_USBRST_Pos:										// USB RESET (reset detected on USB core)

			break;

		case USB_OTG_GINTSTS_ENUMDNE_Pos:										// enumeration done (core knows the speed of the port)

			break;

		case USB_OTG_GINTSTS_ISOODRP_Pos:										// Isochronous OUT packet dropped interrupt

			break;

		case USB_OTG_GINTSTS_EOPF_Pos:											// end of periodic frame (the period specified in the periodic frame interval field of the device configuration register has been reached in the current frame)

			break;

		case USB_OTG_GINTSTS_IEPINT_Pos:										// IN End-point interrupt

			break;

		case USB_OTG_GINTSTS_OEPINT_Pos:										// OUT End-point interrupt

			break;

		case USB_OTG_GINTSTS_IISOIXFR_Pos:										// Incomplete isochronous IN transfer Interrupt

			break;

		case USB_OTG_GINTSTS_PXFR_INCOMPISOOUT_Pos:								// Incomplete periodic transfer

			break;

		case USB_OTG_GINTSTS_HPRTINT_Pos:										// Host Port Interrupt

			break;

		case USB_OTG_GINTSTS_HCINT_Pos:											// Host Channel Interrupt

			break;

		case USB_OTG_GINTSTS_PTXFE_Pos:											// Periodic TxFIFO empty / half empty Interrupt
			break;

		case USB_OTG_GINTSTS_DISCINT_Pos:										// Disconnect detected interrupt

			break;

		case USB_OTG_GINTSTS_WKUINT_Pos:										// Resume/remote wake-up detected interrupt

			break;
		}
	}
}

void OTG_FS_IRQHandler()
{
	USB_LL_Interrupts___Interrupt_Handler(USB_LL_Hardware___PORT_0);
}

void OTG_HS_IRQHandler()
{
	USB_LL_Interrupts___Interrupt_Handler(USB_LL_Hardware___PORT_1);
}
