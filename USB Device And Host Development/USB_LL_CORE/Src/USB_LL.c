/*
 * USB_LL_Hardware.cpp
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>

#include "../Inc/USB_LL_Definitions.h"
#include "../Inc/USB_LL.h"
#include "../Inc/USB_LL_Device.h"
#include "../Inc/USB_LL_Host.h"

#if (Skipper_Clock___48Mhz_FREQUENCY != 48000000)
	#error("USB requires 48Mhz Clock to be running at exactly 48000000 Hz. Reconfigure the clock parameters in Skipper_Clock.h")
#endif

// ------------------- FETCH USB REGISTER STRUCTURE FUNCTIONS ------------------------

uint32_t USB_LL___Get_USB_BASE(uint8_t port_number)
{
	if (port_number == USB_LL___PORT_0)
	{
	    return (USB_OTG_FS_PERIPH_BASE);
	}
	else if (port_number == USB_LL___PORT_1)
	{
	    return (USB_OTG_HS_PERIPH_BASE);
	}
	else
	{
		return(0);
	}
}

USB_OTG_GlobalTypeDef* USB_LL___Get_USB(uint8_t port_Number)
{
	return((USB_OTG_GlobalTypeDef *) USB_LL___Get_USB_BASE(port_Number));
}

USB_OTG_DeviceTypeDef* USB_LL___Get_USB_Device(uint8_t port_Number)
{
	return((USB_OTG_DeviceTypeDef *) (USB_LL___Get_USB_BASE(port_Number) + USB_OTG_DEVICE_BASE));
}

USB_OTG_HostTypeDef* USB_LL___Get_USB_Host(uint8_t port_Number)
{
	return((USB_OTG_HostTypeDef *) (USB_LL___Get_USB_BASE(port_Number) + USB_OTG_HOST_BASE));
}

USB_OTG_HostPortTypeDef* USB_LL___Get_USB_Host_Port(uint8_t port_Number)
{
	return((USB_OTG_HostPortTypeDef *) (USB_LL___Get_USB_BASE(port_Number) + USB_OTG_HOST_PORT_BASE));
}

USB_OTG_HostChannelTypeDef* USB_LL___Get_USB_Host_Channel(uint8_t port_Number, uint8_t channel_Number)
{
	return((USB_OTG_HostChannelTypeDef *) (USB_LL___Get_USB_BASE(port_Number) + USB_OTG_HOST_CHANNEL_BASE + (channel_Number * USB_OTG_HOST_CHANNEL_SIZE)));
}

USB_OTG_INEndpointTypeDef* USB_LL___Get_USB_Device_IN(uint8_t port_Number, uint8_t endpoint_Number)
{
	return((USB_OTG_INEndpointTypeDef *) (USB_LL___Get_USB_BASE(port_Number) + USB_OTG_IN_ENDPOINT_BASE + (endpoint_Number * USB_OTG_EP_REG_SIZE)));
}

USB_OTG_OUTEndpointTypeDef* USB_LL___Get_USB_Device_OUT(uint8_t port_Number, uint8_t endpoint_Number)
{
	return((USB_OTG_OUTEndpointTypeDef *) (USB_LL___Get_USB_BASE(port_Number) + USB_OTG_OUT_ENDPOINT_BASE + (endpoint_Number * USB_OTG_EP_REG_SIZE)));
}

// -----------------------------------------------------------------------------------

// ---------------------------- USB INIT FUNCTIONS -----------------------------------

void USB_LL___GPIO_Init(uint8_t port_Number)
{
	if(port_Number == USB_LL___PORT_0)
	{
		RCC -> AHB1ENR 	|= (RCC_AHB1ENR_GPIOAEN);
		GPIOA -> MODER 	|= (USB_LL___GPIO_ALTERNATE_MODE 	<< GPIO_MODER_MODE11_Pos);
		GPIOA -> MODER 	|= (USB_LL___GPIO_ALTERNATE_MODE 	<< GPIO_MODER_MODE12_Pos);
		GPIOA -> AFR[1] |= (USB_LL___GPIO_Alt_FUNCTION_10 	<< GPIO_AFRH_AFSEL11_Pos);
		GPIOA -> AFR[1] |= (USB_LL___GPIO_Alt_FUNCTION_10 	<< GPIO_AFRH_AFSEL12_Pos);
	}
	else if(port_Number == USB_LL___PORT_1)
	{
		RCC -> AHB1ENR 	|= (RCC_AHB1ENR_GPIOBEN);
		GPIOB -> MODER 	|= (USB_LL___GPIO_ALTERNATE_MODE 	<< GPIO_MODER_MODE14_Pos);
		GPIOB -> MODER 	|= (USB_LL___GPIO_ALTERNATE_MODE 	<< GPIO_MODER_MODE15_Pos);
		GPIOB -> AFR[1] |= (USB_LL___GPIO_Alt_FUNCTION_12 	<< GPIO_AFRH_AFSEL14_Pos);
		GPIOB -> AFR[1] |= (USB_LL___GPIO_Alt_FUNCTION_12 	<< GPIO_AFRH_AFSEL15_Pos);
	}
}

void USB_LL___Disable_IRQ(uint8_t port_Number)
{
	if (port_Number == USB_LL___PORT_0)
	{
		NVIC_DisableIRQ(OTG_FS_IRQn);
	}
	else if (port_Number == USB_LL___PORT_1)
	{
		NVIC_DisableIRQ(OTG_HS_IRQn);
	}
}

void USB_LL___Enable_IRQ(uint8_t port_Number)
{
	if (port_Number == USB_LL___PORT_0)
	{
		NVIC_EnableIRQ(OTG_FS_IRQn);
	}
	else if (port_Number == USB_LL___PORT_1)
	{
		NVIC_EnableIRQ(OTG_HS_IRQn);
	}
}

void USB_LL___Init(uint8_t port_Number, uint8_t port_Mode)
{
	USB_OTG_GlobalTypeDef* USB = USB_LL___Get_USB(port_Number);

	if(port_Number == USB_LL___PORT_0)
	{
		RCC->AHB2ENR |= (RCC_AHB2ENR_OTGFSEN);
		NVIC_SetPriority (OTG_FS_IRQn, 1);
		NVIC_EnableIRQ (OTG_FS_IRQn);
	}
	else if(port_Number == USB_LL___PORT_1)
	{
		RCC->AHB1ENR |= (RCC_AHB1ENR_OTGHSEN);
		NVIC_SetPriority (OTG_HS_IRQn, 1);
		NVIC_EnableIRQ (OTG_HS_IRQn);
	}

	Skipper_Clock___Delay_ms(USB_LL___USB_ENABLE_DELAY);

	USB -> GAHBCFG &= ~(USB_OTG_GAHBCFG_DMAEN);
	USB -> GUSBCFG &= ~(USB_OTG_GUSBCFG_HNPCAP);
	USB -> GUSBCFG &= ~(USB_OTG_GUSBCFG_SRPCAP);
	USB -> GUSBCFG &= ~(USB_OTG_GUSBCFG_TRDT_Msk);
	USB -> GUSBCFG |=  (USB_LL___TRDT_Value << USB_OTG_GUSBCFG_TRDT_Pos);
	USB -> GUSBCFG |=  (USB_OTG_GUSBCFG_PHYSEL);

	if(port_Mode == USB_LL___HOST_MODE)
	{
		USB_OTG_HostPortTypeDef* 	USB_Host_Port 	= USB_LL___Get_USB_Host_Port(port_Number);
		USB_OTG_HostTypeDef* 		USB_Host 		= USB_LL___Get_USB_Host(port_Number);

		USB -> 				GUSBCFG 	|= 	(USB_OTG_GUSBCFG_FHMOD);

		Skipper_Clock___Delay_ms(USB_LL___CHANGE_MODE_DELAY);

		USB_Host -> 		HCFG 		|= 	(USB_LL___HCFG_SELECT_48Mhz_PHY_FREQUENCY 	<<	USB_OTG_HCFG_FSLSPCS_Pos);
		USB_Host_Port -> 	HPRT 		|= 	(USB_LL___HPRT_Port_Power_Enabled 			<< 	USB_OTG_HPRT_PPWR_Pos);
	}
	else if(port_Mode == USB_LL___DEVICE_MODE)
	{
		USB_OTG_DeviceTypeDef* USB_Device = USB_LL___Get_USB_Device(port_Number);

		USB 	   	-> 		GUSBCFG		|= 	USB_OTG_GUSBCFG_FDMOD;
		USB 		-> 		GCCFG 		|= 	USB_OTG_GCCFG_NOVBUSSENS;

		Skipper_Clock___Delay_ms(USB_LL___CHANGE_MODE_DELAY);

		USB_Device 	-> 		DCFG 		|= 	 (USB_LL___DCFG_SELECT_48Mhz_PHY_FREQUENCY << USB_OTG_DCFG_DSPD_Pos);
		USB_Device 	-> 		DCFG 		&= 	~(USB_OTG_DCFG_NZLSOHSK);
	}

	USB -> GINTMSK 	 = (USB_LL___GLOBAL_INTERRUPTS_MASK);
	USB -> GCCFG 	|= (USB_OTG_GCCFG_PWRDWN);
	USB -> GAHBCFG 	|= (USB_OTG_GAHBCFG_GINT);
}

// -----------------------------------------------------------------------------------

void USB_LL___FIFO_Transfer_In(uint8_t* Source, uint32_t* Destination, uint32_t transfer_Size)
{
	uint32_t 	full_Copies 	= 	transfer_Size / USB_LL___NUMBER_OF_BYTES_IN_WORD_32;
	uint8_t 	remainder 		= 	transfer_Size % USB_LL___NUMBER_OF_BYTES_IN_WORD_32;

	for(uint16_t i = 0; i < full_Copies; i++)
	{
		Destination[i] = ((uint32_t*)Source)[i];
	}

	if(remainder > 0)
	{
		uint32_t final32 = 0;
		for(uint8_t i = 0; i < remainder; i++)
		{
			final32 |= Source[transfer_Size - (remainder - i)] << (i * USB_LL___NUMBER_OF_BITS_IN_BYTE);
		}
		Destination[full_Copies] = final32;
	}
}

void USB_LL___FIFO_Transfer_Out(uint32_t* Source, uint8_t* Destination, uint32_t transfer_Size)
{
	uint32_t 	full_Copies 	= 	transfer_Size / USB_LL___NUMBER_OF_BYTES_IN_WORD_32;
	uint8_t 	remainder 		= 	transfer_Size % USB_LL___NUMBER_OF_BYTES_IN_WORD_32;

	for(uint32_t i = 0; i < full_Copies; i++)
	{
		((uint32_t*)Destination)[i] = Source[i];
	}

	if (remainder > 0)
	{
		uint32_t final32 = Source[full_Copies];

		for (uint8_t i = 0; i < remainder; i++)
		{
			Destination[transfer_Size - (remainder - i)] = (final32 >> (i * USB_LL___NUMBER_OF_BITS_IN_BYTE)) & 0xff;
		}
	}
}

//-----------------------------------------------------------------------

void USB_LL___Start_Of_Frame_Reveived(uint8_t port_Number)
{
	USB_LL_Host___Start_Of_Frame_Interrupt_Received(port_Number);
}

void USB_LL___Interrupt_Handler(uint8_t port_Number)
{
	USB_OTG_GlobalTypeDef* USB = USB_LL___Get_USB(port_Number);

	while((USB -> GINTSTS) & USB_LL___GLOBAL_INTERRUPTS_MASK)
	{
		switch(POSITION_VAL((USB -> GINTSTS) & USB_LL___GLOBAL_INTERRUPTS_MASK))
		{
		case USB_OTG_GINTSTS_SOF_Pos:											// SOF Received/sent Interrupt
			USB -> GINTSTS = (USB_OTG_GINTSTS_SOF);
			USB_LL___Start_Of_Frame_Reveived(port_Number);
			break;

		case USB_OTG_GINTSTS_RXFLVL_Pos:										// RXFIFO nonempty (data ready to be read)
			USB -> GINTMSK &= ~(USB_OTG_GINTMSK_RXFLVLM);
			if(USB_LL___GET_BIT_SEGMENT(USB -> GINTSTS, USB_OTG_GINTSTS_CMOD_Msk, USB_OTG_GINTSTS_CMOD_Pos))
			{
				USB_LL_Host___Packet_Received(port_Number);
			}
			else
			{
				USB_LL_Device___Packet_Received(port_Number);
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
			USB_LL_Device___USB_Suspend(port_Number);							// Host suspend (device mode)
			break;

		case USB_OTG_GINTSTS_USBRST_Pos:										// USB RESET (reset detected on USB core)
			USB -> GINTSTS = (USB_OTG_GINTSTS_USBRST);
			break;

		case USB_OTG_GINTSTS_ENUMDNE_Pos:										// enumeration done (core knows the speed of the port)
			USB -> GINTSTS = (USB_OTG_GINTSTS_ENUMDNE);
			USB_LL_Device___Host_Enumerated(port_Number);						// Host connected (device mode)
			break;

		case USB_OTG_GINTSTS_ISOODRP_Pos:										// Isochronous OUT packet dropped interrupt
			USB -> GINTSTS = (USB_OTG_GINTSTS_ISOODRP);
			break;

		case USB_OTG_GINTSTS_EOPF_Pos:											// end of periodic frame (the period specified in the periodic frame interval field of the device configuration register has been reached in the current frame)
			USB -> GINTSTS = (USB_OTG_GINTSTS_EOPF);
			break;

		case USB_OTG_GINTSTS_IEPINT_Pos:										// IN End-point interrupt
			USB_LL_Device___IN_Endpoint_Interrupt_Handler(port_Number);
			break;

		case USB_OTG_GINTSTS_OEPINT_Pos:										// OUT End-point interrupt
			USB_LL_Device___OUT_Endpoint_Interrupt_Handler(port_Number);
			break;

		case USB_OTG_GINTSTS_IISOIXFR_Pos:										// Incomplete isochronous IN transfer Interrupt
			USB -> GINTSTS = (USB_OTG_GINTSTS_IISOIXFR);
			break;

		case USB_OTG_GINTSTS_PXFR_INCOMPISOOUT_Pos:								// Incomplete periodic transfer
			//printf("This is bad\n");
			USB -> GINTSTS = (USB_OTG_GINTSTS_PXFR_INCOMPISOOUT);
			break;

		case USB_OTG_GINTSTS_HPRTINT_Pos:										// Host Port Interrupt
			USB_LL_Host___Port_Interrupt_Handler(port_Number);
			break;

		case USB_OTG_GINTSTS_HCINT_Pos:											// Host Channel Interrupt
			USB_LL_Host___Channel_Interrupt_Handler(port_Number);
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
	USB_LL___Interrupt_Handler(USB_LL___PORT_0);
}

void OTG_HS_IRQHandler()
{
	USB_LL___Interrupt_Handler(USB_LL___PORT_1);
}
