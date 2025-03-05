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

void USB_VICE_Host___URB_Setup_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS);

#if USB_VICE_Host___DYNAMICALLY_ALLOCATE_VICE_DEVICES == true

	USB_VICE_Host___VICE_Interface_Node_TypeDef* USB_VICE_Host___Allocate_VICE_Node()
	{
		USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node;
		p_VICE_Node = (USB_VICE_Host___VICE_Interface_Node_TypeDef*)malloc(sizeof(USB_VICE_Host___VICE_Interface_Node_TypeDef));
		p_VICE_Node -> is_Allocated 	= 1;
		p_VICE_Node -> next_Node 	= NULL;
		p_VICE_Node -> previous_Node = NULL;
		return(p_VICE_Node);
	}

	void USB_VICE_Host___Free_VICE_Node(USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node)
	{
		free(p_VICE_Node);
	}

#else

	static USB_VICE_Host___VICE_Interface_Node_TypeDef USB_VICE_Host___VICE_Node_Pool[USB_VICE_Host___NUMBER_OF_VICE_INSTANCES];

	USB_VICE_Host___VICE_Interface_Node_TypeDef* USB_VICE_Host___Allocate_VICE_Node()
	{
		for(uint32_t i = 0; i < USB_VICE_Host___NUMBER_OF_VICE_INSTANCES; i++)
		{
			if(!USB_VICE_Host___VICE_Node_Pool[i].is_Allocated)
			{
				USB_VICE_Host___VICE_Node_Pool[i].is_Allocated 	= true;
				USB_VICE_Host___VICE_Node_Pool[i].next_Node 		= NULL;
				USB_VICE_Host___VICE_Node_Pool[i].previous_Node 	= NULL;
				return(&USB_VICE_Host___VICE_Node_Pool[i]);
			}
		}
		return(NULL);
	}

	void USB_VICE_Host___Free_VICE_Node(USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node)
	{
		p_VICE_Node -> is_Allocated = false;
	}

#endif

static USB_VICE_Host___VICE_Interface_List_TypeDef 	USB_VICE_Host___VICE_Interface_List[USB_Host___NUMBER_OF_PORTS];
static USB_VICE_Host___Application_Callbacks_TypeDef USB_VICE_Host___Application_Callbacks[USB_Host___NUMBER_OF_PORTS];

void USB_VICE_Host___Initiate_VICE_Node(USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node)
{
	p_VICE_Node->VICE_Device.VICE_Descriptor.interface_Registered = false;
}

USB_VICE_Host___VICE_Interface_Node_TypeDef* USB_VICE_Host___Create_VICE_Node(uint8_t port_Number)
{
	USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node = USB_VICE_Host___Allocate_VICE_Node();

	if(p_VICE_Node != NULL)
	{
		USB_VICE_Host___Initiate_VICE_Node(p_VICE_Node);

		p_VICE_Node -> next_Node = NULL;
		p_VICE_Node -> previous_Node = NULL;

		if(USB_VICE_Host___VICE_Interface_List[port_Number].first_Node == NULL)
		{
			USB_VICE_Host___VICE_Interface_List[port_Number].first_Node = p_VICE_Node;
			USB_VICE_Host___VICE_Interface_List[port_Number].last_Node  = p_VICE_Node;
		}
		else
		{
			p_VICE_Node -> previous_Node 												= USB_VICE_Host___VICE_Interface_List[port_Number].last_Node;
			USB_VICE_Host___VICE_Interface_List[port_Number].last_Node -> next_Node 		= p_VICE_Node;
			USB_VICE_Host___VICE_Interface_List[port_Number].last_Node 					= p_VICE_Node;
		}
	}
	return(p_VICE_Node);
}

