/*
 * USB_HID_Host.c
 *
 *  Created on: Dec 27, 2024
 *      Author: jackh
 */

#include "../Inc/USB_HID_Host.h"

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>
#include <stdio.h>
#include <USB_Host.h>
#include <USB_Host_Device_Manager.h>
#include <USB_Host_Transfers.h>

#if USB_HID_Host___DYNAMICALLY_ALLOCATE_HID_DEVICES == true

	USB_HID_Host___HID_Node_TypeDef* USB_Host_Hub___Allocate_HID_Node()
	{
		USB_HID_Host___HID_Node_TypeDef* p_HID_Node;
		p_HID_Node = (USB_HID_Host___HID_Node_TypeDef*)malloc(sizeof(USB_HID_Host___HID_Node_TypeDef));
		p_HID_Node -> is_Allocated 	= 1;
		p_HID_Node -> next_Node 	= NULL;
		p_HID_Node -> previous_Node = NULL;
		return(p_HID_Node);
	}

	void USB_Host_Hub___Free_HID_Node(USB_HID_Host___HID_Node_TypeDef* p_HID_Node)
	{
		free(p_HID_Node);
	}

#else

	static USB_HID_Host___HID_Node_TypeDef USB_HID_Host___HID_Node_Pool[USB_HID_Host___NUMBER_OF_HID_INSTANCES];

	USB_HID_Host___HID_Node_TypeDef* USB_Host_Hub___Allocate_HID_Node()
	{
		for(uint32_t i = 0; i < USB_HID_Host___NUMBER_OF_HID_INSTANCES; i++)
		{
			if(!USB_HID_Host___HID_Node_Pool[i].is_Allocated)
			{
				USB_HID_Host___HID_Node_Pool[i].is_Allocated 	= true;
				USB_HID_Host___HID_Node_Pool[i].next_Node 		= NULL;
				USB_HID_Host___HID_Node_Pool[i].previous_Node 	= NULL;
				return(&USB_HID_Host___HID_Node_Pool[i]);
			}
		}
		return(NULL);
	}

	void USB_Host_Hub___Free_HID_Node(USB_HID_Host___HID_Node_TypeDef* p_HID_Node)
	{
		p_HID_Node -> is_Allocated = false;
	}

#endif

uint8_t USB_HID_Host___Is_Device_HID_Device(uint8_t port_Number, uint8_t device_Address)
{
	uint8_t											num_HID_Interfaces			= 0;
	USB_Host___Device_Descriptor_TypeDef			device_Descriptor			= USB_Host_Device_Manager___Device_Get_Device_Descriptor(port_Number, device_Address);
	uint8_t 										current_Configuration 		= USB_Host_Device_Manager___Get_Device_Current_Configuration(port_Number, device_Address);
	USB_Host___Configuration_Descriptor_TypeDef 	configuration_Descriptor 	= USB_Host_Device_Manager___Device_Get_Configuration_Descriptor(port_Number, device_Address, current_Configuration);

	if(device_Descriptor.bDeviceClass == USB_HID_Host___COMPOSITE_DEVICE_CLASS)
	{
		for(uint8_t i = 0; i < configuration_Descriptor.bNumInterfaces; i++)
		{
			USB_Host___Interface_Descriptor_TypeDef 	interface_Descriptor 		= USB_Host_Device_Manager___Device_Get_Interface_Descriptor(port_Number, device_Address, current_Configuration, i);

			if(interface_Descriptor.bInterfaceClass == USB_HID_Host___HID_INTERFACE_CLASS)
			{
				num_HID_Interfaces++;
			}
		}
	}
	return(num_HID_Interfaces);
}

void USB_HID_Host___HID_Interface_Disconnected_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Number, uint8_t interface_Number)
{
	printf("Interface Disconnected at interface %d\n", interface_Number);
}

void USB_HID_Host___Setup_HID_Interface(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Number, uint8_t interface_Number)
{
	USB_Host_Device_Manager___Set_Interface_Disconnected_Callback(port_Number, device_Address, configuration_Number, interface_Number, USB_HID_Host___HID_Interface_Disconnected_Callback);
	printf("interface %d is a HID interface\n", interface_Number);
}

void USB_HID_Host___Setup_HID_Device(uint8_t port_Number, uint8_t device_Address)
{
	uint8_t 										current_Configuration 		= USB_Host_Device_Manager___Get_Device_Current_Configuration(port_Number, device_Address);
	USB_Host___Device_Descriptor_TypeDef			device_Descriptor			= USB_Host_Device_Manager___Device_Get_Device_Descriptor(port_Number, device_Address);
	USB_Host___Configuration_Descriptor_TypeDef 	configuration_Descriptor 	= USB_Host_Device_Manager___Device_Get_Configuration_Descriptor(port_Number, device_Address, current_Configuration);

	if(device_Descriptor.bDeviceClass == USB_HID_Host___COMPOSITE_DEVICE_CLASS)
	{
		for(uint8_t i = 0; i < configuration_Descriptor.bNumInterfaces; i++)
		{
			USB_Host___Interface_Descriptor_TypeDef 	interface_Descriptor 		= USB_Host_Device_Manager___Device_Get_Interface_Descriptor(port_Number, device_Address, current_Configuration, i);

			if(interface_Descriptor.bInterfaceClass == USB_HID_Host___HID_INTERFACE_CLASS)
			{
				USB_HID_Host___Setup_HID_Interface(port_Number, device_Address, current_Configuration, i);
			}
		}
	}
}
