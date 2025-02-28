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

USB_Device___Device_TypeDef USB_Device___Device[USB_Device___NUM_PORTS];

void USB_Device___RX_Callback				(USB_LL_Device___RX_CALLBACK_PARAMETERS);
void USB_Device___TX_Callback				(USB_LL_Device___TX_CALLBACK_PARAMETERS);
void USB_Device___Set_Endpoint_TX_Callback	(uint8_t port_Number, uint8_t endpoint_Number, void callback(USB_Device___TX_CALLBACK_PARAMETERS));
void USB_Device___EP0_TX_Callback			(USB_Device___TX_CALLBACK_PARAMETERS);
void USB_Device___Set_FIFOs					(uint8_t port_Number)
{
	USB_LL_Device___FIFO_Config_TypeDef FIFO_Config = {0};

	FIFO_Config.RX_FIFO_Depth = USB_Device___DEFAULT_RX_FIFO_SIZE;

	for (uint8_t i = 0; i < USB_Device___NUM_ENDPOINTS; i++)
	{
		FIFO_Config.Endpoint_TX_FIFO_Depth[i] = USB_Device___Device[port_Number].endpoint[i].TX_FIFO_Depth;
	}

	USB_LL_Device___Set_FIFO_Size(port_Number, FIFO_Config);
}

void USB_Device___Set_Endpoint_TX_FIFO_Size(uint8_t port_Number, uint8_t endpoint_Number, uint16_t size)
{
	USB_Device___Device[port_Number].endpoint[endpoint_Number].TX_FIFO_Depth = size;

	USB_Device___Set_FIFOs(port_Number);
}

void USB_Device___Initialize_FIFOs(uint8_t port_Number)
{
	for (uint8_t i = 0; i < USB_Device___NUM_ENDPOINTS; i++)
	{
		USB_Device___Device[port_Number].endpoint[i].TX_FIFO_Depth = 0;
	}

	USB_Device___Device[port_Number].endpoint[0].TX_FIFO_Depth = USB_Device___DEFAULT_TX_FIFO_SIZE;

	USB_Device___Set_FIFOs(port_Number);
}

void USB_Device___Reset_Endpoint(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction)
{
	USB_LL_Device___Disable_Endpoint(port_Number, endpoint_Number,endpoint_Direction);
}

void USB_Device___Init(uint8_t port_Number)
{
	USB_LL___GPIO_Init(port_Number);
	USB_LL___Init(port_Number, USB_LL___DEVICE_MODE);
	USB_Device___Initialize_FIFOs(port_Number);

	USB_LL_Device___Init(port_Number);
	USB_LL_Device___Setup_Endpoint(port_Number, 0, USB_LL_Device___ENDPOINT_DERECTION_OUT, USB_LL_Device___ENDPOINT_TYPE_CONTROL, 64);
	USB_LL_Device___Setup_Endpoint(port_Number, 0, USB_LL_Device___ENDPOINT_DERECTION_IN, USB_LL_Device___ENDPOINT_TYPE_CONTROL, 64);
	USB_LL_Device___Endpoint_Transfer_Out(port_Number, 0, 0, USB_Device___Device[port_Number].endpoint_Zero_RX_Buffer, USB_Device___DEFAULT_EP0_RX_BUFFER_SIZE);
	USB_LL_Device___Set_Port_RX_Callback(port_Number, USB_Device___RX_Callback);
	USB_LL_Device___Set_Port_TX_Callback(port_Number, USB_Device___TX_Callback);
	USB_Device___Set_Endpoint_TX_Callback(port_Number, 0, USB_Device___EP0_TX_Callback);

	USB_Device___Device[port_Number].endpoint[0].control_Buffer 		= USB_Device___Device[port_Number].endpoint_Zero_RX_Buffer;
	USB_Device___Device[port_Number].endpoint[0].control_Buffer_Size 	= USB_Device___DEFAULT_EP0_RX_BUFFER_SIZE;
}

void USB_Device___Setup_Transmission(uint8_t port_Number, uint8_t endpoint_Number, uint8_t* data, uint16_t length)
{
	USB_LL_Device___Endpoint_Transfer_Out(port_Number, endpoint_Number, 0, data, length);
}

void USB_Device___Set_Conrol_Solutions(uint8_t port_Number, uint8_t endpoint_Number, struct USB_Device___Control_Solution *control_Solutions, uint8_t num_Solutions)
{
	USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Solutions 		= control_Solutions;
	USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Solution_Count 	= num_Solutions;
}

void USB_Device___Initialize_Endpoint(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction, uint8_t endpoint_Type, uint16_t max_Packet_Size)
{
	USB_LL_Device___Setup_Endpoint(port_Number, endpoint_Number, endpoint_Direction, endpoint_Type, max_Packet_Size);
	USB_Device___Device[port_Number].endpoint[endpoint_Number].type = endpoint_Type;
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

void USB_Device___Set_Endpoint_TX_Callback(uint8_t port_Number, uint8_t endpoint_Number, void callback(USB_Device___TX_CALLBACK_PARAMETERS))
{
    USB_Device___Device[port_Number].endpoint[endpoint_Number].TX_Callback = callback;
}

void USB_Device___Set_Endpoint_RX_Callback(uint8_t port_Number, uint8_t endpoint_Number, void callback(USB_Device___RX_CALLBACK_PARAMETERS))
{
    USB_Device___Device[port_Number].endpoint[endpoint_Number].RX_Callback = callback;
}

void USB_Device___Set_Nak(uint8_t port_Number, uint8_t endpoint_Number, uint8_t direction)
{
	USB_LL_Device___Endpoint_Set_NAK(port_Number, endpoint_Number, direction);
}

uint8_t USB_Device___Transfer_In(uint8_t port_Number, uint8_t endpoint_Number, void *data, uint16_t length) {

	if(!USB_LL_Device___Is_Endpoint_Busy(port_Number, endpoint_Number, USB_LL_Device___ENDPOINT_DERECTION_IN))
	{
		USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, (uint8_t*)data, length);
		return(1);
	}
	return(0);
}

