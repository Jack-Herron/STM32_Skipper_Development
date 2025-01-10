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
#include <USB_LL.h>
#include <USB_LL_Device.h>

void USB_CDC_Device___RX_Callback(USB_LL_Device___RX_CALLBACK_PARAMETERS);

void USB_CDC_Device___Init(uint8_t port_Number)
{
	USB_LL_Device___FIFO_Config_TypeDef FIFO_Config = {0};

	FIFO_Config.RX_FIFO_Depth = 0x80;
	FIFO_Config.Endpoint_TX_FIFO_Depth[0] = 0x80;
	FIFO_Config.Endpoint_TX_FIFO_Depth[1] = 0x80;
	FIFO_Config.Endpoint_TX_FIFO_Depth[2] = 0x80;

	USB_LL___GPIO_Init(port_Number);
	USB_LL___Init(port_Number, USB_LL___DEVICE_MODE);
	USB_LL_Device___Set_FIFO_Size(port_Number, FIFO_Config);
	USB_LL_Device___Set_RX_Callback(port_Number, USB_CDC_Device___RX_Callback);
}

void USB_CDC_Device___RX_Callback(USB_LL_Device___RX_CALLBACK_PARAMETERS)
{

}
