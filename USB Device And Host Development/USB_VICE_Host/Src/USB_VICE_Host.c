/*
 * USB_VICE_Host.c
 *
 *  Created on: Dec 25, 2024
 *      Author: jackh
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>
#include <stdio.h>
#include "../Inc/USB_VICE_Host.h"

#include <USB_Host.h>
#include <USB_Host_Device_Manager.h>
#include <USB_Host_Transfers.h>

uint8_t USB_VICE_Host___Is_Device_VICE_Device(uint8_t port_Number, uint8_t device_Address)
{
	uint8_t device_Class = USB_Host_Device_Manager___Get_Device_Class(port_Number, device_Address);

}

void USB_VICE_Host___Setup_Device(uint8_t port_Number, uint8_t device_Address)
{

}
