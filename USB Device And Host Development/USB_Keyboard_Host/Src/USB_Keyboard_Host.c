/*
 * USB_Keyboard_Host.c
 *
 *  Created on: Jan 4, 2025
 *      Author: jackh
 */

#include <stdint.h>					// Include C library for fixed-width integer types
#include <stm32f4xx.h>				// include MCU specific definitions
#include <stdio.h>

#include <USB_Host.h>
#include <USB_HID_Host.h>
#include "../Inc/USB_Keyboard_Host.h"



void USB_Keyboard_Host___HID_Interface_Connected_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	if ((USB_HID_Host___Interface_Get_Device_Type(port_Number, device_Address, interface_Number) == USB_HID_Host___DEVICE_TYPE_KEYBOARD) && (USB_HID_Host___Interface_Is_Boot_Mode_Supported(port_Number, device_Address, interface_Number) == 1))
	{
		printf("Keyboard connected\n");
	}
}

void USB_Keyboard_Host___HID_Interface_Disconnected_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t interface_Number)
{
	printf("Keyboard driver received interface disconnected\n");
}

void USB_Keyboard_Host___Init(uint8_t port_Number)
{
	USB_HID_Host___Init(port_Number);
	USB_HID_Host___Add_Interface_Connected_Callback(port_Number, USB_Keyboard_Host___HID_Interface_Connected_Callback);
	USB_HID_Host___Add_Interface_Disconnected_Callback(port_Number, USB_Keyboard_Host___HID_Interface_Disconnected_Callback);
}
