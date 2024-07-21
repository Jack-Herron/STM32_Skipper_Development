/*
 * USB_Host_Enumerate.cpp
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#include "../Inc/USB_Host_Enumerate.h"
#include "../Inc/USB_Host_Device_Manager.h"
#include "../Inc/USB_Host_Transfers.h"

void USB_Host_Enumerate___Do_Setup_Stage(uint8_t port_Number, uint8_t device_Address);

void USB_Host_Enumerate___Set_Next_Setup_Stage(uint8_t port_Number, uint8_t device_Address)
{
	switch (USB_Host_Device_Manager___Device_Get_Setup_Stage(port_Number, device_Address))
	{
	case USB_Host_Device_Manager___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR:
	{
		USB_Host_Device_Manager___Device_Set_Setup_Stage(port_Number, device_Address, USB_Host_Device_Manager___SETUP_STAGE_SET_ADDRESS);
		break;
	}
	case USB_Host_Device_Manager___SETUP_STAGE_SET_ADDRESS:
	{
		USB_Host_Device_Manager___Device_Set_Setup_Stage(port_Number, device_Address, USB_Host_Device_Manager___SETUP_STAGE_GET_FULL_DEVICE_DESCRIPTOR);
		break;
	}
	}
}

void USB_Host_Enumerate___Setup_Stage_Completed(uint8_t port_Number, uint8_t device_Address)
{
	switch (USB_Host_Device_Manager___Device_Get_Setup_Stage(port_Number, device_Address))
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
		break;
	case USB_Host_Device_Manager___SETUP_STAGE_GET_FULL_DEVICE_DESCRIPTOR:

		break;

	}
}

void USB_Host_Enumerate___URB_Set_Address_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	USB_Host_Device_Manager___Port_remove_Device_From_Address(URB.port_Number, URB.device_Address);
	USB_Host_Enumerate___Setup_Stage_Completed(URB.port_Number, URB.control_Setup_Packet.wValue);
	USB_Host_Enumerate___Set_Next_Setup_Stage(URB.port_Number, URB.control_Setup_Packet.wValue);
	USB_Host_Enumerate___Do_Setup_Stage(URB.port_Number,URB.control_Setup_Packet.wValue);

}

void USB_Host_Enumerate___URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	USB_Host_Enumerate___Setup_Stage_Completed(URB.port_Number, URB.device_Address);
	USB_Host_Enumerate___Set_Next_Setup_Stage(URB.port_Number, URB.device_Address);
	USB_Host_Enumerate___Do_Setup_Stage(URB.port_Number, URB.device_Address);

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

	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_OUT, setup_Packet, 0, 0, USB_Host_Enumerate___URB_Set_Address_Callback);
}

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

void USB_Host_Enumerate___Do_Setup_Stage(uint8_t port_Number, uint8_t device_Address)
{
	switch(USB_Host_Device_Manager___Device_Get_Setup_Stage(port_Number, device_Address))
	{
	case USB_Host_Device_Manager___SETUP_STAGE_IDLE:

		break;

	case USB_Host_Device_Manager___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR:
		USB_Host_Enumerate___Get_First_Eight_Device_Descriptor(port_Number, device_Address);
		break;

	case USB_Host_Device_Manager___SETUP_STAGE_SET_ADDRESS:
		uint8_t new_Address = USB_Host_Device_Manager___Reserve_New_Device_Address(port_Number, device_Address);
		USB_Host_Enumerate___Set_Address(port_Number, device_Address, new_Address);
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

void USB_Host_Enumerate___Enumerate_Device(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_Set_Setup_Stage(port_Number, device_Address, USB_Host_Device_Manager___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR);
	USB_Host_Device_Manager___Device_Set_In_Endpoint_Max_Packet_Size(port_Number, device_Address, 0, USB_Host_Enumerate___DEFAULT_MAX_PACKET_SIZE);
	USB_Host_Device_Manager___Device_Set_Out_Endpoint_Max_Packet_Size(port_Number, device_Address, 0, USB_Host_Enumerate___DEFAULT_MAX_PACKET_SIZE);
	USB_Host_Enumerate___Do_Setup_Stage(port_Number, device_Address);
}