void USB_VICE_Host___Delete_VICE_Node(uint8_t port_Number, USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node)
{
	if(p_VICE_Node != NULL)
	{
		if(p_VICE_Node == USB_VICE_Host___VICE_Interface_List[port_Number].first_Node)
		{

			USB_VICE_Host___VICE_Interface_List[port_Number].first_Node = p_VICE_Node->next_Node;
		}

		if(p_VICE_Node == USB_VICE_Host___VICE_Interface_List[port_Number].last_Node)
		{
			USB_VICE_Host___VICE_Interface_List[port_Number].last_Node = p_VICE_Node->previous_Node;
		}

		if(p_VICE_Node -> previous_Node != NULL)
		{
			p_VICE_Node->previous_Node -> next_Node 	= p_VICE_Node -> next_Node;
		}

		if(p_VICE_Node -> next_Node != NULL)
		{
			p_VICE_Node->next_Node -> previous_Node 	= p_VICE_Node -> previous_Node;
		}

		USB_VICE_Host___Free_VICE_Node(p_VICE_Node);
	}
}

USB_VICE_Host___VICE_Interface_Node_TypeDef* USB_VICE_Host___Get_VICE_Node_From_Device_Interface(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	if (USB_VICE_Host___VICE_Interface_List[port_Number].first_Node != NULL)
	{
		USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node = USB_VICE_Host___VICE_Interface_List[port_Number].first_Node;
		while (p_VICE_Node != NULL)
		{
			if (p_VICE_Node->VICE_Device.device_Address == device_Address && p_VICE_Node->VICE_Device.interface_Number == interface_Number)
			{
				return (p_VICE_Node);
			}
			p_VICE_Node = p_VICE_Node->next_Node;
		}
	}
	return(NULL);
}

USB_VICE_Host___VICE_Interface_Node_TypeDef* USB_VICE_Host___Get_VICE_Node_From_IN_Endpoint(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number)
{
	if (USB_VICE_Host___VICE_Interface_List[port_Number].first_Node != NULL)
	{
		USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node = USB_VICE_Host___VICE_Interface_List[port_Number].first_Node;
		while (p_VICE_Node != NULL)
		{
			if (p_VICE_Node->VICE_Device.device_Address == device_Address && p_VICE_Node->VICE_Device.interrupt_In_Endpoint_Number == endpoint_Number)
			{
				return (p_VICE_Node);
			}
			p_VICE_Node = p_VICE_Node->next_Node;
		}
	}
	return(NULL);
}

uint8_t USB_Vice_Host___Get_Num_VICE_Instances(uint8_t port_Number)
{
	uint8_t num_Instances = 0;

	if(USB_VICE_Host___VICE_Interface_List[port_Number].first_Node != NULL)
	{
		num_Instances++;
		USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node = USB_VICE_Host___VICE_Interface_List[port_Number].first_Node;

		for (uint8_t i = 0; i < USB_VICE_Host___NUMBER_OF_VICE_INSTANCES; i++)
		{
			if (USB_VICE_Host___VICE_Interface_List[port_Number].first_Node->next_Node != NULL)
			{
				num_Instances++;
				p_VICE_Node = p_VICE_Node->next_Node;
			}
			else
			{
				break;
			}
		}
	}

	return(num_Instances);
}

USB_VICE_Host___VICE_Interface_TypeDef* USB_Vice_Host___Get_VICE_Instance(uint8_t port_Number, uint8_t instance_Number)
{
	USB_VICE_Host___VICE_Interface_Node_TypeDef *p_VICE_Node = USB_VICE_Host___VICE_Interface_List[port_Number].first_Node;

	if(p_VICE_Node != NULL)
	{
		for (uint8_t i = 0; i < instance_Number; i++)
		{
			if(p_VICE_Node->next_Node != NULL)
			{
				p_VICE_Node = p_VICE_Node->next_Node;
			}
			else
			{
				return (NULL);
			}
		}
	}

	return (&(p_VICE_Node->VICE_Device));
}

