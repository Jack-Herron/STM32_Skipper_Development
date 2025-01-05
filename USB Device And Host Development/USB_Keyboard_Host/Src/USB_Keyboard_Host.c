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


void USB_Keyboard_Host___Composite_Device_Connected_Callback(uint8_t port_Number, uint8_t device_Address)
{
	if(USB_HID_Host___Is_Device_HID_Device(port_Number, device_Address))
	{
		printf("HID Device Connected!\n");
		USB_HID_Host___Setup_HID_Device(port_Number, device_Address);
	}
}

void USB_Keyboard_Host___Init(uint8_t port_Number)
{
	USB_Host___Add_Device_Connected_Callback(port_Number, USB_Keyboard_Host___Composite_Device_Connected_Callback);
}
