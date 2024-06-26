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
#include "../Inc/USB_LL_Interrupts.h"
#include "../Inc/USB_LL_Hardware.h"

#if (Skipper_Clock___48Mhz_FREQUENCY != 48000000)
	#error("USB requires 48Mhz Clock to be running at exactly 48000000. Reconfigure the clock parameters in Skipper_Clock.h")
#endif

// ------------------- FETCH USB REGISTER STRUCTURE FUNCTIONS ------------------------

uint32_t USB_LL_Hardware___Get_USB_BASE(uint8_t port_number)
{
	if (port_number == USB_LL_Hardware___PORT_0)
	{
	    return (USB_OTG_FS_PERIPH_BASE);
	}
	else if (port_number == USB_LL_Hardware___PORT_1)
	{
	    return (USB_OTG_HS_PERIPH_BASE);
	}
	else
	{
		return(0);
	}
}

USB_OTG_GlobalTypeDef* USB_LL_Hardware___Get_USB(uint8_t port_Number)
{
	return((USB_OTG_GlobalTypeDef *) USB_LL_Hardware___Get_USB_BASE(port_Number));
}

USB_OTG_DeviceTypeDef* USB_LL_Hardware___Get_USB_Device(uint8_t port_Number)
{
	return((USB_OTG_DeviceTypeDef *) (USB_LL_Hardware___Get_USB_BASE(port_Number) + USB_OTG_DEVICE_BASE));
}

USB_OTG_HostTypeDef* USB_LL_Hardware___Get_USB_Host(uint8_t port_Number)
{
	return((USB_OTG_HostTypeDef *) (USB_LL_Hardware___Get_USB_BASE(port_Number) + USB_OTG_HOST_BASE));
}

USB_OTG_HostPortTypeDef* USB_LL_Hardware___Get_USB_Host_Port(uint8_t port_Number)
{
	return((USB_OTG_HostPortTypeDef *) (USB_LL_Hardware___Get_USB_BASE(port_Number) + USB_OTG_HOST_PORT_BASE));
}

USB_OTG_HostChannelTypeDef* USB_LL_Hardware___Get_USB_Host_Channel(uint8_t port_Number, uint8_t channel_Number)
{
	return((USB_OTG_HostChannelTypeDef *) (USB_LL_Hardware___Get_USB_BASE(port_Number) + USB_OTG_HOST_CHANNEL_BASE + (channel_Number * USB_OTG_HOST_CHANNEL_SIZE)));
}

USB_OTG_INEndpointTypeDef* USB_LL_Hardware___Get_USB_Device_IN(uint8_t port_Number, uint8_t endpoint_Number)
{
	return((USB_OTG_INEndpointTypeDef *) (USB_LL_Hardware___Get_USB_BASE(port_Number) + USB_OTG_IN_ENDPOINT_BASE + (endpoint_Number * USB_OTG_EP_REG_SIZE)));
}

USB_OTG_OUTEndpointTypeDef* USB_LL_Hardware___Get_USB_Device_OUT(uint8_t port_Number, uint8_t endpoint_Number)
{
	return((USB_OTG_OUTEndpointTypeDef *) (USB_LL_Hardware___Get_USB_BASE(port_Number) + USB_OTG_OUT_ENDPOINT_BASE + (endpoint_Number * USB_OTG_EP_REG_SIZE)));
}

// -----------------------------------------------------------------------------------

// ---------------------------- USB INIT FUNCTIONS -----------------------------------

void USB_LL_Hardware___GPIO_Init(uint8_t port_Number)
{
	if(port_Number == USB_LL_Hardware___PORT_0)
	{
		RCC -> AHB1ENR 	|= (RCC_AHB1ENR_GPIOAEN);
		GPIOA -> MODER 	|= (USB_LL_Hardware___GPIO_ALTERNATE_MODE 	<< GPIO_MODER_MODE11_Pos);
		GPIOA -> MODER 	|= (USB_LL_Hardware___GPIO_ALTERNATE_MODE 	<< GPIO_MODER_MODE12_Pos);
		GPIOA -> AFR[1] |= (USB_LL_Hardware___GPIO_Alt_FUNCTION_10 	<< GPIO_AFRH_AFSEL11_Pos);
		GPIOA -> AFR[1] |= (USB_LL_Hardware___GPIO_Alt_FUNCTION_10 	<< GPIO_AFRH_AFSEL12_Pos);
	}
	else if(port_Number == USB_LL_Hardware___PORT_1)
	{
		RCC -> AHB1ENR 	|= (RCC_AHB1ENR_GPIOBEN);
		GPIOB -> MODER 	|= (USB_LL_Hardware___GPIO_ALTERNATE_MODE 	<< GPIO_MODER_MODE14_Pos);
		GPIOB -> MODER 	|= (USB_LL_Hardware___GPIO_ALTERNATE_MODE 	<< GPIO_MODER_MODE15_Pos);
		GPIOB -> AFR[1] |= (USB_LL_Hardware___GPIO_Alt_FUNCTION_12 	<< GPIO_AFRH_AFSEL14_Pos);
		GPIOB -> AFR[1] |= (USB_LL_Hardware___GPIO_Alt_FUNCTION_12 	<< GPIO_AFRH_AFSEL15_Pos);
	}
}