uint8_t USB_VICE_Host___Is_Device_VICE_Device(uint8_t port_Number, uint8_t device_Address)
{
	uint8_t											num_VICE_Interfaces				= 0;
	USB_Host___Device_Descriptor_TypeDef			device_Descriptor				= USB_Host_Device_Manager___Device_Get_Device_Descriptor(port_Number, device_Address);
	uint8_t 										current_Configuration_Number 	= USB_Host_Device_Manager___Get_Device_Current_Configuration_Number(port_Number, device_Address);

	if(current_Configuration_Number > 0)
	{
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

void USB_VICE_Host___Close_VICE_Interface(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	USB_VICE_Host___VICE_Interface_Node_TypeDef *		p_VICE_Node				= USB_VICE_Host___Get_VICE_Node_From_Device_Interface(port_Number, device_Address, interface_Number);
	uint8_t 										configuration_Number	= USB_Host_Device_Manager___Get_Device_Current_Configuration_Number(port_Number, device_Address);

	for (uint8_t i = 0; i < USB_VICE_Host___MAX_APPLICATIONS; i++)
	{
		if (USB_VICE_Host___Application_Callbacks[port_Number].VICE_Interface_Disconnected_Callback[i] != NULL)
		{
			USB_VICE_Host___Application_Callbacks[port_Number].VICE_Interface_Disconnected_Callback[i](port_Number, device_Address, interface_Number);
		}
	}

	USB_Host_Device_Manager___Remove_Interface_Polling_Process(port_Number, device_Address, configuration_Number ,interface_Number);
	USB_VICE_Host___Delete_VICE_Node(port_Number, p_VICE_Node);
}

void USB_VICE_Host___VICE_Interface_Disconnected_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Number, uint8_t interface_Number)
{
	USB_VICE_Host___Close_VICE_Interface(port_Number, device_Address, interface_Number);
}

void USB_VICE_Host___Get_VICE_Report_Descriptor(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, uint16_t length, uint8_t* p_Buffer, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet_TypeDef setup_Packet;
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_INTERFACE_TO_HOST;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_DESCRIPTOR;
	setup_Packet.wValue 		= 0x2200;
	setup_Packet.wIndex 		= interface_Number;
	setup_Packet.wLength 		= length;

	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_IN, setup_Packet, p_Buffer, length, USB_VICE_Host___STANDARD_NUMBER_OF_RETRIES, callback);
}

uint8_t USB_VICE_Host___Is_Interface_Registered(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	USB_VICE_Host___VICE_Interface_Node_TypeDef *p_VICE_Node = USB_VICE_Host___Get_VICE_Node_From_Device_Interface(port_Number, device_Address, interface_Number);
	return(p_VICE_Node->VICE_Device.VICE_Descriptor.interface_Registered);
}

void USB_VICE_Host___Do_Setup_Stage(USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node)
{
	uint8_t port_Number 		= p_VICE_Node->VICE_Device.port_Number;
	uint8_t device_Address	 	= p_VICE_Node->VICE_Device.device_Address;
	uint8_t interface_Number 	= p_VICE_Node->VICE_Device.interface_Number;

	switch(p_VICE_Node->VICE_Device.setup_Stage)
	{
	case USB_VICE_Host___SETUP_STAGE_GET_VICE_REPORT_DESCRIPTOR:
		{
			uint16_t 	report_Descriptor_Length = p_VICE_Node->VICE_Device.VICE_Descriptor.wDescriptorLength;
			uint8_t* 	report_Descriptor_Buffer = p_VICE_Node->VICE_Device.VICE_Report_Descriptor_Buffer;

			USB_VICE_Host___Get_VICE_Report_Descriptor(port_Number, device_Address, interface_Number, report_Descriptor_Length, report_Descriptor_Buffer, USB_VICE_Host___URB_Setup_Callback);
			break;
		}

	case USB_VICE_Host___SETUP_STAGE_NOTIFY_APPLICATIONS:
		{
			for (uint8_t i = 0; i < USB_VICE_Host___MAX_APPLICATIONS; i++)
			{
				if (USB_VICE_Host___Application_Callbacks[port_Number].VICE_Interface_Connected_Callback[i] != NULL)
				{
					USB_VICE_Host___Application_Callbacks[port_Number].VICE_Interface_Connected_Callback[i](port_Number, device_Address, interface_Number);
				}
			}

			if(USB_VICE_Host___Is_Interface_Registered(port_Number, device_Address, interface_Number) == false)
			{
				printf("Device not registered so it was deleted\n");
				USB_VICE_Host___Close_VICE_Interface(port_Number, device_Address, interface_Number);
			}

			break;
		}
	}
}

