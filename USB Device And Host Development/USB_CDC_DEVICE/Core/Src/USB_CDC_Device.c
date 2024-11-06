/*
 * USB_CDC_Device.c
 *
 *  Created on: Nov 5, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>

#include "../Inc/USB_CDC_Device.h"
#include <USB_LL_Hardware.h>

void USB_CDC_Device___Init(uint8_t port_Number)
{
	USB_LL_Hardware___Init(port_Number, USB_LL_Hardware___DEVICE_MODE);
}
