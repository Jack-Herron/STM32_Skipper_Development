/*
 * USB_Host_Pipes.cpp
 *
 *  Created on: Jun 23, 2024
 *      Author: Jack Herron
 */

#include "../Inc/USB_Host_Pipes.h"
#include <stdlib.h>
#include <USB_LL_Definitions.h>
#include <USB_LL_Host.h>
#include <USB_LL_Interrupts_Host.h>
#include "../Inc/USB_Host.h"

static USB_Host_Pipes___Pipe_TypeDef USB_Host_Pipes___Pipe[USB_LL_Definitions___NUMBER_OF_PORTS][USB_Host_Pipes___NUMBER_OF_PIPES];

uint8_t USB_Host_Pipes___Allocate_Pipe(uint8_t port_Number)
{
	for (uint8_t i = 0; i < USB_LL_Definitions___MAX_NUMBER_OF_CHANNELS_PER_PORT; i++)
	{
		if (USB_Host_Pipes___Pipe[port_Number][i].is_Allocated == 0)
		{
			USB_Host_Pipes___Pipe[port_Number][i].is_Allocated = 1;
			return (i);
		}
	}
	return (0xFF);
}

void USB_Host_Pipes___Free_Pipe(uint8_t port_Number, uint8_t channel_Number)
{
	USB_Host_Pipes___Pipe[port_Number][channel_Number].is_Allocated = 0;
}

uint8_t USB_Host_Pipes___Create_Pipe
		(
		uint8_t 	port_Number,
		uint8_t 	device_Address,
		void*		context,
		uint8_t 	pipe_Type,
		uint8_t 	pipe_Direction,
		uint8_t 	endpoint_Number,
		uint16_t 	max_Packet_Size,
		uint8_t*	p_Buffer,
		uint32_t 	transfer_Length,
		uint16_t 	number_Of_Retries,
		uint8_t		is_Odd_Frame,
		uint8_t		is_Low_Speed,
		uint8_t 	multi_Count,
		uint8_t 	packet_ID,
		void		callback(USB_Host_Pipes___Callback_Parameters)
		)
{
	uint8_t pipe_Number = USB_Host_Pipes___Allocate_Pipe(port_Number);

	USB_Host_Pipes___Pipe[port_Number][pipe_Number].context					= context;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].pipe_Type 				= pipe_Type;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].pipe_Direction 			= pipe_Direction;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].device_Address 			= device_Address;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].endpoint_Number 		= endpoint_Number;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].max_Packet_Size 		= max_Packet_Size;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].p_Buffer 				= p_Buffer;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].transfer_Length 		= transfer_Length;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].is_Odd_Frame 			= is_Odd_Frame;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].is_Low_Speed 			= is_Low_Speed;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].multi_Count 			= multi_Count;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].num_Packets				= (transfer_Length + max_Packet_Size-1) / max_Packet_Size;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].num_Packets_Remaining 	= (transfer_Length + max_Packet_Size-1) / max_Packet_Size;
	USB_Host_Pipes___Pipe[port_Number][pipe_Number].callback 				= callback;

	if(USB_Host_Pipes___Pipe[port_Number][pipe_Number].num_Packets == 0)
	{
		USB_Host_Pipes___Pipe[port_Number][pipe_Number].num_Packets = 1;
	}

	USB_LL_Host___Channel_Load_HCTSIZ(port_Number, pipe_Number, transfer_Length, USB_Host_Pipes___Pipe[port_Number][pipe_Number].num_Packets, packet_ID);
	USB_LL_Host___Channel_Setup_Buffer(port_Number, pipe_Number, p_Buffer, transfer_Length);
	USB_LL_Host___Channel_Set_Number_Of_Retries(port_Number, pipe_Number, number_Of_Retries);
	USB_LL_Host___Channel_Set_Characteristics(port_Number, pipe_Number, max_Packet_Size, endpoint_Number, pipe_Direction, is_Low_Speed, pipe_Type, multi_Count, device_Address, is_Odd_Frame);
	USB_LL_Host___Channel_Set_Interrupts(port_Number, pipe_Number);

	return(pipe_Number);
}

uint8_t USB_Host_Pipes___Get_Pipe_Direction(uint8_t port_Number, uint8_t pipe_Number)
{
	return(USB_Host_Pipes___Pipe[port_Number][pipe_Number].pipe_Direction);
}

uint8_t USB_Host_Pipes___Get_Current_Packet_ID(uint8_t port_Number, uint8_t pipe_Number)
{
	return(USB_LL_Host___Channel_Get_Current_Packet_ID(port_Number, pipe_Number));
}

void USB_Host_Pipes___Begin_Transfer(uint8_t port_Number, uint8_t pipe_Number)
{
	if(USB_Host_Pipes___Pipe[port_Number][pipe_Number].pipe_Direction == USB_Host___TRANSFER_DIRECTION_OUT)
	{
		USB_LL_Host___Channel_Begin_Transfer_Out(port_Number,  pipe_Number);
	}
	else
	{
		USB_LL_Host___Channel_Begin_Transfer_In(port_Number,  pipe_Number);
	}
}

void 	USB_Host_Pipes___Process_Pipes	(uint8_t port_Number)
{
	if(USB_LL_Interrupts_Host___Get_All_Channels_Status_Change_Flag(port_Number))
	{
		USB_LL_Interrupts_Host___Clear_All_Channels_Status_Change_Flag(port_Number);
		for(uint8_t i = 0; i < USB_Host_Pipes___NUMBER_OF_PIPES; i++)
		{
			if(USB_LL_Interrupts_Host___Get_Channel_Status_Change_Flag(port_Number, i))
			{
				USB_LL_Interrupts_Host___Clear_Channel_Status_Change_Flag(port_Number, i);
				uint8_t channel_Status = USB_LL_Interrupts_Host___Get_Channel_Status(port_Number, i);
				if(channel_Status == USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_COMPLETE)
				{
					USB_Host_Pipes___Free_Pipe(port_Number, i);
					if(USB_Host_Pipes___Pipe[port_Number][i].callback != NULL)
					{
						USB_Host_Pipes___Pipe[port_Number][i].callback(port_Number, i, USB_Host_Pipes___Pipe[port_Number][i].context, channel_Status, USB_Host_Pipes___Pipe[port_Number][i].p_Buffer, USB_Host_Pipes___Pipe[port_Number][i].transfer_Length);
					}
				}
			}
		}
	}
}
