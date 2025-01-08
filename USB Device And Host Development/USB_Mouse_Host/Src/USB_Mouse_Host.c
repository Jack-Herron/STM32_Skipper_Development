/*
 * USB_Mouse_Host.c
 *
 *  Created on: Jan 4, 2025
 *      Author: jackh
 */

#include <stdint.h>					// Include C library for fixed-width integer types
#include <stm32f4xx.h>				// include MCU specific definitions
#include <stdio.h>

#include <USB_Host.h>
#include <USB_HID_Host.h>
#include "../Inc/USB_Mouse_Host.h"

void USB_Mouse_Host___Set_Protocol_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	printf("Mouse protocol set\n");
	USB_HID_Host___Start_Reporting(port_Number, device_Address, interface_Number);
}

void USB_Mouse_Host___Setup_Mouse(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	printf("Mouse connected\n");
	USB_HID_Host___Set_Protocol(port_Number, device_Address, interface_Number, USB_HID_Host___PROTOCOL_BOOT_MODE, USB_Mouse_Host___Set_Protocol_Callback);
}

void USB_Mouse_Host___HID_Interface_Connected_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	if ((USB_HID_Host___Interface_Get_Device_Type(port_Number, device_Address, interface_Number) == USB_HID_Host___DEVICE_TYPE_MOUSE) && (USB_HID_Host___Interface_Is_Boot_Mode_Supported(port_Number, device_Address, interface_Number) == 1))
	{
		USB_Mouse_Host___Setup_Mouse(port_Number, device_Address, interface_Number);
	}
}

void USB_Mouse_Host___HID_Interface_Disconnected_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	printf("Mouse driver received interface disconnected\n");
}

void USB_Mouse_Host___Init(uint8_t port_Number)
{
	USB_HID_Host___Init(port_Number);
	USB_HID_Host___Add_Interface_Connected_Callback(port_Number, USB_Mouse_Host___HID_Interface_Connected_Callback);
	USB_HID_Host___Add_Interface_Disconnected_Callback(port_Number, USB_Mouse_Host___HID_Interface_Disconnected_Callback);
}
