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

	USB_HID_Host___HID_Interface_Node_TypeDef* USB_HID_Host___Allocate_HID_Node()
	{
		USB_HID_Host___HID_Interface_Node_TypeDef* p_HID_Node;
		p_HID_Node = (USB_HID_Host___HID_Interface_Node_TypeDef*)malloc(sizeof(USB_HID_Host___HID_Interface_Node_TypeDef));
		p_HID_Node -> is_Allocated 	= 1;
		p_HID_Node -> next_Node 	= NULL;
		p_HID_Node -> previous_Node = NULL;
		return(p_HID_Node);
	}

	void USB_HID_Host___Free_HID_Node(USB_HID_Host___HID_Interface_Node_TypeDef* p_HID_Node)
	{
		free(p_HID_Node);
	}

#else

	static USB_HID_Host___HID_Interface_Node_TypeDef USB_HID_Host___HID_Node_Pool[USB_HID_Host___NUMBER_OF_HID_INSTANCES];

	USB_HID_Host___HID_Interface_Node_TypeDef* USB_HID_Host___Allocate_HID_Node()
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

	void USB_HID_Host___Free_HID_Node(USB_HID_Host___HID_Interface_Node_TypeDef* p_HID_Node)
	{
		p_HID_Node -> is_Allocated = false;
	}

#endif

static USB_HID_Host___HID_Interface_List_TypeDef 	USB_HID_Host___HID_Interface_List[USB_Host___NUMBER_OF_PORTS];
static USB_HID_Host___Application_Callbacks_TypeDef USB_HID_Host___Application_Callbacks[USB_Host___NUMBER_OF_PORTS];

void USB_HID_Host___Initiate_HID_Node(USB_HID_Host___HID_Interface_Node_TypeDef* p_HID_Node)
{
	p_HID_Node->HID_Device.HID_Descriptor.interface_Registered = false;
}

USB_HID_Host___HID_Interface_Node_TypeDef* USB_HID_Host___Create_HID_Node(uint8_t port_Number)
{
	USB_HID_Host___HID_Interface_Node_TypeDef* p_HID_Node = USB_HID_Host___Allocate_HID_Node();

	if(p_HID_Node != NULL)
	{
		USB_HID_Host___Initiate_HID_Node(p_HID_Node);

		p_HID_Node -> next_Node = NULL;
		p_HID_Node -> previous_Node = NULL;

		if(USB_HID_Host___HID_Interface_List[port_Number].first_Node == NULL)
		{
			USB_HID_Host___HID_Interface_List[port_Number].first_Node = p_HID_Node;
			USB_HID_Host___HID_Interface_List[port_Number].last_Node  = p_HID_Node;
		}
		else
		{
			p_HID_Node -> previous_Node 												= USB_HID_Host___HID_Interface_List[port_Number].last_Node;
			USB_HID_Host___HID_Interface_List[port_Number].last_Node -> next_Node 		= p_HID_Node;
			USB_HID_Host___HID_Interface_List[port_Number].last_Node 					= p_HID_Node;
		}
	}
	return(p_HID_Node);
}

void USB_HID_Host___Delete_HID_Node(uint8_t port_Number, USB_HID_Host___HID_Interface_Node_TypeDef* p_HID_Node)
{
	if(p_HID_Node != NULL)
	{
		if(p_HID_Node == USB_HID_Host___HID_Interface_List[port_Number].first_Node)
		{

			USB_HID_Host___HID_Interface_List[port_Number].first_Node = p_HID_Node->next_Node;
		}

		if(p_HID_Node == USB_HID_Host___HID_Interface_List[port_Number].last_Node)
		{
			USB_HID_Host___HID_Interface_List[port_Number].last_Node = p_HID_Node->previous_Node;
		}

		if(p_HID_Node -> previous_Node != NULL)
		{
			p_HID_Node->previous_Node -> next_Node 	= p_HID_Node -> next_Node;
		}

		if(p_HID_Node -> next_Node != NULL)
		{
			p_HID_Node->next_Node -> previous_Node 	= p_HID_Node -> previous_Node;
		}

		USB_HID_Host___Free_HID_Node(p_HID_Node);
	}
}

