/*
 * USB_VICE_Host.c
 *
 *  Created on: Dec 25, 2024
 *      Author: jackh
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>
#include <stdio.h>
#include "../Inc/USB_VICE_Host.h"

#include <USB_Host.h>
#include <USB_Host_Device_Manager.h>
#include <USB_Host_Transfers.h>

uint8_t USB_VICE_Host___Is_Device_VICE_Device(uint8_t port_Number, uint8_t device_Address)
{
	uint8_t											num_VICE_Interfaces				= 0;
	uint8_t 										current_Configuration_Number 	= USB_Host_Device_Manager___Get_Device_Current_Configuration_Number(port_Number, device_Address);
	if(current_Configuration_Number > 0)
	{
		USB_Host___Device_Descriptor_TypeDef			device_Descriptor				= USB_Host_Device_Manager___Device_Get_Device_Descriptor(port_Number, device_Address);
		USB_Host___Configuration_Descriptor_TypeDef 	configuration_Descriptor 		= USB_Host_Device_Manager___Device_Get_Configuration_Descriptor(port_Number, device_Address, current_Configuration_Number);
		if(device_Descriptor.bDeviceClass == USB_VICE_Host___COMPOSITE_DEVICE_CLASS)
		{
			for(uint8_t i = 0; i < configuration_Descriptor.bNumInterfaces; i++)
			{
				USB_Host___Interface_Descriptor_TypeDef 	interface_Descriptor 		= USB_Host_Device_Manager___Device_Get_Interface_Descriptor(port_Number, device_Address, current_Configuration_Number, i);

				if(interface_Descriptor.bInterfaceClass == USB_VICE_Host___VICE_INTERFACE_CLASS)
				{
					num_VICE_Interfaces++;
				}
			}
		}
	}
	return(num_VICE_Interfaces);
}

void USB_VICE_Host___Setup_VICE_Interface(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{

}

void USB_VICE_Host___Setup_VICE_Device(uint8_t port_Number, uint8_t device_Address)
{
	uint8_t 										current_Configuration_Number 	= USB_Host_Device_Manager___Get_Device_Current_Configuration_Number(port_Number, device_Address);
	if(current_Configuration_Number > 0)
	{
		USB_Host___Device_Descriptor_TypeDef			device_Descriptor				= USB_Host_Device_Manager___Device_Get_Device_Descriptor(port_Number, device_Address);
		USB_Host___Configuration_Descriptor_TypeDef 	configuration_Descriptor 		= USB_Host_Device_Manager___Device_Get_Configuration_Descriptor(port_Number, device_Address, current_Configuration_Number);

		if(device_Descriptor.bDeviceClass == USB_VICE_Host___COMPOSITE_DEVICE_CLASS)
		{
			for(uint8_t i = 0; i < configuration_Descriptor.bNumInterfaces; i++)
			{
				USB_Host___Interface_Descriptor_TypeDef 	interface_Descriptor 		= USB_Host_Device_Manager___Device_Get_Interface_Descriptor(port_Number, device_Address, current_Configuration_Number, i);

				if(interface_Descriptor.bInterfaceClass == USB_VICE_Host___VICE_INTERFACE_CLASS)
				{
					USB_VICE_Host___Setup_VICE_Interface(port_Number, device_Address, i);
				}
			}
		}
	}
}

void USB_VICE_Host___Setup_Device(uint8_t port_Number, uint8_t device_Address)
{

}
