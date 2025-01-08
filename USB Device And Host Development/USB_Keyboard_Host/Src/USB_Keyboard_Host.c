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
	printf("Keyboard driver received interface connected\n");
}

void USB_Keyboard_Host___Init(uint8_t port_Number)
{
	USB_HID_Host___Init(port_Number);
	USB_HID_Host___Add_Interface_Connected_Callback(port_Number, USB_Keyboard_Host___HID_Interface_Connected_Callback);
}
