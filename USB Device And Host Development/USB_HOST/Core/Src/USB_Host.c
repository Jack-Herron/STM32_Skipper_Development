/*
 * USB_Host.cpp
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>

#include <USB_LL_Hardware.h>
#include <USB_LL_Host.h>
#include <USB_LL_Interrupts_Host.h>
#include "../Inc/USB_Host_Device_Manager.h"
#include "../Inc/USB_Host.h"

void USB_Host___Init(uint8_t port_Number)
{
	USB_LL_Hardware___GPIO_Init(port_Number);
	USB_LL_Hardware___Init(port_Number, USB_LL_Hardware___HOST_MODE);
}

uint8_t USB_Host___Convert_USB_LL_Interrupts_Host_Speed_To_USB_Host_Device_Manager_Speed(uint8_t USB_Interrupts_Host_Speed)
{
	if(USB_Interrupts_Host_Speed == USB_LL_Interrupts_Host___LOW_SPEED_VALUE)
	{
		return(USB_Host_Device_Manager___LOW_SPEED_DEVICE);
	}
	else if(USB_Interrupts_Host_Speed == USB_LL_Interrupts_Host___HIGH_SPEED_VALUE)
	{
		return(USB_Host_Device_Manager___HIGH_SPEED_DEVICE);
	}
	else
	{
		return(USB_Host_Device_Manager___FULL_SPEED_DEVICE);
	}
}

void USB_Host___Process_Host_Interrupts(uint8_t port_Number)
{
	if(USB_LL_Interrupts_Host___Is_Root_Device_Connection_Status_Change(port_Number))
	{
		if(USB_LL_Interrupts_Host___Is_Root_Device_Connected(port_Number))
		{
			uint8_t USB_LL_Interrupts_Host_Speed = USB_LL_Interrupts_Host___Get_Root_Device_Speed(port_Number);

			uint8_t device_Manager_Speed = USB_Host___Convert_USB_LL_Interrupts_Host_Speed_To_USB_Host_Device_Manager_Speed(USB_LL_Interrupts_Host_Speed);

			if(USB_Host_Device_Manager___Allocate_Device_At_Address_Zero(port_Number, device_Manager_Speed, true) == EXIT_SUCCESS)
			{
				USB_LL_Interrupts_Host___Clear_Connection_Status_Change(port_Number);
			}
		}
		else
		{
			uint8_t root_Device_Address = USB_Host_Device_Manager___Port_Get_Root_Device_Address(port_Number);
			USB_Host_Device_Manager___Device_Disconnected(port_Number, root_Device_Address);
			USB_LL_Interrupts_Host___Clear_Connection_Status_Change(port_Number);
		}
	}
}

void USB_Host___Process_Device_Connect(uint8_t port_Number, uint8_t device_Address)
{

}

void USB_Host___Process_Device_Disconnect(uint8_t port_Number, uint8_t device_Address)
{

}

void USB_Host___Process_Device_Manager_Status(uint8_t port_Number)
{
	if(USB_Host_Device_Manager__Port_Is_Device_Connected_Or_Disconnected_Flag(port_Number))
	{
		USB_Host_Device_Manager___Port_Clear_Device_Connected_Or_Disconnected_Flag(port_Number);
		for(uint8_t i = 0; i < USB_Host_Device_Manager___PORT_DEVICE_LIMIT; i++)
		{
			if(USB_Host_Device_Manager___Device_Is_Connected_Status_Change(port_Number, i))
			{
				if(USB_Host_Device_Manager___Device_Is_Connected(port_Number, i))
				{
					USB_Host___Process_Device_Connect(port_Number, i);
				}
				else
				{
					USB_Host___Process_Device_Disconnect(port_Number, i);
				}
			}
		}
	}
}

void USB_Host___Process(uint8_t port_Number)
{
	USB_Host___Process_Host_Interrupts(port_Number);
	USB_Host___Process_Device_Manager_Status(port_Number);
}
