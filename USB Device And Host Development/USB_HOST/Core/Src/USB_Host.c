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

void USB_Host___Proccess_Host_Interrupts(uint8_t port_Number)
{
	if(USB_LL_Interrupts_Host___Is_Root_Device_Connection_Status_Change(port_Number))
	{
		if(USB_LL_Interrupts_Host___Is_Root_Device_Connected(port_Number))
		{

		}
		else
		{

		}
	}
}

void USB_Host___Proccess_Device_Manager_Status(uint8_t port_Number)
{
	USB_Host_Device_Manager___Port_Status_TypeDef port_Status = USB_Host_Device_Manager___Get_Port_Status(port_Number);

	if(port_Status.is_New_Device_Connected)
	{

	}

	if(port_Status.is_New_Device_Enumerated)
	{

	}
}

void USB_Host___Proccess(uint8_t port_Number)
{
	USB_Host___Proccess_Host_Interrupts(port_Number);
	USB_Host___Proccess_Device_Manager_Status(port_Number);
}