void USB_HID_Host___Polling_Callback(uint8_t port_Number, void* context)
{
	printf("polling callback triggered\n");
}

USB_HID_Host___HID_Interface_Node_TypeDef* USB_HID_Host___Get_HID_Node_From_Device_Interface(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	if (USB_HID_Host___HID_Interface_List[port_Number].first_Node != NULL)
	{
		USB_HID_Host___HID_Interface_Node_TypeDef* p_HID_Node = USB_HID_Host___HID_Interface_List[port_Number].first_Node;
		while (p_HID_Node != NULL)
		{
			if (p_HID_Node->HID_Device.device_Address == device_Address && p_HID_Node->HID_Device.interface_Number == interface_Number)
			{
				return (p_HID_Node);
			}
			p_HID_Node = p_HID_Node->next_Node;
		}
	}
	return(NULL);
}

uint8_t USB_HID_Host___Is_Device_HID_Device(uint8_t port_Number, uint8_t device_Address)
{
	uint8_t											num_HID_Interfaces				= 0;
	USB_Host___Device_Descriptor_TypeDef			device_Descriptor				= USB_Host_Device_Manager___Device_Get_Device_Descriptor(port_Number, device_Address);
	uint8_t 										current_Configuration_Number 	= USB_Host_Device_Manager___Get_Device_Current_Configuration_Number(port_Number, device_Address);
	if(current_Configuration_Number > 0)
	{
		USB_Host___Configuration_Descriptor_TypeDef 	configuration_Descriptor 		= USB_Host_Device_Manager___Device_Get_Configuration_Descriptor(port_Number, device_Address, current_Configuration_Number);

		if(device_Descriptor.bDeviceClass == USB_HID_Host___COMPOSITE_DEVICE_CLASS)
		{
			for(uint8_t i = 0; i < configuration_Descriptor.bNumInterfaces; i++)
			{
				USB_Host___Interface_Descriptor_TypeDef 	interface_Descriptor 		= USB_Host_Device_Manager___Device_Get_Interface_Descriptor(port_Number, device_Address, current_Configuration_Number, i);

				if(interface_Descriptor.bInterfaceClass == USB_HID_Host___HID_INTERFACE_CLASS)
				{
					num_HID_Interfaces++;
				}
			}
		}
	}
	return(num_HID_Interfaces);
}

void USB_HID_Host___HID_Interface_Disconnected_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Number, uint8_t interface_Number)
{
	USB_HID_Host___HID_Interface_Node_TypeDef* HID_Node = USB_HID_Host___Get_HID_Node_From_Device_Interface(port_Number, device_Address, interface_Number);
	USB_HID_Host___Delete_HID_Node(port_Number, HID_Node);

	printf("Interface Disconnected at interface %d\n", interface_Number);
}

void USB_HID_Host___URB_Setup_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS);

void USB_HID_Host___Get_HID_Report_Descriptor(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, uint16_t length, uint8_t* p_Buffer, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet_TypeDef setup_Packet;
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_INTERFACE_TO_HOST;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_DESCRIPTOR;
	setup_Packet.wValue 		= 0x2200;
	setup_Packet.wIndex 		= interface_Number;
	setup_Packet.wLength 		= length;

	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_IN, setup_Packet, p_Buffer, length, USB_HID_Host___STANDARD_NUMBER_OF_RETRIES, callback);
}

void USB_HID_Host___Get_HID_Descriptor(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, uint16_t length, uint8_t* p_Buffer, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet_TypeDef setup_Packet;
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_INTERFACE_TO_HOST;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_DESCRIPTOR;
	setup_Packet.wValue 		= 0x2100;
	setup_Packet.wIndex 		= interface_Number;
	setup_Packet.wLength 		= length;

	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_IN, setup_Packet, p_Buffer, length, USB_HID_Host___STANDARD_NUMBER_OF_RETRIES, callback);
}

