/*
 * USB_Device_Desciptors.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>
#include <stdio.h>

#include "../Inc/USB_Device_Descriptors.h"
#include "../Inc/USB_Device.h"
struct USB_Device_Descriptors___Entry USB_Device_Descriptors___Entries[USB_Device___NUM_PORTS][USB_Device_Descriptors___MAX_ENTRIES];

void USB_Device_Descriptors___Add_Entry(uint8_t port_Number, uint8_t descriptor_Type, uint8_t index, uint8_t* descriptor, uint16_t descriptor_Size)
{
	for (uint8_t i = 0; i < USB_Device_Descriptors___MAX_ENTRIES; i++)
	{
		if (USB_Device_Descriptors___Entries[port_Number][i].descriptor == NULL)
		{
			USB_Device_Descriptors___Entries[port_Number][i].descriptor_Type	= descriptor_Type;
			USB_Device_Descriptors___Entries[port_Number][i].index 				= index;
			USB_Device_Descriptors___Entries[port_Number][i].descriptor 		= descriptor;
			USB_Device_Descriptors___Entries[port_Number][i].descriptor_Size 	= descriptor_Size;
			break;
		}
	}
}

void USB_Device_Descriptors___Remove_Entry(uint8_t port_Number, uint8_t* descriptor)
{
	for (uint8_t i = 0; i < USB_Device_Descriptors___MAX_ENTRIES; i++)
	{
		if (USB_Device_Descriptors___Entries[port_Number][i].descriptor == descriptor)
		{
			USB_Device_Descriptors___Entries[port_Number][i].descriptor = NULL;
			break;
		}
	}
}

USB_Device_Descriptors___Entry_TypeDef* USB_Device_Descriptors___Get_Descriptor(uint8_t port_Number, uint8_t descriptor_Type, uint8_t index)
{
	for (uint8_t i = 0; i < USB_Device_Descriptors___MAX_ENTRIES; i++)
	{
		if (USB_Device_Descriptors___Entries[port_Number][i].descriptor_Type == descriptor_Type && USB_Device_Descriptors___Entries[port_Number][i].index == index)
		{
			return(&USB_Device_Descriptors___Entries[port_Number][i]);
		}
	}

	return(NULL);
}
