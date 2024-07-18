/*
 * USB_Host_Enumerate.cpp
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#include "../Inc/USB_Host_Enumerate.h"
#include "../Inc/USB_Host_Device_Manager.h"
#include "../Inc/USB_Host_Transfers.h"

void USB_Host_Enumerate___URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{

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

void USB_Host_Enumerate___Do_Setup_Stage(uint8_t port_Number, uint8_t device_Address, uint8_t setup_Stage)
{
	switch(setup_Stage)
	{
	case USB_Host_Enumerate___SETUP_STAGE_IDLE:

		break;

	case USB_Host_Enumerate___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR:
		USB_Host_Enumerate___Get_First_Eight_Device_Descriptor(port_Number, device_Address);
		break;

	case USB_Host_Enumerate___SETUP_STAGE_SET_ADDRESS:

		break;

	case USB_Host_Enumerate___SETUP_STAGE_GET_FULL_DEVICE_DESCRIPTOR:

		break;

	case USB_Host_Enumerate___SETUP_STAGE_GET_SHORT_STRING_DESCRIPTOR:

		break;

	case USB_Host_Enumerate___SETUP_STAGE_GET_FULL_STRING_DESCRIPTOR:

		break;

	case USB_Host_Enumerate___SETUP_STAGE_GET_SHORT_CONFIGURATION_DESCRIPTOR:

		break;

	case USB_Host_Enumerate___SETUP_STAGE_GET_FULL_CONFIGURATION_DESCRIPTOR:

		break;

	case USB_Host_Enumerate___SETUP_STAGE_COMPLETE:

		break;
	}
}

void USB_Host_Enumerate___Enumerate_Device(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_Set_Setup_Stage(port_Number, device_Address, USB_Host_Enumerate___SETUP_STAGE_IDLE);
	USB_Host_Device_Manager___Device_Set_In_Endpoint_Max_Packet_Size(port_Number, device_Address, 0, USB_Host_Enumerate___DEFAULT_MAX_PACKET_SIZE);
	USB_Host_Device_Manager___Device_Set_Out_Endpoint_Max_Packet_Size(port_Number, device_Address, 0, USB_Host_Enumerate___DEFAULT_MAX_PACKET_SIZE);
	USB_Host_Enumerate___Do_Setup_Stage(port_Number, device_Address, USB_Host_Enumerate___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR);
}
