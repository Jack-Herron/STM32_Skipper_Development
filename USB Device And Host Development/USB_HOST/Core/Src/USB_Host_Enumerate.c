/*
 * USB_Host_Enumerate.cpp
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */


#include <stdlib.h>
#include "../Inc/USB_Host_Enumerate.h"
#include "../Inc/USB_Host_Device_Manager.h"
#include "../Inc/USB_Host_Transfers.h"

#if USB_Host_Config___DYNAMICALLY_ALLOCATE_ENUMERATORS == true

USB_Host_Enumerate___Enumerator_Node_TypeDef* USB_Host_Enumerate___Allocate_Enumerator_Node()
	{
		USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node;
		p_Enumerator_Node = (USB_Host_Enumerate___Enumerator_Node_TypeDef*)malloc(sizeof(USB_Host_Enumerate___Enumerator_Node_TypeDef));
		return(p_Enumerator_Node);
	}

	void USB_Host_Enumerate___Free_Enumerator_Node(USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node)
	{
		free(p_Enumerator_Node);
	}

#else

	static USB_Host_Enumerate___Enumerator_Node_TypeDef USB_Host_Enumerate___Enumerator_Node_Pool[USB_Host_Config___MAX_USB_ENUMERATORS];

	USB_Host_Enumerate___Enumerator_Node_TypeDef* USB_Host_Enumerate___Allocate_Enumerator_Node()
	{
		for(uint32_t i = 0; i < USB_Host_Config___MAX_USB_ENUMERATORS; i++)
		{
			if(!USB_Host_Enumerate___Enumerator_Node_Pool[i].is_Allocated)
			{
				USB_Host_Enumerate___Enumerator_Node_Pool[i].is_Allocated = true;
				return(&USB_Host_Enumerate___Enumerator_Node_Pool[i]);
			}
		}
		return(NULL);
	}

	void USB_Host_Enumerate___Free_Enumerator_Node(USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node)
	{
		p_Enumerator_Node -> is_Allocated = false;
	}

#endif

static USB_Host_Enumerate___Enumerator_Queue_TypeDef USB_Host_Enumerate___Enumerator_Queue[USB_Host___NUMBER_OF_PORTS];

USB_Host_Enumerate___Enumerator_Node_TypeDef* USB_Host_Enumerate___Create_Enumerator(uint8_t port_Number)
{
	USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node = USB_Host_Enumerate___Allocate_Enumerator_Node();
	if(p_Enumerator_Node != NULL)
	{
		p_Enumerator_Node -> next_Node = NULL;
		p_Enumerator_Node -> previous_Node = NULL;

		if(USB_Host_Enumerate___Enumerator_Queue[port_Number].first_Node == NULL)
		{
			USB_Host_Enumerate___Enumerator_Queue[port_Number].first_Node = p_Enumerator_Node;
			USB_Host_Enumerate___Enumerator_Queue[port_Number].last_Node  = p_Enumerator_Node;
		}
		else
		{
			USB_Host_Enumerate___Enumerator_Queue[port_Number].last_Node -> next_Node 	= p_Enumerator_Node;
			USB_Host_Enumerate___Enumerator_Queue[port_Number].last_Node 				= p_Enumerator_Node;
			p_Enumerator_Node -> previous_Node 											= USB_Host_Enumerate___Enumerator_Queue[port_Number].last_Node;
		}
	}
	return(p_Enumerator_Node);
}

void USB_Host_Enumerate___Delete_Enumerator_Node(uint8_t port_Number, USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node)
{
	if(p_Enumerator_Node != NULL)
	{
		if (p_Enumerator_Node -> previous_Node != NULL)
		{
			p_Enumerator_Node -> previous_Node -> next_Node = p_Enumerator_Node -> next_Node;
			p_Enumerator_Node -> next_Node -> previous_Node = p_Enumerator_Node -> previous_Node;
		}
		else
		{
			USB_Host_Enumerate___Enumerator_Queue[port_Number].first_Node = p_Enumerator_Node -> next_Node;
		}

		USB_Host_Enumerate___Free_Enumerator_Node(p_Enumerator_Node);
	}
}

