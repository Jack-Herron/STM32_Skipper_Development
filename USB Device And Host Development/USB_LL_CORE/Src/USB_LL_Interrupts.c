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
#include "../Inc/USB_LL_Host.h"
#include "../Inc/USB_LL_Interrupts.h"
#include "../Inc/USB_LL_Interrupts_Device.h"
#include "../Inc/USB_LL_Interrupts_Host.h"

void USB_LL_Interrupts___Start_Of_Frame_Reveived(uint8_t port_Number)
{
	USB_LL_Interrupts_Host___Start_Of_Frame_Interrupt_Received(port_Number);
}

void USB_LL_Interrupts___Interrupt_Handler(uint8_t port_Number)
{
	USB_OTG_GlobalTypeDef* USB = USB_LL_Hardware___Get_USB(port_Number);

	while((USB -> GINTSTS) & USB_LL_Interrupts___GLOBAL_INTERRUPTS_MASK)
	{
		switch(POSITION_VAL((USB -> GINTSTS) & USB_LL_Interrupts___GLOBAL_INTERRUPTS_MASK))
		{
		case USB_OTG_GINTSTS_SOF_Pos:											// SOF Received/sent Interrupt
			USB -> GINTSTS = (USB_OTG_GINTSTS_SOF);
			USB_LL_Interrupts___Start_Of_Frame_Reveived(port_Number);
			break;

		case USB_OTG_GINTSTS_RXFLVL_Pos:										// RXFIFO nonempty (data ready to be read)
			USB -> GINTMSK &= ~(USB_OTG_GINTMSK_RXFLVLM);
			if(USB_LL_Hardware___GET_BIT_SEGMENT(USB -> GINTSTS, USB_OTG_GINTSTS_CMOD_Msk, USB_OTG_GINTSTS_CMOD_Pos))
			{
				USB_LL_Interrupts_Host___Packet_Received(port_Number);
			}
			else
			{
				USB_LL_Interrupts_Device___Packet_Received(port_Number);
			}
			USB -> GINTMSK |= (USB_OTG_GINTMSK_RXFLVLM);
			break;

		case USB_OTG_GINTSTS_NPTXFE_Pos:										// non-periodic TX-FIFO empty / half empty (ready to be filled with new data)
			break;

		case USB_OTG_GINTSTS_GINAKEFF_Pos:										// Global IN non-periodic NAK effective Interrupt
			break;

		case USB_OTG_GINTSTS_BOUTNAKEFF_Pos:									// Global OUT NAK effective Interrupt
			break;

		case USB_OTG_GINTSTS_ESUSP_Pos:											// Early suspend (USB has been in idle state for 3ms)
			USB -> GINTSTS = (USB_OTG_GINTSTS_ESUSP);
			break;

		case USB_OTG_GINTSTS_USBSUSP_Pos:										// USB suspend (no data activity for 3ms)
			USB -> GINTSTS = (USB_OTG_GINTSTS_USBSUSP);
			break;

		case USB_OTG_GINTSTS_USBRST_Pos:										// USB RESET (reset detected on USB core)
			USB -> GINTSTS = (USB_OTG_GINTSTS_USBRST);
			break;

		case USB_OTG_GINTSTS_ENUMDNE_Pos:										// enumeration done (core knows the speed of the port)
			USB -> GINTSTS = (USB_OTG_GINTSTS_ENUMDNE);
			break;

		case USB_OTG_GINTSTS_ISOODRP_Pos:										// Isochronous OUT packet dropped interrupt
			USB -> GINTSTS = (USB_OTG_GINTSTS_ISOODRP);
			break;

		case USB_OTG_GINTSTS_EOPF_Pos:											// end of periodic frame (the period specified in the periodic frame interval field of the device configuration register has been reached in the current frame)
			USB -> GINTSTS = (USB_OTG_GINTSTS_EOPF);
			break;

		case USB_OTG_GINTSTS_IEPINT_Pos:										// IN End-point interrupt

			break;

		case USB_OTG_GINTSTS_OEPINT_Pos:										// OUT End-point interrupt

			break;

		case USB_OTG_GINTSTS_IISOIXFR_Pos:										// Incomplete isochronous IN transfer Interrupt
			USB -> GINTSTS = (USB_OTG_GINTSTS_IISOIXFR);
			break;

		case USB_OTG_GINTSTS_PXFR_INCOMPISOOUT_Pos:								// Incomplete periodic transfer
			USB -> GINTSTS = (USB_OTG_GINTSTS_PXFR_INCOMPISOOUT);
			break;

		case USB_OTG_GINTSTS_HPRTINT_Pos:										// Host Port Interrupt
			USB_LL_Interrupts_Host___Port_Interrupt_Handler(port_Number);
			break;

		case USB_OTG_GINTSTS_HCINT_Pos:											// Host Channel Interrupt
			USB_LL_Interrupts_Host___Channel_Interrupt_Handler(port_Number);
			break;

		case USB_OTG_GINTSTS_PTXFE_Pos:											// Periodic TxFIFO empty / half empty Interrupt
			break;

		case USB_OTG_GINTSTS_DISCINT_Pos:										// Disconnect detected interrupt
			USB -> GINTSTS = (USB_OTG_GINTSTS_DISCINT);
			break;

		case USB_OTG_GINTSTS_WKUINT_Pos:										// Resume/remote wake-up detected interrupt
			USB -> GINTSTS = (USB_OTG_GINTSTS_WKUINT);
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