void USB_LL_Hardware___Init(uint8_t port_Number, uint8_t port_Mode)
{
	USB_OTG_GlobalTypeDef* USB = USB_LL_Hardware___Get_USB(port_Number);

	if(port_Number == USB_LL_Hardware___PORT_0)
	{
		RCC->AHB2ENR |= (RCC_AHB2ENR_OTGFSEN);
		NVIC_SetPriority (OTG_FS_IRQn, 1);
		NVIC_EnableIRQ (OTG_FS_IRQn);
	}
	else if(port_Number == USB_LL_Hardware___PORT_1)
	{
		RCC->AHB1ENR |= (RCC_AHB1ENR_OTGHSEN);
		NVIC_SetPriority (OTG_HS_IRQn, 1);
		NVIC_EnableIRQ (OTG_HS_IRQn);
	}

	Skipper_Clock___Delay_ms(USB_LL_Hardware___USB_ENABLE_DELAY);

	USB -> GAHBCFG &= ~(USB_OTG_GAHBCFG_DMAEN);
	USB -> GUSBCFG &= ~(USB_OTG_GUSBCFG_HNPCAP);
	USB -> GUSBCFG &= ~(USB_OTG_GUSBCFG_SRPCAP);
	USB -> GUSBCFG &= ~(USB_OTG_GUSBCFG_TRDT_Msk);
	USB -> GUSBCFG |=  (USB_LL_Hardware___TRDT_Value << USB_OTG_GUSBCFG_TRDT_Pos);
	USB -> GUSBCFG |=  (USB_OTG_GUSBCFG_PHYSEL);

	if(port_Mode == USB_LL_Hardware___HOST_MODE)
	{
		USB_OTG_HostPortTypeDef* 	USB_Host_Port 	= USB_LL_Hardware___Get_USB_Host_Port(port_Number);
		USB_OTG_HostTypeDef* 		USB_Host 		= USB_LL_Hardware___Get_USB_Host(port_Number);

		USB -> 				GUSBCFG 	|= 	(USB_OTG_GUSBCFG_FHMOD);

		Skipper_Clock___Delay_ms(USB_LL_Hardware___CHANGE_MODE_DELAY);

		USB_Host -> 		HCFG 		|= 	(USB_LL_Hardware___HCFG_SELECT_48Mhz_PHY_FREQUENCY 	<<	USB_OTG_HCFG_FSLSPCS_Pos);
		USB_Host_Port -> 	HPRT 		|= 	(USB_LL_Hardware___HPRT_Port_Power_Enabled 			<< 	USB_OTG_HPRT_PPWR_Pos);
	}
	else if(port_Mode == USB_LL_Hardware___DEVICE_MODE)
	{
		USB_OTG_DeviceTypeDef* USB_Device = USB_LL_Hardware___Get_USB_Device(port_Number);

		USB_Device 	-> 		DIEPMSK		= 	USB_LL_Interrupts___IN_ENDPOINT_INTERRUPTS_MASK;
		USB_Device 	-> 		DOEPMSK 	= 	USB_LL_Interrupts___OUT_ENDPOINT_INTERRUPTS_MASK;
		USB 	   	-> 		GUSBCFG		|= 	USB_OTG_GUSBCFG_FDMOD;
		USB 		-> 		GCCFG 		|= 	USB_OTG_GCCFG_NOVBUSSENS;

		Skipper_Clock___Delay_ms(USB_LL_Hardware___CHANGE_MODE_DELAY);

		USB_Device 	-> 		DCFG 		|= 	 (USB_LL_Hardware___DCFG_SELECT_48Mhz_PHY_FREQUENCY << USB_OTG_DCFG_DSPD_Pos);
		USB_Device 	-> 		DCFG 		&= 	~(USB_OTG_DCFG_NZLSOHSK);
	}

	USB -> GINTMSK 	 = (USB_LL_Interrupts___GLOBAL_INTERRUPTS_MASK);
	USB -> GCCFG 	|= (USB_OTG_GCCFG_PWRDWN);
	USB -> GAHBCFG 	|= (USB_OTG_GAHBCFG_GINT);
}

// -----------------------------------------------------------------------------------