USB_Host_Enumerate___Enumerator_Node_TypeDef* USB_Host_Tranfers___Get_first_Enumerator_Node(uint8_t port_Number)
{
	if(USB_Host_Enumerate___Enumerator_Queue[port_Number].first_Node != NULL)
	{
		return(USB_Host_Enumerate___Enumerator_Queue[port_Number].first_Node);
	}
	return(NULL);
}

USB_Host_Enumerate___Enumerator_Node_TypeDef* USB_Host_Enumerate___Get_Enumerator_Node_From_Device_Address(uint8_t port_Number, uint8_t device_Address)
{
	if (USB_Host_Enumerate___Enumerator_Queue[port_Number].first_Node != NULL)
	{
		USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node = USB_Host_Enumerate___Enumerator_Queue[port_Number].first_Node;
		while (p_Enumerator_Node != NULL)
		{
			if (p_Enumerator_Node->enumerator.current_USB_Device_Address == device_Address)
			{
				return (p_Enumerator_Node);
			}
			p_Enumerator_Node = p_Enumerator_Node->next_Node;
		}
	}
	return(NULL);
}

void USB_Host_Enumerate___Set_Current_USB_Address_To_Pending_USB_Address(USB_Host_Enumerate___Enumerator_Node_TypeDef *p_Enumerator_Node)
{
	p_Enumerator_Node->enumerator.current_USB_Device_Address = p_Enumerator_Node->enumerator.pending_USB_Device_Address;
}

void USB_Host_Enumerate___Set_Pending_USB_Device_Address(USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node, uint8_t pending_Address)
{
	p_Enumerator_Node->enumerator.pending_USB_Device_Address = pending_Address;
}

void USB_Host_Enumerate___Set_Setup_Stage(USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node, uint8_t setup_Stage)
{
	p_Enumerator_Node -> enumerator.setup_Stage = setup_Stage;
}

uint8_t USB_Host_Enumerate___Device_Get_Setup_Stage(USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node)
{
	return (p_Enumerator_Node->enumerator.setup_Stage);
}

void USB_Host_Enumerate___Do_Setup_Stage(USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node);

uint8_t USB_Host_Enumerate___Get_Next_String_Descriptor_Type(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host___Device_Descriptor_TypeDef device_Descriptor = USB_Host_Device_Manager___Device_Get_Device_Descriptor(port_Number, device_Address);

	if(
	(	(device_Descriptor.iManufacturer 	!= 0) 	||
		(device_Descriptor.iProduct 		!= 0) 	||
		(device_Descriptor.iSerialNumber 	!= 0))	&&
		USB_Host_Device_Manager___Get_Language_ID_List_Length(port_Number, device_Address) < 0
	)
	{
		return(USB_Host_Enumerate___STRING_TYPE_LANGUAGE);
	}
	else if (
		device_Descriptor.iManufacturer != 0 	&&
		USB_Host_Device_Manager___Get_Manufacturer_String_Length(port_Number, device_Address) < 0
	)
	{
		return USB_Host_Enumerate___STRING_TYPE_MANUFACTURER;
	}
	else if (
		device_Descriptor.iProduct != 0 &&
		USB_Host_Device_Manager___Get_Product_String_Length(port_Number, device_Address) < 0
	)
	{
		return (USB_Host_Enumerate___STRING_TYPE_PRODUCT);
	}
	else if (
		device_Descriptor.iSerialNumber != 0 &&
		USB_Host_Device_Manager___Get_Serial_Number_String_Length(port_Number, device_Address) < 0
	)
	{
		return (USB_Host_Enumerate___STRING_TYPE_SERIAL_NUMBER);
	}
	else
	{
		return 0;
	}
}

