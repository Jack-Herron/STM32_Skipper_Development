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
struct USB_Device___Control_Solution* control_Solutions[USB_Device___NUM_PORTS][USB_Device___NUM_ENDPOINTS];
uint8_t control_Solution_Count[USB_Device___NUM_PORTS][USB_Device___NUM_ENDPOINTS];
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

void USB_Device___Setup_Transmission(uint8_t port_Number, uint8_t endpoint_Number, uint8_t* data, uint16_t length)
{
	USB_LL_Device___Endpoint_Transfer_Out(port_Number, endpoint_Number, 0, data, length);
}

void USB_Device___Setup_Reception(uint8_t port_Number, uint8_t endpoint_Number, uint8_t *data, uint16_t length)
{
	USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, data, length);
}

void USB_Device___Set_Control_Transfer_Callback(uint8_t port_Number, uint8_t endpoint_Number, void callback(USB_Device___CONTROL_TRANSFER_CALLBACK_PARAMETERS))
{
	USB_Device___Control_Transfer[port_Number][endpoint_Number].callback = callback;
}

void USB_Device___Set_Conrol_Solutions(uint8_t port_Number, uint8_t endpoint_Number, struct USB_Device___Control_Solution *control_Solution, uint8_t num_Solutions)
{
	control_Solutions[port_Number][endpoint_Number] = control_Solution;
	control_Solution_Count[port_Number][endpoint_Number]= num_Solutions;
}

void USB_Device___Initialize_Endpoint(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction, uint8_t endpoint_Type, uint16_t max_Packet_Size) {
	USB_LL_Device___Setup_Endpoint(port_Number, endpoint_Number, endpoint_Direction, endpoint_Type, max_Packet_Size);
}

uint8_t USB_Device___Control_Solution_Check_Key(struct USB_Device___Control_Solution *control_Solution, struct USB_Device___Setup_Packet setup_Packet)
{
	if (		((control_Solution->Setup_Packet_Key.bmRequestType 	== setup_Packet.bmRequestType) 	|| !(control_Solution->is_Key & (1<<4)))
			&& 	((control_Solution->Setup_Packet_Key.bRequest 		== setup_Packet.bRequest) 		|| !(control_Solution->is_Key & (1<<3)))
			&& 	((control_Solution->Setup_Packet_Key.wValue 		== setup_Packet.wValue) 		|| !(control_Solution->is_Key & (1<<2)))
			&& 	((control_Solution->Setup_Packet_Key.wIndex 		== setup_Packet.wIndex) 		|| !(control_Solution->is_Key & (1<<1)))
			&& 	((control_Solution->Setup_Packet_Key.wLength 		== setup_Packet.wLength) 		|| !(control_Solution->is_Key & (1<<0))))
	{
		return 1;
	}

	return 0;
}

uint8_t USB_Device___Set_Address_Solution_Callback(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS)
{
	USB_LL_Device___Set_Address(port_Number, setup_Packet.wValue);
	return(1);
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
		uint8_t solution_Found = 0;
		for(uint8_t i = 0; i < control_Solution_Count[port_Number][endpoint_Number]; i++)
		{
			if (USB_Device___Control_Solution_Check_Key(&control_Solutions[port_Number][endpoint_Number][i], control_Transfer.Setup_Packet))
			{
				solution_Found = 1;
				if(control_Transfer.Setup_Packet.bmRequestType & 0x80)
				{
					if(control_Solutions[port_Number][endpoint_Number][i].callback != NULL)
					{
						control_Solutions[port_Number][endpoint_Number][i].callback(port_Number, endpoint_Number, &control_Solutions[port_Number][endpoint_Number][i], control_Transfer.Setup_Packet, control_Transfer.data, control_Transfer.data_Size);
					}

					uint16_t length = USB_Device___GET_MIN(control_Transfer.Setup_Packet.wLength, control_Solutions[port_Number][endpoint_Number][i].buffer_Size));
					USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, control_Solutions[port_Number][endpoint_Number][i].buffer, length);
				}
				else
				{
					uint8_t result = 1;

					if(control_Solutions[port_Number][endpoint_Number][i].callback != NULL)
					{
						control_Solutions[port_Number][endpoint_Number][i].callback(port_Number, endpoint_Number, &control_Solutions[port_Number][endpoint_Number][i], control_Transfer.Setup_Packet, control_Transfer.data, control_Transfer.data_Size);
					}

					if(result == 1)
					{
						USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, control_Solutions[port_Number][endpoint_Number][i].buffer, control_Solutions[port_Number][endpoint_Number][i].buffer_Size);
					}
				}

				return;
			}
		}
		if (!solution_Found)
		{
			printf("No solution found for request----------------------------------------------------------\n");
			USB_LL_Device___Endpoint_Set_Stall(port_Number, endpoint_Number, USB_LL_Device___ENDPOINT_DERECTION_IN);
		}
	}
}

void USB_Device___Set_Nak(uint8_t port_Number, uint8_t endpoint_Number, uint8_t direction)
{
	USB_LL_Device___Endpoint_Set_NAK(port_Number, endpoint_Number, direction);
}

void USB_Device___Transfer_In(uint8_t port_Number, uint8_t endpoint_Number, uint8_t *data, uint16_t length) {
	USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, data, length);
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

		if (length == 0)
		{
			printf("Error: Setup packet too short\n");
		}

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