void USB_HID_Host___Do_Setup_Stage(USB_HID_Host___HID_Interface_Node_TypeDef* p_HID_Node)
{
	uint8_t port_Number 		= p_HID_Node->HID_Device.port_Number;
	uint8_t device_Address	 	= p_HID_Node->HID_Device.device_Address;
	uint8_t interface_Number 	= p_HID_Node->HID_Device.interface_Number;

	switch(p_HID_Node->HID_Device.setup_Stage)
	{
	case USB_HID_Host___SETUP_STAGE_GET_HID_REPORT_DESCRIPTOR:
		{
			uint16_t 	report_Descriptor_Length = p_HID_Node->HID_Device.HID_Descriptor.wDescriptorLength;
			uint8_t* 	report_Descriptor_Buffer = p_HID_Node->HID_Device.HID_Report_Descriptor_Buffer;

			USB_HID_Host___Get_HID_Report_Descriptor(port_Number, device_Address, interface_Number, report_Descriptor_Length, report_Descriptor_Buffer, USB_HID_Host___URB_Setup_Callback);
			break;
		}

	case USB_HID_Host___SETUP_STAGE_NOTIFY_APPLICATIONS:
		{
			for (uint8_t i = 0; i < USB_HID_Host___MAX_APPLICATIONS; i++)
			{
				if (USB_HID_Host___Application_Callbacks[port_Number].HID_Interface_Connected_Callback[i] != NULL)
				{
					USB_HID_Host___Application_Callbacks[port_Number].HID_Interface_Connected_Callback[i](port_Number, device_Address, interface_Number);
				}
			}
			break;
		}
	}
}

void print_Usage(uint16_t usage)
{
	switch(usage)
	{
	case 0x01:
		printf("Pointer");
		break;
	case 0x02:
		printf("Mouse");
		break;
	case 0x06:
		printf("Keyboard");
		break;
	case 0x07:
		printf("Keypad");
		break;
	case 0x30:
		printf("X");
		break;
	case 0x31:
		printf("Y");
		break;
	case 0x38:
		printf("Wheel");
		break;
	default:
		printf("%x", usage);
		break;
	}
}

void USB_HID_Host___Print_HID_Report_Descriptor(USB_HID_Host___HID_Interface_Node_TypeDef* p_HID_Node)
{
	uint16_t 	report_Descriptor_Length = p_HID_Node->HID_Device.HID_Descriptor.wDescriptorLength;
	uint8_t* 	report_Descriptor_Buffer = p_HID_Node->HID_Device.HID_Report_Descriptor_Buffer;

	uint8_t 	cont = true;
	uint16_t 	current_Index = 0;
	printf("\n");
	printf("Report descriptor: \n");

	while(cont)
	{
		uint8_t item = report_Descriptor_Buffer[current_Index];
		uint8_t item_Size_Value = item & 0x03;
		uint8_t item_Size = 1;
		uint8_t print_Bytes = 1;


		switch(item_Size_Value)
		{
		case 1:
			item_Size += 1;
			break;
		case 2:
			item_Size += 2;
			break;
		case 3:
			item_Size += 4;
			break;
		}

		switch(item & 0xfc)
		{
		case 0x04:
			printf("Usage page: ");
			break;
		case 0x14:
			printf("Logical minimum: ");
			break;
		case 0x24:
			printf("Logical maximum: ");
			break;
		case 0x34:
			printf("Physical minimum: ");
			break;
		case 0x44:
			printf("Physical maximum: ");
			break;
		case 0x80:
			printf("Input: ");
			break;
		case 0x90:
			printf("Output: ");
			break;
		case 0xb0:
			printf("Feature: ");
			break;
		case 0xa0:
			printf("Collection: ");
			break;
		case 0xc0:
			printf("End collection: ");
			break;
		case 0x84:
			printf("Report ID: ");
			break;
		case 0x74:
			printf("Report size: ");
			break;
		case 0x94:
			printf("Report count: ");
			break;
		case 0x08:
			print_Bytes = 0;
			printf("Usage: ");
			{
				uint16_t usage_Size = item_Size -1;
				uint8_t usage = 0;
				if(usage_Size == 1)
				{
					usage = report_Descriptor_Buffer[current_Index+1];
				}
				else
				{
					usage = report_Descriptor_Buffer[current_Index+2] | (report_Descriptor_Buffer[current_Index+1] << 8);
				}
				print_Usage(usage);
			}

			break;
		case 0x18:
			printf("Usage minimum: ");
			break;
		case 0x28:
			printf("Usage maximum: ");
			break;
		case 0x78:
			printf("String index: ");
			break;
		default:
			printf("Undefined item: ");
			break;
		}
		if(print_Bytes)
		{
			for(uint16_t i = 1; i < item_Size; i++)
			{
				printf("%d ", report_Descriptor_Buffer[current_Index+i]);
			}
		}
		printf("\n");

		current_Index+= item_Size;

		if(current_Index >= report_Descriptor_Length)
		{
			cont = false;
		}
	}
	printf("\n");
}