void USB_Host_Enumerate___Set_Next_Setup_Stage(USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node)
{
	uint8_t port_Number = p_Enumerator_Node -> enumerator.port_Number;
	uint8_t device_Address = p_Enumerator_Node -> enumerator.current_USB_Device_Address;

	switch (p_Enumerator_Node -> enumerator.setup_Stage)
	{
	case USB_Host_Device_Manager___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR:
	{
		USB_Host_Enumerate___Set_Setup_Stage(p_Enumerator_Node, USB_Host_Device_Manager___SETUP_STAGE_SET_ADDRESS);
		break;
	}
	case USB_Host_Device_Manager___SETUP_STAGE_SET_ADDRESS:
	{
		USB_Host_Enumerate___Set_Setup_Stage(p_Enumerator_Node, USB_Host_Device_Manager___SETUP_STAGE_GET_FULL_DEVICE_DESCRIPTOR);
		break;
	}
	case USB_Host_Device_Manager___SETUP_STAGE_GET_FULL_DEVICE_DESCRIPTOR:
	{
		uint8_t next_String_Type = USB_Host_Enumerate___Get_Next_String_Descriptor_Type(port_Number, device_Address);
		if(next_String_Type != 0)
		{
			USB_Host_Enumerate___Set_Setup_Stage(p_Enumerator_Node, USB_Host_Device_Manager___SETUP_STAGE_GET_SHORT_STRING_DESCRIPTOR);
		}
		else
		{
			USB_Host_Enumerate___Set_Setup_Stage(p_Enumerator_Node, USB_Host_Device_Manager___SETUP_STAGE_GET_SHORT_CONFIGURATION_DESCRIPTOR);
		}
		break;
	}
	}
}

void USB_Host_Enumerate___Setup_Stage_Completed(USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node)
{
	uint8_t port_Number = p_Enumerator_Node -> enumerator.port_Number;
	uint8_t device_Address = p_Enumerator_Node -> enumerator.current_USB_Device_Address;

	switch (p_Enumerator_Node -> enumerator.setup_Stage)
	{
	case USB_Host_Device_Manager___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR:
	{
		USB_Host___Device_Descriptor_TypeDef device_Descriptor = USB_Host_Device_Manager___Device_Get_Device_Descriptor(port_Number, device_Address);
		uint16_t endpoint_Zero_Max_Packet_Size = device_Descriptor.bMaxPacketSize;
		USB_Host_Device_Manager___Device_Set_Out_Endpoint_Max_Packet_Size(port_Number, device_Address, 0, endpoint_Zero_Max_Packet_Size);
		USB_Host_Device_Manager___Device_Set_In_Endpoint_Max_Packet_Size(port_Number, device_Address, 0, endpoint_Zero_Max_Packet_Size);
		break;
	}
	case USB_Host_Device_Manager___SETUP_STAGE_SET_ADDRESS:
		USB_Host_Device_Manager___Device_Update_Current_USB_Address(port_Number, device_Address);
		USB_Host_Enumerate___Set_Current_USB_Address_To_Pending_USB_Address(p_Enumerator_Node);
		break;
	case USB_Host_Device_Manager___SETUP_STAGE_GET_FULL_DEVICE_DESCRIPTOR:

		break;

	}
}

void USB_Host_Enumerate___URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node = USB_Host_Enumerate___Get_Enumerator_Node_From_Device_Address(URB.port_Number, URB.device_Address);

	if(p_Enumerator_Node != NULL)
	{
		USB_Host_Enumerate___Setup_Stage_Completed(p_Enumerator_Node);
		USB_Host_Enumerate___Set_Next_Setup_Stage(p_Enumerator_Node);
		USB_Host_Enumerate___Do_Setup_Stage(p_Enumerator_Node);
	}
}

void USB_Host_Enumerate___Get_Full_Device_Descriptor(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Transfers___Control_Setup_Packet setup_Packet;
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_DEVICE_TO_HOST;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_DESCRIPTOR;
	setup_Packet.wValue 		= 0x0100;
	setup_Packet.wIndex 		= 0x0000;
	setup_Packet.wLength 		= 0x0012;

	uint8_t* p_Buffer = USB_Host_Device_Manager___Get_Device_Descriptor_Buffer(port_Number, device_Address);

	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_IN, setup_Packet, p_Buffer, 0x12, USB_Host_Enumerate___URB_Callback);
}

