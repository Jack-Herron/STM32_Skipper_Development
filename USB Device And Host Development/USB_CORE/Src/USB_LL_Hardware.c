/*
 * USB_LL_Hardware.cpp
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include "../Inc/USB_LL_Interrupts.h"
#include "../Inc/USB_LL_Hardware.h"

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