void USB_HID_Host___Set_Next_Setup_Stage(USB_HID_Host___HID_Interface_Node_TypeDef* p_HID_Node)
{
	switch(p_HID_Node->HID_Device.setup_Stage)
	{
	case USB_HID_Host___SETUP_STAGE_GET_HID_DESCRIPTOR:
		p_HID_Node->HID_Device.setup_Stage = USB_HID_Host___SETUP_STAGE_GET_HID_REPORT_DESCRIPTOR;
		break;
	case USB_HID_Host___SETUP_STAGE_GET_HID_REPORT_DESCRIPTOR:
		p_HID_Node->HID_Device.setup_Stage = USB_HID_Host___SETUP_STAGE_NOTIFY_APPLICATIONS;
		USB_HID_Host___Print_HID_Report_Descriptor(p_HID_Node);
		break;
	}
}

void USB_HID_Host___URB_Setup_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	if(URB.transfer_Status == USB_Host_Transfers___URB_STATUS_SUCCESS)
	{
		USB_HID_Host___HID_Interface_Node_TypeDef* p_HID_Node = USB_HID_Host___Get_HID_Node_From_Device_Interface(URB.port_Number, URB.device_Address, URB.control_Setup_Packet.wIndex);
		if(p_HID_Node != NULL)
		{
			USB_HID_Host___Set_Next_Setup_Stage(p_HID_Node);
			USB_HID_Host___Do_Setup_Stage(p_HID_Node);
		}
	}
	else
	{
		// delete node, end device
	}
}

void USB_HID_Host___Setup_HID_Interface(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Number, uint8_t interface_Number)
{
	USB_Host_Device_Manager___Set_Interface_Disconnected_Callback(port_Number, device_Address, configuration_Number, interface_Number, USB_HID_Host___HID_Interface_Disconnected_Callback);

	USB_HID_Host___HID_Interface_Node_TypeDef* 	HID_Node 	= USB_HID_Host___Create_HID_Node(port_Number);
	USB_HID_Host___HID_Interface_TypeDef* 	HID_Interface 	= &HID_Node->HID_Device;

	HID_Interface->port_Number				= port_Number;
	HID_Interface->device_Address 			= device_Address;
	HID_Interface->interface_Number		= interface_Number;
	HID_Interface->setup_Stage				= 0;

	uint8_t num_Endpoints 				= (USB_Host_Device_Manager___Device_Get_Interface_Descriptor(port_Number, device_Address, configuration_Number, interface_Number)).bNumEndpoints;

	for(uint8_t endpoint_Descriptor_Number = 0; endpoint_Descriptor_Number < num_Endpoints; endpoint_Descriptor_Number++)
	{
		USB_Host___Endpoint_Descriptor_TypeDef endpoint_Descriptor = USB_Host_Device_Manager___Device_Get_Endpoint_Descriptor(port_Number, device_Address, configuration_Number, interface_Number, endpoint_Descriptor_Number);

		if((endpoint_Descriptor.bEndpointAddress >> 7) == 1)
		{
			HID_Interface->interrupt_In_Endpoint_Number 	= endpoint_Descriptor.bEndpointAddress & 0x7f;
			HID_Interface->interrupt_In_Endpoint_Interval 	= endpoint_Descriptor.bInterval;
		}
		else
		{
			HID_Interface->interrupt_Out_Endpoint_Number 	= endpoint_Descriptor.bEndpointAddress & 0x7f;
			HID_Interface->interrupt_Out_Endpoint_Interval = endpoint_Descriptor.bInterval;
		}
	}

	USB_HID_Host___Get_HID_Descriptor(port_Number, device_Address, interface_Number, USB_HID_Host___HID_Descriptor_Base_Length, (uint8_t*)&(HID_Interface->HID_Descriptor), USB_HID_Host___URB_Setup_Callback);

	printf("interface %d is a HID interface\n", interface_Number);
}

