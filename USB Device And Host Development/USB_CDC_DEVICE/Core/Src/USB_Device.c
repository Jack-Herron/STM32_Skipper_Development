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
#include "../Inc/USB_Device_Descriptors.h"
#include <USB_LL.h>
#include <USB_LL_Device.h>

void USB_Device___EP0_RX_Callback(USB_LL_Device___RX_CALLBACK_PARAMETERS);
void USB_Device___EP0_TX_Callback( USB_LL_Device___TX_CALLBACK_PARAMETERS);

uint8_t test_Buffer[64];

struct USB_Device___Control_Transfer USB_Device___Control_Transfer[USB_Device___NUM_PORTS][USB_Device___NUM_ENDPOINTS];



void USB_Device___Init(uint8_t port_Number)
{
	USB_LL_Device___FIFO_Config_TypeDef FIFO_Config = {0};

	FIFO_Config.RX_FIFO_Depth 				= USB_Device___DEFAULT_RX_FIFO_SIZE;
	FIFO_Config.Endpoint_TX_FIFO_Depth[0] 	= USB_Device___DEFAULT_TX_FIFO_SIZE;

	USB_LL___GPIO_Init(port_Number);
	USB_LL___Init(port_Number, USB_LL___DEVICE_MODE);
	USB_LL_Device___Set_FIFO_Size(port_Number, FIFO_Config);

	USB_LL_Device___Init(port_Number);
	USB_LL_Device___Setup_Endpoint(port_Number, 0, USB_LL_Device___ENDPOINT_DERECTION_OUT, USB_LL_Device___ENDPOINT_TYPE_CONTROL, 64);
	USB_LL_Device___Setup_Endpoint(port_Number, 0, USB_LL_Device___ENDPOINT_DERECTION_IN, USB_LL_Device___ENDPOINT_TYPE_CONTROL, 64);
	USB_LL_Device___Endpoint_Transfer_Out(port_Number, 0, 0, test_Buffer, 64);
	USB_LL_Device___Set_RX_Callback(port_Number, 0, USB_Device___EP0_RX_Callback);
	USB_LL_Device___Set_TX_Callback(port_Number, 0, USB_Device___EP0_TX_Callback);
}

void USB_Device___Set_Control_Transfer_Callback(uint8_t port_Number, uint8_t endpoint_Number, void callback(USB_Device___CONTROL_TRANSFER_CALLBACK_PARAMETERS))
{
	USB_Device___Control_Transfer[port_Number][endpoint_Number].callback = callback;
}

void USB_Device___Handle_Control_Transfer(uint8_t port_Number, uint8_t endpoint_Number)
{
	struct USB_Device___Control_Transfer control_Transfer = USB_Device___Control_Transfer[port_Number][endpoint_Number];

	if (!(control_Transfer.Setup_Packet.bmRequestType & 0x80) && control_Transfer.Setup_Packet.wLength > 0 && control_Transfer.data == NULL)
	{
		USB_LL_Device___Endpoint_Transfer_Out(port_Number, 0, 0, test_Buffer, 64);
	}
	else
	{
		if(control_Transfer.Setup_Packet.bRequest == USB_Device___bRequest_GET_DESCRIPTOR)
		{
			USB_Device_Descriptors___Entry_TypeDef* descriptor = USB_Device_Descriptors___Get_Descriptor(port_Number, control_Transfer.Setup_Packet.wValue >> 8, control_Transfer.Setup_Packet.wValue & 0xFF);

			if (descriptor != NULL)
			{
				uint16_t length = USB_Device___GET_MIN(control_Transfer.Setup_Packet.wLength, descriptor->descriptor_Size));
				USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, descriptor->descriptor, length);
			}
		}
		else if(control_Transfer.Setup_Packet.bRequest == USB_Device___bRequest_SET_ADDRESS)
		{
			USB_LL_Device___Set_Address(port_Number, control_Transfer.Setup_Packet.wValue);
			USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, NULL, 0);
		}
		else if(control_Transfer.Setup_Packet.bRequest == USB_Device___bRequest_GET_STATUS)
		{
			uint8_t status[] = {0,0};
			USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, status, 2);
		}
		else if(control_Transfer.Setup_Packet.bRequest == USB_Device___bRequest_SET_CONFIGURATION)
		{
			USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, NULL, 0);
		}
		else if(control_Transfer.Setup_Packet.bRequest == USB_Device___bRequest_SET_INTERFACE)
		{
			USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, NULL, 0);
		}
		else if(control_Transfer.callback != NULL)
		{
			control_Transfer.callback(port_Number, endpoint_Number, control_Transfer.Setup_Packet, control_Transfer.data, control_Transfer.data_Size);
		}
	}
}

void USB_Device___EP0_TX_Callback( USB_LL_Device___TX_CALLBACK_PARAMETERS)
{
	USB_LL_Device___Endpoint_Transfer_Out(port_Number, 0, 0, test_Buffer, 64);
}

void USB_Device___EP0_RX_Callback(USB_LL_Device___RX_CALLBACK_PARAMETERS)
{
	if (packet_Type == USB_LL_Device___PACKET_TYPE_SETUP)
	{
		printf("Setup packet received: ");

		for(uint16_t i = 0; i < length; i++)
		{
			printf("%2.2x ", data[i]);
		}

		printf("\n");

		struct USB_Device___Setup_Packet setup_Packet;

		for (uint8_t i = 0; i < USB_Device___SETUP_PACKET_SIZE && i < length; i++)
		{
			((uint8_t*) &setup_Packet)[i] = data[i];
		}

		USB_Device___Control_Transfer[port_Number][endpoint_Number].Setup_Packet = setup_Packet;
		USB_Device___Control_Transfer[port_Number][endpoint_Number].data         = NULL;

		USB_Device___Handle_Control_Transfer(port_Number, endpoint_Number);

	}
	else if (packet_Type == USB_LL_Device___PACKET_TYPE_DATA)
	{
		printf("Data packet received: ");

		for(uint16_t i = 0; i < length; i++)
		{
			printf("%2.2x ", data[i]);
		}
		printf("\n");

		if(length > 0)
		{
			USB_Device___Control_Transfer[port_Number][endpoint_Number].data = data;
			USB_Device___Control_Transfer[port_Number][endpoint_Number].data_Size = length;
			USB_Device___Handle_Control_Transfer(port_Number, endpoint_Number);
		}
	}
}
