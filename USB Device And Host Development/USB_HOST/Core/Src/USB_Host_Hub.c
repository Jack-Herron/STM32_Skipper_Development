/*
 * USB_Host_Hub.cpp
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#include <stdint.h>
#include <stdlib.h>
#include "../Inc/USB_Host_Hub.h"
#include "../Inc/USB_Host_Device_Manager.h"
#include "../Inc/USB_Host_Transfers.h"

#if USB_Host_Config___DYNAMICALLY_ALLOCATE_USB_HUBS == true

	USB_Host_Hub___Hub_Node_TypeDef* USB_Host_Hub___Allocate_Hub_Node()
	{
		USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node;
		p_Hub_Node = (USB_Host_Hub___Hub_Node_TypeDef*)malloc(sizeof(USB_Host_Hub___Hub_Node_TypeDef));
		p_Hub_Node -> is_Allocated 	= 1;
		p_Hub_Node -> next_Node 	= NULL;
		p_Hub_Node -> previous_Node = NULL;
		return(p_Hub_Node);
	}

	void USB_Host_Hub___Free_Hub_Node(USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node)
	{
		free(p_Hub_Node);
	}

#else

	static USB_Host_Hub___Hub_Node_TypeDef USB_Host_Hub___Hub_Node_Pool[USB_Host_Config___MAX_USB_HUBS];

	USB_Host_Hub___Hub_Node_TypeDef* USB_Host_Hub___Allocate_Hub_Node()
	{
		for(uint32_t i = 0; i < USB_Host_Config___MAX_USB_HUBS; i++)
		{
			if(!USB_Host_Hub___Hub_Node_Pool[i].is_Allocated)
			{
				USB_Host_Hub___Hub_Node_Pool[i].is_Allocated 	= true;
				USB_Host_Hub___Hub_Node_Pool[i].next_Node 		= NULL;
				USB_Host_Hub___Hub_Node_Pool[i].previous_Node 	= NULL;
				return(&USB_Host_Hub___Hub_Node_Pool[i]);
			}
		}
		return(NULL);
	}

	void USB_Host_Hub___Free_Hub_Node(USB_Host_Hub___Hub_Node_TypeDef* p_Enumerator_Node)
	{
		p_Enumerator_Node -> is_Allocated = false;
	}

#endif

static USB_Host_Hub___Hub_Queue_TypeDef USB_Host_Hub___Hub_Queue[USB_Host___NUMBER_OF_PORTS];

USB_Host_Hub___Hub_Node_TypeDef* USB_Host_Hub___Create_Enumerator(uint8_t port_Number)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Allocate_Hub_Node();
	if(p_Hub_Node != NULL)
	{
		p_Hub_Node -> next_Node = NULL;
		p_Hub_Node -> previous_Node = NULL;

		if(USB_Host_Hub___Hub_Queue[port_Number].first_Node == NULL)
		{
			USB_Host_Hub___Hub_Queue[port_Number].first_Node = p_Hub_Node;
			USB_Host_Hub___Hub_Queue[port_Number].last_Node  = p_Hub_Node;
		}
		else
		{
			USB_Host_Hub___Hub_Queue[port_Number].last_Node -> next_Node 	= p_Hub_Node;
			USB_Host_Hub___Hub_Queue[port_Number].last_Node 				= p_Hub_Node;
			p_Hub_Node -> previous_Node 									= USB_Host_Hub___Hub_Queue[port_Number].last_Node;
		}
	}
	return(p_Hub_Node);
}

void USB_Host_Hub___Delete_Hub_Node(uint8_t port_Number, USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node)
{
	if(p_Hub_Node != NULL)
	{
		if (p_Hub_Node -> previous_Node != NULL)
		{
			p_Hub_Node -> previous_Node -> next_Node = p_Hub_Node -> next_Node;
			p_Hub_Node -> next_Node -> previous_Node = p_Hub_Node -> previous_Node;
		}
		else
		{
			USB_Host_Hub___Hub_Queue[port_Number].first_Node = p_Hub_Node -> next_Node;
		}

		USB_Host_Hub___Free_Hub_Node(p_Hub_Node);
	}
}

USB_Host_Hub___Hub_Node_TypeDef* USB_Host_Hub___Get_Hub_Node_From_Device_Address(uint8_t port_Number, uint8_t device_Address)
{
	if (USB_Host_Hub___Hub_Queue[port_Number].first_Node != NULL)
	{
		USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Hub_Queue[port_Number].first_Node;
		while (p_Hub_Node != NULL)
		{
			if (p_Hub_Node->hub.device_Address == device_Address)
			{
				return (p_Hub_Node);
			}
			p_Hub_Node = p_Hub_Node->next_Node;
		}
	}
	return(NULL);
}

void USB_Host_Hub___URB_Setup_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS);

void USB_Host_Hub___Set_Configuration(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet setup_Packet;

	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_HOST_TO_DEVICE;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_SET_CONFIGURATION;
	setup_Packet.wValue 		= configuration_Index;
	setup_Packet.wIndex 		= 0;
	setup_Packet.wLength 		= 0;

	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_OUT, setup_Packet, 0, 0, USB_Host_Hub___STANDARD_NUMBER_OF_RETRIES, callback);
}

void USB_Host_Hub___Get_Hub_Descriptor(uint8_t port_Number, uint8_t device_Address, uint8_t* p_Buffer, uint8_t descriptor_Length, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet setup_Packet;

	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_DEVICE_TO_HOST;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_DESCRIPTOR;
	setup_Packet.wValue 		= (USB_Host_Hub___HUB_DESCRIPTOR_TYPE << 8);
	setup_Packet.wIndex 		= 0;
	setup_Packet.wLength 		= descriptor_Length;

	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_IN, setup_Packet, p_Buffer, descriptor_Length, USB_Host_Hub___STANDARD_NUMBER_OF_RETRIES, callback);
}

void USB_Host_Hub___Set_Port_Feature(uint8_t port_Number, uint8_t device_Address, uint8_t hub_Port_Number, uint8_t main_Feature_Selector, uint8_t sub_Feature_Selector, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet setup_Packet;

	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_HOST_TO_OTHER;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_SET_FEATURE;
	setup_Packet.wValue 		= main_Feature_Selector;
	setup_Packet.wIndex 		= hub_Port_Number | (sub_Feature_Selector << 8);
	setup_Packet.wLength 		= 0;

	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_OUT, setup_Packet, 0, 0, USB_Host_Hub___STANDARD_NUMBER_OF_RETRIES, callback);
}

void USB_Host_Hub___Set_Next_Setup_Stage(USB_Host_Hub___Hub_Node_TypeDef* p_USB_Hub_Node)
{
	switch(p_USB_Hub_Node->hub.setup_Stage)
	{
	case USB_Host_Hub___HUB_SETUP_STAGE_SET_CONFIGURATION:
		p_USB_Hub_Node->hub.setup_Stage = USB_Host_Hub___HUB_SETUP_STAGE_GET_SHORT_HUB_DESCRIPTOR;
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_GET_SHORT_HUB_DESCRIPTOR:
		p_USB_Hub_Node->hub.setup_Stage = USB_Host_Hub___HUB_SETUP_STAGE_GET_FULL_HUB_DESCRIPTOR;
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_GET_FULL_HUB_DESCRIPTOR:
		p_USB_Hub_Node->hub.setup_Stage = USB_Host_Hub___HUB_SETUP_STAGE_ENABLE_PORTS;
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_ENABLE_PORTS:
		p_USB_Hub_Node->hub.setup_Stage = USB_Host_Hub___HUB_SETUP_STAGE_COMPLETE;
		break;
	}
}

void USB_Host_Hub___Interrupt_URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(URB.port_Number, URB.device_Address);

	if(p_Hub_Node != NULL)
	{
		if(URB.transfer_Status == USB_Host_Transfers___URB_STATUS_SUCCESS)
		{
			uint8_t i = 0;
		}
	}
}

void USB_Host_Hub___Polling_Callback(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(port_Number, device_Address);
	uint8_t odd_Frame = USB_Host___Get_Frame_Number(port_Number) %2;
	USB_Host_Transfers___Interrupt_Transfer(port_Number, device_Address, p_Hub_Node->hub.interrupt_Endpoint_Number, USB_Host_Transfers___URB_DIRECTION_IN, p_Hub_Node->hub.interrupt_Endpoint_Data_Buffer, p_Hub_Node->hub.interrupt_Endpoint_Packet_Size, 0, odd_Frame, 1, USB_Host_Hub___Interrupt_URB_Callback);
}

void USB_Host_Hub___Port_Enable_URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(URB.port_Number, URB.device_Address);

	if(p_Hub_Node != NULL)
	{

	}
}

void USB_Host_Hub___Do_Setup_Stage(USB_Host_Hub___Hub_Node_TypeDef* p_USB_Hub_Node)
{
	switch(p_USB_Hub_Node->hub.setup_Stage)
	{
	case USB_Host_Hub___HUB_SETUP_STAGE_SET_CONFIGURATION:
		USB_Host_Hub___Set_Configuration(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, 1, USB_Host_Hub___URB_Setup_Callback);
		USB_Host_Device_Manager___Set_Configuration(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, 0);
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_GET_SHORT_HUB_DESCRIPTOR:
		USB_Host_Hub___Get_Hub_Descriptor(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, (uint8_t*)&(p_USB_Hub_Node->hub.descriptor), USB_Host_Hub___HUB_DESCRIPTOR_BASE_LENGTH, USB_Host_Hub___URB_Setup_Callback);
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_GET_FULL_HUB_DESCRIPTOR:
		USB_Host_Hub___Get_Hub_Descriptor(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, (uint8_t*)&(p_USB_Hub_Node->hub.descriptor), p_USB_Hub_Node->hub.descriptor.bDescriptorLength, USB_Host_Hub___URB_Setup_Callback);
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_ENABLE_PORTS:
		for(uint8_t i = 0; i < p_USB_Hub_Node->hub.descriptor.bNumberOfPorts -1; i++)
		{
			USB_Host_Hub___Set_Port_Feature(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, i+1, USB_Host_Hub___FEATURE_PORT_POWER, 0, USB_Host_Hub___Port_Enable_URB_Callback);
		}

		USB_Host_Hub___Set_Port_Feature(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, p_USB_Hub_Node->hub.descriptor.bNumberOfPorts, USB_Host_Hub___FEATURE_PORT_POWER, 0, USB_Host_Hub___URB_Setup_Callback);
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_COMPLETE:
		USB_Host_Device_Manager___Add_Polling_Device(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, p_USB_Hub_Node->hub.polling_Interval, USB_Host_Hub___Polling_Callback);
		break;
	}
}

void USB_Host_Hub___URB_Setup_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(URB.port_Number, URB.device_Address);

	if(p_Hub_Node != NULL)
	{
		USB_Host_Hub___Set_Next_Setup_Stage(p_Hub_Node);
		USB_Host_Hub___Do_Setup_Stage(p_Hub_Node);
	}
}

void USB_Host_Hub___Initiate_Hub(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_USB_Hub_Node = USB_Host_Hub___Create_Enumerator(port_Number);
	if(p_USB_Hub_Node != NULL)
	{
		USB_Host___Endpoint_Descriptor_TypeDef* p_Endpoint_Descriptor 	= USB_Host_Device_Manager___Device_Get_Endpoint_Descriptor(port_Number, device_Address, 0, 0, 0);
		p_USB_Hub_Node->hub.polling_Interval 							= p_Endpoint_Descriptor->bInterval;
		p_USB_Hub_Node->hub.interrupt_Endpoint_Number 					= p_Endpoint_Descriptor->bEndpointAddress & 0x0f;
		p_USB_Hub_Node->hub.interrupt_Endpoint_Packet_Size 				= p_Endpoint_Descriptor->wMaxPacketSize;
		p_USB_Hub_Node->hub.setup_Stage 								= USB_Host_Hub___HUB_SETUP_STAGE_SET_CONFIGURATION;
		p_USB_Hub_Node->hub.device_Address 								= device_Address;
		p_USB_Hub_Node->hub.port_Number 								= port_Number;
		USB_Host_Hub___Do_Setup_Stage(p_USB_Hub_Node);
	}
}