void USB_HID_Host___Setup_HID_Device(uint8_t port_Number, uint8_t device_Address)
{
	uint8_t 										current_Configuration_Number	= USB_Host_Device_Manager___Get_Device_Current_Configuration_Number(port_Number, device_Address);
	if(current_Configuration_Number > 0)
	{
		USB_Host___Device_Descriptor_TypeDef			device_Descriptor				= USB_Host_Device_Manager___Device_Get_Device_Descriptor(port_Number, device_Address);
		USB_Host___Configuration_Descriptor_TypeDef 	configuration_Descriptor 		= USB_Host_Device_Manager___Device_Get_Configuration_Descriptor(port_Number, device_Address, current_Configuration_Number);

		if(device_Descriptor.bDeviceClass == USB_HID_Host___COMPOSITE_DEVICE_CLASS)
		{
			for(uint8_t i = 0; i < configuration_Descriptor.bNumInterfaces; i++)
			{
				USB_Host___Interface_Descriptor_TypeDef 	interface_Descriptor 		= USB_Host_Device_Manager___Device_Get_Interface_Descriptor(port_Number, device_Address, current_Configuration_Number, i);

				if(interface_Descriptor.bInterfaceClass == USB_HID_Host___HID_INTERFACE_CLASS)
				{
					USB_HID_Host___Setup_HID_Interface(port_Number, device_Address, current_Configuration_Number, i);
				}
			}
		}
	}
}

uint8_t USB_HID_Host___Register_Interface(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	USB_HID_Host___HID_Interface_Node_TypeDef* HID_Node = USB_HID_Host___Get_HID_Node_From_Device_Interface(port_Number, device_Address, interface_Number);
	if (HID_Node != NULL)
	{
		if (HID_Node->HID_Device.HID_Descriptor.interface_Registered == false)
		{
			HID_Node->HID_Device.HID_Descriptor.interface_Registered = true;
			return (EXIT_SUCCESS);
		}
		return (EXIT_SUCCESS);
	}
	return (EXIT_FAILURE);
}

void USB_HID_Host___Set_Interface_Report_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, void callback())
{
	// set callback
}

void USB_HID_Host___Device_Connected_Callback(uint8_t port_Number, uint8_t device_Address)
{
	if(USB_HID_Host___Is_Device_HID_Device(port_Number, device_Address))
	{
		printf("HID Device Connected!\n");
		USB_HID_Host___Setup_HID_Device(port_Number, device_Address);
	}
}

uint8_t USB_HID_Host___Add_Interface_Connected_Callback(uint8_t port_Number, void (*callback)(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number))
{
	uint8_t first_Free_Index = 0;
	uint8_t free_Index_Found = false;

	for (uint8_t i = 0; i < USB_HID_Host___MAX_APPLICATIONS; i++)
	{
		if (USB_HID_Host___Application_Callbacks[port_Number].HID_Interface_Connected_Callback[i] == NULL && !free_Index_Found)
		{
			free_Index_Found = true;
            first_Free_Index = i;
		}
		else if (USB_HID_Host___Application_Callbacks[port_Number].HID_Interface_Connected_Callback[i] == callback)
		{
			return (EXIT_SUCCESS);
		}
	}
	if (free_Index_Found)
	{
		USB_HID_Host___Application_Callbacks[port_Number].HID_Interface_Connected_Callback[first_Free_Index] = callback;
		return (EXIT_SUCCESS);
	}
	return (EXIT_FAILURE);
}

void USB_HID_Host___Init(uint8_t port_Number)
{
	USB_Host___Add_Device_Connected_Callback(port_Number, USB_HID_Host___Device_Connected_Callback);
}
