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

#endif // --------------------------------------------------------------------------------------------------------

void USB_Host_Device_Manager___Device_Set_Is_Root_Device(uint8_t port_Number, uint8_t device_Address, uint8_t is_Root_Device)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];
	p_Device -> status.is_Root_Device = is_Root_Device;
}

void USB_Host_Device_Manager___Device_Set_Is_Connected(uint8_t port_Number, uint8_t device_Address, uint8_t is_Connected)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	p_Device -> status.is_Connected_Status_Change 	= true;
	p_Device -> status.is_Connected 				= is_Connected;
}

int8_t USB_Host_Device_Manager___Port_Set_Device_To_Address(uint8_t port_Number, uint8_t device_Address, USB_Host_Device_Manager___Device_TypeDef* p_Device)
{
	if(USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address] == NULL)
	{
		USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address] = p_Device;
		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

void USB_Host_Device_Manager___Port_Set_Is_New_Device_Connected(uint8_t port_Number)
{
	USB_Host_Device_Manager___Port[port_Number].port_Status.is_New_Device_Connected = true;
}


void USB_Host_Device_Manager___Port_Clear_Is_New_Device_Connected(uint8_t port_Number)
{
	USB_Host_Device_Manager___Port[port_Number].port_Status.is_New_Device_Connected = false;
}

uint8_t USB_Host_Device_Manager___Device_Is_Connected_Status_Change(uint8_t port_Number, uint8_t device_Address, uint8_t is_Connected)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	return(p_Device -> status.is_Connected_Status_Change);
}

uint8_t USB_Host_Device_Manager__Port_Set_Is_New_Device_Connected(uint8_t port_Number)
{
	return(USB_Host_Device_Manager___Port[port_Number].port_Status.is_New_Device_Connected);
}


uint8_t USB_Host_Device_Manager__Port_Is_New_Device_Connected(uint8_t port_Number)
{
	return(USB_Host_Device_Manager___Port[port_Number].port_Status.is_New_Device_Connected);
}


void USB_Host_Device_Manager___Device_Set_Port_Number(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	p_Device -> status.port_Number = port_Number;
}

void USB_Host_Device_Manager___Device_Set_Speed(uint8_t port_Number, uint8_t device_Address, uint8_t device_Speed)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

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



int8_t USB_Host_Device_Manager___Allocate_Device_At_Address_Zero(uint8_t port_Number, uint8_t device_Speed, uint8_t is_Root_Device)
{
	if(USB_Host_Device_Manager___Port[port_Number].p_Device[0] == NULL)
	{
		USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Allocate_Device();
		if(USB_Host_Device_Manager___Port_Set_Device_To_Address(port_Number, 0, p_Device) == EXIT_SUCCESS)
		{
			USB_Host_Device_Manager___Device_Set_Port_Number			(port_Number, 0);
			USB_Host_Device_Manager___Device_Set_Speed					(port_Number, 0, device_Speed);
			USB_Host_Device_Manager___Device_Set_Is_Connected			(port_Number, 0, true);
			USB_Host_Device_Manager___Port_Set_Is_New_Device_Connected	(port_Number);

			return(EXIT_SUCCESS);
		}
		return(EXIT_FAILURE);
	}
	return(EXIT_FAILURE);
}
