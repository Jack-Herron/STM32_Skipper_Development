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
#include "../Inc/USB_LL_Interrupts_Host.h"
#include "../Inc/USB_LL_Interrupts_Device.h"
static USB_LL_Interrupts___Status_TypeDef port_Status[USB_LL_Definitions___NUMBER_OF_PORTS];

USB_LL_Interrupts___Status_TypeDef* USB_LL_Interrupts___Get_Status(uint8_t port_Number)
{
	return(&port_Status[port_Number]);
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
			USB_LL_Interrupts_Host___Port_Interrupt_Handler(port_Number);
			break;

		case USB_OTG_GINTSTS_HCINT_Pos:											// Host Channel Interrupt
			USB_LL_Interrupts_Host___Channel_Interrupt_Handler(port_Number);
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
