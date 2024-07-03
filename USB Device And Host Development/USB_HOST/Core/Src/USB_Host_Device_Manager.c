/*
 * USB_Host_Device_Manager.c
 *
 *  Created on: Jun 28, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <stdbool.h>

#include "../Inc/USB_Host.h"
#include "../../Config/USB_Host_Config.h"
#include "../Inc/USB_Host_Device_Manager.h"

#if USB_Host_Config___DYNAMICALLY_ALLOCATE_DEVICES == false
	USB_Host_Device_Manager___Device_TypeDef device_Pool[USB_Host_Device_Manager___DEVICE_POOL_SIZE];
#endif

static USB_Host_Device_Manager___Port_TypeDef  	 USB_Host_Device_Manager___Port[USB_Host___NUMBER_OF_PORTS] = {0};

USB_Host_Device_Manager___Device_TypeDef* USB_Host_Device_Manager___Get_Free_Device_From_Pool()
{
	for(uint16_t i = 0; i < USB_Host_Device_Manager___DEVICE_POOL_SIZE; i++)
	{
		if(device_Pool[i].status.is_Connected == false)
		{
			return(&device_Pool[i]);
		}
	}
	return(NULL);
}

USB_Host_Device_Manager___Device_TypeDef* USB_Host_Device_Manager___Allocate_Device()
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = NULL;

	if(USB_Host_Config___DYNAMICALLY_ALLOCATE_DEVICES)
	{
		p_Device = (USB_Host_Device_Manager___Device_TypeDef*)malloc(sizeof(USB_Host_Device_Manager___Device_TypeDef));
	}
	else
	{
		p_Device = USB_Host_Device_Manager___Get_Free_Device_From_Pool();
	}

	return(p_Device);
}

int8_t USB_Host_Device_Manager___Allocate_Device_At_Address_Zero(uint8_t port_Number)
{
	if(USB_Host_Device_Manager___Port[port_Number].p_Device[0] == NULL)
	{
		USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Allocate_Device();
		if(p_Device != NULL)
		{
			USB_Host_Device_Manager___Port[port_Number].p_Device[0] = p_Device;

			return(EXIT_SUCCESS);
		}
		return(EXIT_FAILURE);
	}
	return(EXIT_FAILURE);
}

USB_Host_Device_Manager___Port_Status_TypeDef USB_Host_Device_Manager___Get_Port_Status(uint8_t port_Number)
{
	return(USB_Host_Device_Manager___Port[port_Number].port_Status);
}