void USB_Host_Enumerate___Get_First_Eight_Device_Descriptor(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Transfers___Control_Setup_Packet setup_Packet;
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_DEVICE_TO_HOST;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_DESCRIPTOR;
	setup_Packet.wValue 		= 0x0100;
	setup_Packet.wIndex 		= 0x0000;
	setup_Packet.wLength 		= 0x08;

	uint8_t* p_Buffer = USB_Host_Device_Manager___Get_Device_Descriptor_Buffer(port_Number, device_Address);

	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_IN, setup_Packet, p_Buffer, 0x08, USB_Host_Enumerate___URB_Callback);
}

void USB_Host_Enumerate___Set_Address(uint8_t port_Number, uint8_t device_Address, uint8_t new_Address)
{
	USB_Host_Transfers___Control_Setup_Packet setup_Packet;
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_HOST_TO_DEVICE;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_SET_ADDRESS;
	setup_Packet.wValue 		= new_Address;
	setup_Packet.wIndex 		= 0x0000;
	setup_Packet.wLength 		= 0x0000;

	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_OUT, setup_Packet, 0, 0, USB_Host_Enumerate___URB_Callback);
}

void USB_Host_Enumerate___Do_Setup_Stage(USB_Host_Enumerate___Enumerator_Node_TypeDef* p_Enumerator_Node)
{
	uint8_t port_Number = p_Enumerator_Node -> enumerator.port_Number;
	uint8_t device_Address = p_Enumerator_Node -> enumerator.current_USB_Device_Address;

	switch(p_Enumerator_Node -> enumerator.setup_Stage)
	{
	case USB_Host_Device_Manager___SETUP_STAGE_IDLE:

		break;

	case USB_Host_Device_Manager___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR:
		USB_Host_Enumerate___Get_First_Eight_Device_Descriptor(port_Number, device_Address);
		break;

	case USB_Host_Device_Manager___SETUP_STAGE_SET_ADDRESS:
		uint8_t pending_Address = USB_Host_Device_Manager___Reserve_New_Device_Address(port_Number, device_Address);
		USB_Host_Enumerate___Set_Pending_USB_Device_Address(p_Enumerator_Node, pending_Address);
		USB_Host_Enumerate___Set_Address(port_Number, device_Address, pending_Address);
		break;

	case USB_Host_Device_Manager___SETUP_STAGE_GET_FULL_DEVICE_DESCRIPTOR:
		USB_Host_Enumerate___Get_Full_Device_Descriptor(port_Number, device_Address);
		break;

	case USB_Host_Device_Manager___SETUP_STAGE_GET_SHORT_STRING_DESCRIPTOR:

		break;

	case USB_Host_Device_Manager___SETUP_STAGE_GET_FULL_STRING_DESCRIPTOR:

		break;

	case USB_Host_Device_Manager___SETUP_STAGE_GET_SHORT_CONFIGURATION_DESCRIPTOR:

		break;

	case USB_Host_Device_Manager___SETUP_STAGE_GET_FULL_CONFIGURATION_DESCRIPTOR:

		break;

	case USB_Host_Device_Manager___SETUP_STAGE_COMPLETE:

		break;
	}
}

uint8_t USB_Host_Enumerate___Enumerate_Device(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Enumerate___Enumerator_Node_TypeDef* Enumerator_Node = USB_Host_Enumerate___Create_Enumerator(port_Number);
	if (Enumerator_Node != NULL)
	{
		USB_Host_Enumerate___Enumerator_TypeDef new_Enumerator;
		new_Enumerator.is_Allocated = true;
		new_Enumerator.port_Number = port_Number;
		new_Enumerator.current_USB_Device_Address = device_Address;
		new_Enumerator.current_String_Descriptor_Type = 0;
		new_Enumerator.is_Busy = 0;
		new_Enumerator.setup_Stage = USB_Host_Device_Manager___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR;
		Enumerator_Node->enumerator = new_Enumerator;

		USB_Host_Device_Manager___Device_Set_In_Endpoint_Max_Packet_Size(port_Number, device_Address, 0, USB_Host_Enumerate___DEFAULT_MAX_PACKET_SIZE);
		USB_Host_Device_Manager___Device_Set_Out_Endpoint_Max_Packet_Size(port_Number, device_Address, 0, USB_Host_Enumerate___DEFAULT_MAX_PACKET_SIZE);

		USB_Host_Enumerate___Do_Setup_Stage(Enumerator_Node);

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}
