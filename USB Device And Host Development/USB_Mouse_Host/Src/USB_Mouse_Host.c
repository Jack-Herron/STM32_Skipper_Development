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

void USB_Mouse_Host___Report_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number, uint8_t *report, uint16_t report_Length)
{
	uint8_t button = report[0];
	int8_t x = report[1];
	int8_t y = report[2];

	printf("Button: %x, X: %d, Y: %d\n", button, x, y);
}

void USB_Mouse_Host___Set_Protocol_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	USB_HID_Host___Start_Reporting(port_Number, device_Address, interface_Number, USB_Mouse_Host___Report_Callback);
}

void USB_Mouse_Host___Setup_Mouse(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	USB_HID_Host___Set_Protocol(port_Number, device_Address, interface_Number, USB_HID_Host___PROTOCOL_BOOT_MODE, USB_Mouse_Host___Set_Protocol_Callback);
}

void USB_Mouse_Host___HID_Interface_Connected_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	if ((USB_HID_Host___Interface_Get_Device_Type(port_Number, device_Address, interface_Number) == USB_HID_Host___DEVICE_TYPE_MOUSE) && (USB_HID_Host___Interface_Is_Boot_Mode_Supported(port_Number, device_Address, interface_Number) == 1))
	{
		USB_HID_Host___Register_Interface(port_Number, device_Address, interface_Number);
		USB_Mouse_Host___Setup_Mouse(port_Number, device_Address, interface_Number);
	}
}



void USB_Mouse_Host___HID_Interface_Disconnected_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{

}

void USB_Mouse_Host___Init(uint8_t port_Number)
{
	USB_HID_Host___Init(port_Number);
	USB_HID_Host___Add_Interface_Connected_Callback(port_Number, USB_Mouse_Host___HID_Interface_Connected_Callback);
	USB_HID_Host___Add_Interface_Disconnected_Callback(port_Number, USB_Mouse_Host___HID_Interface_Disconnected_Callback);
}
