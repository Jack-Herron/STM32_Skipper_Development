/*
 * USB_Device.c
 *
 *  Created on: Jan 27, 2025
 *      Author: Jack Herron
 */



#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>
#include <stdio.h>

#include "../Inc/USB_Device.h"
#include <USB_LL.h>
#include <USB_LL_Device.h>

void USB_Device___RX_Callback(USB_LL_Device___RX_CALLBACK_PARAMETERS);

void USB_Device___Init(uint8_t port_Number)
{
	USB_LL_Device___FIFO_Config_TypeDef FIFO_Config = {0};

	FIFO_Config.RX_FIFO_Depth 				= USB_Device___DEFAULT_RX_FIFO_SIZE;
	FIFO_Config.Endpoint_TX_FIFO_Depth[0] 	= USB_Device___DEFAULT_TX_FIFO_SIZE;

	USB_LL___GPIO_Init(port_Number);
	USB_LL___Init(port_Number, USB_LL___DEVICE_MODE);
	USB_LL_Device___Init(port_Number);
	USB_LL_Device___Set_FIFO_Size(port_Number, FIFO_Config);
	USB_LL_Device___Set_RX_Callback(port_Number, USB_Device___RX_Callback);
}

void USB_Device___RX_Callback(USB_LL_Device___RX_CALLBACK_PARAMETERS)
{
	if (packet_Type == USB_LL_Device___PACKET_TYPE_SETUP)
	{
		printf("Setup packet received: ");

		for(uint16_t i = 0; i < length; i++)
		{
			printf("%2.2x ", data[i]);
		}

		printf("\n");
	}
	else if (packet_Type == USB_LL_Device___PACKET_TYPE_DATA)
	{
		printf("Data packet received: ");

		for(uint16_t i = 0; i < length; i++)
		{
			printf("%2.2x ", data[i]);
		}

		printf("\n");
	}
}