uint8_t USB_Device___Open_RX_Endpoint(uint8_t port_Number, uint8_t endpoint_Number, uint16_t transfer_Size, uint8_t* buffer, uint16_t buffer_Size)
{
	USB_LL_Device___Endpoint_Transfer_Out(port_Number, endpoint_Number, transfer_Size, buffer, buffer_Size);
	return(1);
}

void USB_Device___TX_Callback( USB_LL_Device___TX_CALLBACK_PARAMETERS)
{
	if (USB_Device___Device[port_Number].endpoint[endpoint_Number].TX_Callback != NULL)
	{
		USB_Device___Device[port_Number].endpoint[endpoint_Number].TX_Callback(port_Number, endpoint_Number);
	}
}

void USB_Device___EP0_TX_Callback(USB_Device___TX_CALLBACK_PARAMETERS)
{
	USB_LL_Device___Endpoint_Transfer_Out(port_Number, endpoint_Number, 0, USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Buffer, USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Buffer_Size);
}


void USB_Device___Handle_Control_Transfer(USB_LL_Device___RX_CALLBACK_PARAMETERS)
{
	if (packet_Type == USB_LL_Device___PACKET_TYPE_SETUP && length == USB_Device___SETUP_PACKET_SIZE)
	{
		struct USB_Device___Setup_Packet setup_Packet = *(struct USB_Device___Setup_Packet*)data;
		USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Transfer.Setup_Packet = setup_Packet;
		USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Transfer.data         = NULL;
	}
	else if (packet_Type == USB_LL_Device___PACKET_TYPE_DATA && length > 0 )
	{
		USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Transfer.data = data;
		USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Transfer.data_Size = length;
	}
	else
	{
		return;
	}

	struct USB_Device___Control_Transfer* control_Transfer = &USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Transfer;

	if (!(control_Transfer->Setup_Packet.bmRequestType & 0x80) && control_Transfer->Setup_Packet.wLength > 0 && control_Transfer->data == NULL)
	{
		USB_LL_Device___Endpoint_Transfer_Out(port_Number, endpoint_Number, 0, USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Buffer, USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Buffer_Size);
	}
	else
	{
		uint8_t solution_Found = 0;
		for(uint8_t i = 0; i < USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Solution_Count; i++)
		{
			USB_Device___Control_Solution_TypeDef* control_Solution = &USB_Device___Device[port_Number].endpoint[endpoint_Number].control_Solutions[i];

			if (USB_Device___Control_Solution_Check_Key(control_Solution, control_Transfer->Setup_Packet))
			{
				solution_Found = 1;
				if(control_Transfer->Setup_Packet.bmRequestType & 0x80)
				{
					if(control_Solution->callback != NULL)
					{
						control_Solution->callback(port_Number, endpoint_Number, control_Solution, control_Transfer->Setup_Packet, control_Transfer->data, control_Transfer->data_Size);
					}

					uint16_t length = USB_Device___GET_MIN(control_Transfer->Setup_Packet.wLength, control_Solution->buffer_Size));
					USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, control_Solution->buffer, length);
				}
				else
				{
					uint8_t result = 1;

					if(control_Solution->callback != NULL)
					{
						result = control_Solution->callback(port_Number, endpoint_Number, control_Solution, control_Transfer->Setup_Packet, control_Transfer->data, control_Transfer->data_Size);
					}

					if(result == 1)
					{
						USB_LL_Device___Endpoint_Transfer_In(port_Number, endpoint_Number, control_Solution->buffer, control_Solution->buffer_Size);
					}
					else
					{
						USB_LL_Device___Endpoint_Set_Stall(port_Number, endpoint_Number, USB_LL_Device___ENDPOINT_DERECTION_IN);
					}
				}

				return;
			}
		}

		if (!solution_Found)
		{
			USB_LL_Device___Endpoint_Set_Stall(port_Number, endpoint_Number, USB_LL_Device___ENDPOINT_DERECTION_IN);
		}
	}
}

void USB_Device___RX_Callback(USB_LL_Device___RX_CALLBACK_PARAMETERS)
{
	if(USB_Device___Device[port_Number].endpoint[endpoint_Number].type == USB_Device___ENDPOINT_TYPE_CONTROL)
	{
		USB_Device___Handle_Control_Transfer(port_Number, endpoint_Number, packet_Type, data, length);
	}
	else
	{
		if (USB_Device___Device[port_Number].endpoint[endpoint_Number].RX_Callback != NULL)
		{
			USB_Device___Device[port_Number].endpoint[endpoint_Number].RX_Callback(port_Number, endpoint_Number, data, length);
		}
	}
}