void USB_VICE_Host___Set_Next_Setup_Stage(USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node)
{
	switch(p_VICE_Node->VICE_Device.setup_Stage)
	{
	case USB_VICE_Host___SETUP_STAGE_GET_VICE_DESCRIPTOR:
		p_VICE_Node->VICE_Device.setup_Stage = USB_VICE_Host___SETUP_STAGE_GET_VICE_REPORT_DESCRIPTOR;
		break;
	case USB_VICE_Host___SETUP_STAGE_GET_VICE_REPORT_DESCRIPTOR:
		p_VICE_Node->VICE_Device.setup_Stage = USB_VICE_Host___SETUP_STAGE_NOTIFY_APPLICATIONS;
		//USB_VICE_Host___Print_VICE_Report_Descriptor(p_VICE_Node);
		break;
	}
}

void USB_VICE_Host___URB_Setup_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	if(URB.transfer_Status == USB_Host_Transfers___URB_STATUS_SUCCESS)
	{
		USB_VICE_Host___VICE_Interface_Node_TypeDef* p_VICE_Node = USB_VICE_Host___Get_VICE_Node_From_Device_Interface(URB.port_Number, URB.device_Address, URB.control_Setup_Packet.wIndex);
		if(p_VICE_Node != NULL)
		{
			USB_VICE_Host___Set_Next_Setup_Stage(p_VICE_Node);
			USB_VICE_Host___Do_Setup_Stage(p_VICE_Node);
		}
	}
	else
	{
		// delete node, end device
	}
}

void USB_VICE_Host___Get_VICE_Descriptor(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, uint16_t length, uint8_t* p_Buffer, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet_TypeDef setup_Packet;
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_INTERFACE_TO_HOST;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_DESCRIPTOR;
	setup_Packet.wValue 		= 0x2100;
	setup_Packet.wIndex 		= interface_Number;
	setup_Packet.wLength 		= length;

	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_IN, setup_Packet, p_Buffer, length, USB_VICE_Host___STANDARD_NUMBER_OF_RETRIES, callback);
}


void USB_VICE_Host___Setup_VICE_Interface(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Number, uint8_t interface_Number, uint8_t sub_Class)
{
	USB_Host_Device_Manager___Set_Interface_Disconnected_Callback(port_Number, device_Address, configuration_Number, interface_Number, USB_VICE_Host___VICE_Interface_Disconnected_Callback);

	USB_VICE_Host___VICE_Interface_Node_TypeDef* 	VICE_Node 	= USB_VICE_Host___Create_VICE_Node(port_Number);
	USB_VICE_Host___VICE_Interface_TypeDef* 	VICE_Interface 	= &VICE_Node->VICE_Device;

	VICE_Interface->port_Number				= port_Number;
	VICE_Interface->device_Address 			= device_Address;
	VICE_Interface->interface_Number		= interface_Number;
	VICE_Interface->sub_Class				= sub_Class;
	VICE_Interface->setup_Stage				= 0;

	uint8_t num_Endpoints 				= (USB_Host_Device_Manager___Device_Get_Interface_Descriptor(port_Number, device_Address, configuration_Number, interface_Number)).bNumEndpoints;

	for(uint8_t endpoint_Descriptor_Number = 0; endpoint_Descriptor_Number < num_Endpoints; endpoint_Descriptor_Number++)
	{
		USB_Host___Endpoint_Descriptor_TypeDef endpoint_Descriptor = USB_Host_Device_Manager___Device_Get_Endpoint_Descriptor(port_Number, device_Address, configuration_Number, interface_Number, endpoint_Descriptor_Number);

		if((endpoint_Descriptor.bEndpointAddress >> 7) == 1)
		{
			VICE_Interface->interrupt_In_Endpoint_Number 	= endpoint_Descriptor.bEndpointAddress & 0x7f;
			VICE_Interface->interrupt_In_Endpoint_Interval 	= endpoint_Descriptor.bInterval;
		}
		else
		{
			VICE_Interface->interrupt_Out_Endpoint_Number 	= endpoint_Descriptor.bEndpointAddress & 0x7f;
			VICE_Interface->interrupt_Out_Endpoint_Interval = endpoint_Descriptor.bInterval;
		}
	}
	VICE_Node->VICE_Device.setup_Stage = USB_VICE_Host___SETUP_STAGE_NOTIFY_APPLICATIONS;
	USB_VICE_Host___Do_Setup_Stage(VICE_Node);
	//USB_VICE_Host___Get_VICE_Descriptor(port_Number, device_Address, interface_Number, USB_VICE_Host___VICE_Descriptor_Length, (uint8_t*)&(VICE_Interface->VICE_Descriptor), USB_VICE_Host___URB_Setup_Callback);
}

