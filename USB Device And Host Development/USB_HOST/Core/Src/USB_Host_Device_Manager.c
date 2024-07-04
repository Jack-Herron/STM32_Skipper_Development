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

static USB_Host_Device_Manager___Port_TypeDef  	 USB_Host_Device_Manager___Port[USB_Host___NUMBER_OF_PORTS] = {0};

#if USB_Host_Config___DYNAMICALLY_ALLOCATE_DEVICES == false

	USB_Host_Device_Manager___Device_TypeDef device_Pool[USB_Host_Device_Manager___DEVICE_POOL_SIZE];

	USB_Host_Device_Manager___Device_TypeDef* USB_Host_Device_Manager___Allocate_Device()
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

#else

USB_Host_Device_Manager___Device_TypeDef* USB_Host_Device_Manager___Allocate_Device()
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = NULL;

	p_Device = (USB_Host_Device_Manager___Device_TypeDef*)malloc(sizeof(USB_Host_Device_Manager___Device_TypeDef));

	return(p_Device);
}

#endif

void USB_Host_Device_Manager___Set_Device_Is_Root_Device(USB_Host_Device_Manager___Device_TypeDef* p_Device, uint8_t is_Root_Device)
{
	p_Device -> status.is_Root_Device = is_Root_Device;
}


void USB_Host_Device_Manager___Set_Device_Port_Number(USB_Host_Device_Manager___Device_TypeDef* p_Device, uint8_t port_Number)
{
	p_Device -> status.port_Number = port_Number;
}

void USB_Host_Device_Manager___Set_Device_Speed(USB_Host_Device_Manager___Device_TypeDef* p_Device, uint8_t device_Speed)
{
	if(device_Speed == USB_Host_Device_Manager___LOW_SPEED_DEVICE)
	{
		p_Device -> status.is_Low_Speed_Device = 1;
		p_Device -> status.is_High_Speed_Device = 0;
	}
	else if(device_Speed == USB_Host_Device_Manager___HIGH_SPEED_DEVICE)
	{
		p_Device -> status.is_Low_Speed_Device = 0;
		p_Device -> status.is_High_Speed_Device = 1;
	}
	else
	{
		p_Device -> status.is_Low_Speed_Device = 0;
		p_Device -> status.is_High_Speed_Device = 0;
	}
}

void USB_Host_Device_Manager___Set_Device_Is_Connected(USB_Host_Device_Manager___Device_TypeDef* p_Device, uint8_t is_Device_Connected)
{
	p_Device -> status.is_Connected = is_Device_Connected;
}


int8_t USB_Host_Device_Manager___Allocate_Device_At_Address_Zero(uint8_t port_Number, uint8_t device_Speed, uint8_t is_Root_Device)
{
	if(USB_Host_Device_Manager___Port[port_Number].p_Device[0] == NULL)
	{
		USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Allocate_Device();
		if(p_Device != NULL)
		{
			USB_Host_Device_Manager___Set_Device_Port_Number(p_Device, port_Number);
			USB_Host_Device_Manager___Set_Device_Speed(p_Device, device_Speed);
			USB_Host_Device_Manager___Set_Device_Is_Connected(p_Device, true);

			USB_Host_Device_Manager___Port[port_Number].p_Device[0] = p_Device;

			return(EXIT_SUCCESS);
		}
		return(EXIT_FAILURE);
	}
	return(EXIT_FAILURE);
}
