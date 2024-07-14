/*
 * USB_Host_Enumerate.cpp
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#include "../Inc/USB_Host_Enumerate.h"
#include "../Inc/USB_Host_Device_Manager.h"

void USB_Host_Enumerate___Do_Setup_Stage(uint8_t port_Number, uint8_t device_Address, uint8_t setup_Stage)
{
	switch(setup_Stage)
	{
	case USB_Host_Enumerate___SETUP_STAGE_IDLE:

		break;
	case USB_Host_Enumerate___SETUP_STAGE_GET_FIRST_EIGHT_DEVICE_DESCRIPTOR:

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