uint8_t USB_VICE_Host___Register_Interface(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	USB_VICE_Host___VICE_Interface_Node_TypeDef* VICE_Node = USB_VICE_Host___Get_VICE_Node_From_Device_Interface(port_Number, device_Address, interface_Number);

	if (VICE_Node != NULL)
	{
		if (VICE_Node->VICE_Device.VICE_Descriptor.interface_Registered == false)
		{
			VICE_Node->VICE_Device.VICE_Descriptor.interface_Registered = true;
			return (EXIT_SUCCESS);
		}
		return (EXIT_SUCCESS);
	}

	return (EXIT_FAILURE);
}

uint8_t USB_VICE_Host___Add_Interface_Connected_Callback(uint8_t port_Number, void (*callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number))
{
	uint8_t first_Free_Index = 0;
	uint8_t free_Index_Found = false;

	for (uint8_t i = 0; i < USB_VICE_Host___MAX_APPLICATIONS; i++)
	{
		if (USB_VICE_Host___Application_Callbacks[port_Number].VICE_Interface_Connected_Callback[i] == NULL && !free_Index_Found)
		{
			free_Index_Found = true;
            first_Free_Index = i;
		}
		else if (USB_VICE_Host___Application_Callbacks[port_Number].VICE_Interface_Connected_Callback[i] == callback)
		{
			return (EXIT_SUCCESS);
		}
	}
	if (free_Index_Found)
	{
		USB_VICE_Host___Application_Callbacks[port_Number].VICE_Interface_Connected_Callback[first_Free_Index] = callback;
		return (EXIT_SUCCESS);
	}
	return (EXIT_FAILURE);
}

uint8_t USB_VICE_Host___Add_Interface_Disconnected_Callback(uint8_t port_Number, void (*callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number))
{
	uint8_t first_Free_Index = 0;
	uint8_t free_Index_Found = false;

	for (uint8_t i = 0; i < USB_VICE_Host___MAX_APPLICATIONS; i++)
	{
		if (USB_VICE_Host___Application_Callbacks[port_Number].VICE_Interface_Disconnected_Callback[i] == NULL && !free_Index_Found)
		{
			free_Index_Found = true;
            first_Free_Index = i;
		}
		else if (USB_VICE_Host___Application_Callbacks[port_Number].VICE_Interface_Disconnected_Callback[i] == callback)
		{
			return (EXIT_SUCCESS);
		}
	}
	if (free_Index_Found)
	{
		USB_VICE_Host___Application_Callbacks[port_Number].VICE_Interface_Disconnected_Callback[first_Free_Index] = callback;
		return (EXIT_SUCCESS);
	}
	return (EXIT_FAILURE);
}

void USB_VICE_Host___Setup_VICE_Device(uint8_t port_Number, uint8_t device_Address)
{
	uint8_t 										current_Configuration_Number	= USB_Host_Device_Manager___Get_Device_Current_Configuration_Number(port_Number, device_Address);

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
					USB_VICE_Host___Setup_VICE_Interface(port_Number, device_Address, current_Configuration_Number, i, interface_Descriptor.bInterfaceSubClass);
				}
			}
		}
	}
}

void USB_VICE_Host___Device_Connected_Callback(uint8_t port_Number, uint8_t device_Address)
{
	if(USB_VICE_Host___Is_Device_VICE_Device(port_Number, device_Address))
	{
		USB_VICE_Host___Setup_VICE_Device(port_Number, device_Address);
	}
}


void USB_VICE_Host___Init(uint8_t port_Number)
{
	USB_Host___Add_Device_Connected_Callback(port_Number, USB_VICE_Host___Device_Connected_Callback);
}
