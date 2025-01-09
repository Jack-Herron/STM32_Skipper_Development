/*
 * USB_Host_Transfers.c
 *
 *  Created on: Jul 8, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stdbool.h>
#include <stdio.h>

#include "../Inc/USB_Host_Transfers.h"
#include "../../Config/USB_Host_Config.h"
#include "../Inc/USB_Host_Pipes.h"

#if USB_Host_Config___DYNAMICALLY_ALLOCATE_USB_REQUEST_BLOCKS

	USB_Host_Transfers___URB_Node_TypeDef* USB_Host_Transfers___Allocate_URB_Node()
	{
		USB_Host_Transfers___URB_Node_TypeDef* p_URB_Node;
		p_URB_Node = (USB_Host_Transfers___URB_Node_TypeDef*)malloc(sizeof(USB_Host_Transfers___URB_Node_TypeDef));
		return(p_URB_Node);
	}

	void USB_Host_Transfers___Free_URB_Node(USB_Host_Transfers___URB_Node_TypeDef* p_URB_Node)
	{
		free(p_URB_Node);
	}

#else

	static USB_Host_Transfers___URB_Node_TypeDef USB_Host_Transfers___URB_Node_Pool[USB_Host_Config___MAX_USB_REQUEST_BLOCKS];

	USB_Host_Transfers___URB_Node_TypeDef* USB_Host_Transfers___Allocate_URB_Node()
	{
		for(uint32_t i = 0; i < USB_Host_Config___MAX_USB_REQUEST_BLOCKS; i++)
		{
			if(!USB_Host_Transfers___URB_Node_Pool[i].is_Allocated)
			{
				USB_Host_Transfers___URB_Node_Pool[i].is_Allocated = true;
				return(&USB_Host_Transfers___URB_Node_Pool[i]);
			}
		}
		return(NULL);
	}

	void USB_Host_Transfers___Free_URB_Node(USB_Host_Transfers___URB_Node_TypeDef* p_URB_Node)
	{
		p_URB_Node -> is_Allocated = false;
	}
#endif

static USB_Host_Transfers___URB_Queue_TypeDef URB_Queue[USB_Host___NUMBER_OF_PORTS];

void USB_Debug___Print_URB(USB_Host_Transfers___URB_TypeDef URB)
{
	/*
	USB_Host_Transfers___Debug_Log("--URB--\n");
	USB_Host_Transfers___Debug_Log("Port number    : %d\n",URB.port_Number);
	USB_Host_Transfers___Debug_Log("Device address : %d\n",URB.device_Address);
	USB_Host_Transfers___Debug_Log("Transfer type  : %d\n",URB.transfer_Type);
	USB_Host_Transfers___Debug_Log("Transfer stage : %d\n",URB.transfer_Stage);
	USB_Host_Transfers___Debug_Log("\n");*/
	//, (URB_Queue.num_URBs));
}

USB_Host_Transfers___URB_Node_TypeDef* USB_Host_Transfers___Create_URB_Node(uint8_t port_Number)
{
	USB_Host_Transfers___URB_Node_TypeDef* p_URB_Node = USB_Host_Transfers___Allocate_URB_Node();

	if(p_URB_Node != NULL)
	{
		p_URB_Node -> next_Node = NULL;
		p_URB_Node -> previous_Node = NULL;

		if(URB_Queue[port_Number].first_Node == NULL)
		{
			URB_Queue[port_Number].first_Node = p_URB_Node;
			URB_Queue[port_Number].last_Node = p_URB_Node;
		}
		else
		{
			p_URB_Node -> previous_Node 					= URB_Queue[port_Number].last_Node;
			URB_Queue[port_Number].last_Node -> next_Node 	= p_URB_Node;
			URB_Queue[port_Number].last_Node 				= p_URB_Node;
		}
		URB_Queue[port_Number].num_URBs++;
	}
	return(p_URB_Node);
}

void USB_Host_Transfers___Delete_URB_Node(uint8_t port_Number, USB_Host_Transfers___URB_Node_TypeDef* p_URB_Node)
{
	if(p_URB_Node != NULL)
	{
		if(p_URB_Node == URB_Queue[port_Number].first_Node)
		{

			URB_Queue[port_Number].first_Node = p_URB_Node->next_Node;
		}

		if(p_URB_Node == URB_Queue[port_Number].last_Node)
		{
			URB_Queue[port_Number].last_Node = p_URB_Node->previous_Node;
		}

		if(p_URB_Node -> previous_Node != NULL)
		{
			p_URB_Node->previous_Node -> next_Node 	= p_URB_Node -> next_Node;
		}

		if(p_URB_Node -> next_Node != NULL)
		{
			p_URB_Node->next_Node -> previous_Node 	= p_URB_Node -> previous_Node;
		}
		URB_Queue[port_Number].num_URBs--;
		USB_Host_Transfers___Free_URB_Node(p_URB_Node);
	}
}

int8_t USB_Host_Transfers___Isochronous_Transfer(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t transfer_Direction, uint8_t* transfer_Buffer, uint32_t transfer_Length, uint16_t number_Of_Retries, uint8_t odd_Frame, uint8_t multi_Count, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_Node_TypeDef* 	p_URB_Node = USB_Host_Transfers___Create_URB_Node(port_Number);
	USB_Host_Transfers___URB_TypeDef* 		p_URB = &p_URB_Node->URB;

	if(p_URB != NULL)
	{
		p_URB -> port_Number 		= port_Number;
		p_URB -> endpoint_Number	= endpoint_Number;
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_ISOCHRONOUS;
		p_URB -> transfer_Direction = transfer_Direction;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;
		p_URB -> num_Retries		= number_Of_Retries;
		p_URB -> is_Odd_Frame		= odd_Frame;
		p_URB -> multi_Count		= multi_Count;
		p_URB -> URB_Callback		= URB_Callback;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Interrupt_Transfer(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t transfer_Direction, uint8_t* transfer_Buffer, uint32_t transfer_Length, uint16_t number_Of_Retries, uint8_t odd_Frame, uint8_t multi_Count, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_Node_TypeDef* 	p_URB_Node = USB_Host_Transfers___Create_URB_Node(port_Number);
	USB_Host_Transfers___URB_TypeDef* 		p_URB = &p_URB_Node->URB;

	if(p_URB != NULL)
	{
		p_URB -> port_Number 		= port_Number;
		p_URB -> endpoint_Number	= endpoint_Number;
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_INTERRUPT;
		p_URB -> transfer_Direction = transfer_Direction;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;
		p_URB -> num_Retries		= number_Of_Retries;
		p_URB -> is_Odd_Frame		= odd_Frame;
		p_URB -> multi_Count		= multi_Count;
		p_URB -> URB_Callback		= URB_Callback;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Bulk_Transfer(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t transfer_Direction, uint8_t* transfer_Buffer, uint32_t transfer_Length, uint16_t number_Of_Retries, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_Node_TypeDef* 	p_URB_Node = USB_Host_Transfers___Create_URB_Node(port_Number);
	USB_Host_Transfers___URB_TypeDef* 		p_URB = &p_URB_Node->URB;

	if(p_URB != NULL)
	{
		p_URB -> port_Number 		= port_Number;
		p_URB -> endpoint_Number	= endpoint_Number;
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_BULK;
		p_URB -> transfer_Direction = transfer_Direction;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;
		p_URB -> num_Retries		= number_Of_Retries;
		p_URB -> URB_Callback		= URB_Callback;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Control_Transfer(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t transfer_Direction, USB_Host_Transfers___Control_Setup_Packet_TypeDef setup_Packet, uint8_t* transfer_Buffer, uint32_t transfer_Length, uint16_t number_Of_Retries, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_Node_TypeDef* 	p_URB_Node = USB_Host_Transfers___Create_URB_Node(port_Number);
	USB_Host_Transfers___URB_TypeDef* 		p_URB = &p_URB_Node->URB;

	if(p_URB != NULL)
	{
		p_URB -> port_Number 			= port_Number;
		p_URB -> endpoint_Number		= endpoint_Number;
		p_URB -> transfer_Type 			= USB_Host_Transfers___URB_TYPE_CONTROL;
		p_URB -> transfer_Direction 	= transfer_Direction;
		p_URB -> transfer_Stage			= USB_Host_Transfers___URB_STAGE_SETUP;
		p_URB -> busy					= false;
		p_URB -> device_Address 		= device_Address;
		p_URB -> transfer_Buffer 		= transfer_Buffer;
		p_URB -> transfer_Length 		= transfer_Length;
		p_URB -> num_Retries			= number_Of_Retries;
		p_URB -> URB_Callback			= URB_Callback;
		p_URB -> control_Setup_Packet	= setup_Packet;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

void USB_Host_Transfers___Set_Next_URB_Transfer_Stage(USB_Host_Transfers___URB_Node_TypeDef* p_URB_Node)
{
	USB_Host_Transfers___URB_TypeDef* 		p_URB = &p_URB_Node->URB;

	if((p_URB->transfer_Type == USB_Host_Transfers___URB_TYPE_CONTROL) && (p_URB->transfer_Status == USB_LL_Interrupts_Host___CHANNEL_STATUS_TRANSFER_COMPLETE))
	{
		if(p_URB -> transfer_Stage == USB_Host_Transfers___URB_STAGE_SETUP)
		{
			USB_Host_Device_Manager___Device_Set_Endpoint_Current_Packet_ID(p_URB->port_Number, p_URB->device_Address, p_URB->endpoint_Number, USB_Host___TRANSFER_DIRECTION_IN, USB_Host_Pipes___PID_DATA1);
			USB_Host_Device_Manager___Device_Set_Endpoint_Current_Packet_ID(p_URB->port_Number, p_URB->device_Address, p_URB->endpoint_Number, USB_Host___TRANSFER_DIRECTION_OUT, USB_Host_Pipes___PID_DATA1);
			if(p_URB -> transfer_Length > 0)
			{
				p_URB -> transfer_Stage = USB_Host_Transfers___URB_STAGE_DATA;
			}
			else
			{
				p_URB -> transfer_Stage = USB_Host_Transfers___URB_STAGE_STATUS;
			}
		}
		else if(p_URB -> transfer_Stage == USB_Host_Transfers___URB_STAGE_DATA)
		{
			p_URB -> transfer_Stage = USB_Host_Transfers___URB_STAGE_STATUS;
		}
		else if(p_URB -> transfer_Stage == USB_Host_Transfers___URB_STAGE_STATUS)
		{
			p_URB -> URB_Callback(*p_URB);
			USB_Host_Transfers___Delete_URB_Node(p_URB->port_Number, p_URB_Node);
		}
	}
	else
	{
		p_URB -> URB_Callback(*p_URB);
		USB_Host_Transfers___Delete_URB_Node(p_URB->port_Number, p_URB_Node);
	}
}

void pipe_Callback(USB_Host_Pipes___Callback_Parameters)
{
	USB_Host_Transfers___URB_Node_TypeDef* 	p_URB_Node = (USB_Host_Transfers___URB_Node_TypeDef*)context;
	USB_Host_Transfers___URB_TypeDef* 		p_URB = &p_URB_Node->URB;

	uint8_t pipe_Direction 		= USB_Host_Pipes___Get_Pipe_Direction(port_Number, pipe_Number);
	uint8_t packet_ID 			= USB_Host_Pipes___Get_Current_Packet_ID(port_Number, pipe_Number);

	p_URB -> transfer_Status 	= status;

	USB_Host_Device_Manager___Device_Set_Endpoint_Current_Packet_ID(p_URB->port_Number, p_URB->device_Address, pipe_Direction, p_URB->endpoint_Number, packet_ID);
	USB_Host_Transfers___Set_Next_URB_Transfer_Stage(p_URB_Node);


	p_URB -> busy = false;
}

void USB_Host_Transfers___Process_URB_Setup_Stage(USB_Host_Transfers___URB_Node_TypeDef* p_URB_Node)
{
	USB_Host_Transfers___URB_TypeDef* 		p_URB = &p_URB_Node->URB;

	p_URB -> busy = true;
	p_URB -> transfer_Status = 0;
	uint8_t pipe_Number = USB_Host_Pipes___Create_Pipe
	(
		p_URB->port_Number,
		p_URB->device_Address,
		(void*)p_URB_Node,
		p_URB->transfer_Type,
		USB_Host___TRANSFER_DIRECTION_OUT,
		p_URB->endpoint_Number,
		USB_Host_Device_Manager___Device_Get_Out_Endpoint_Max_Packet_Size(p_URB->port_Number, p_URB->device_Address, p_URB->endpoint_Number),
		(uint8_t*)&p_URB->control_Setup_Packet,
		USB_Host_Transfers___CONTROL_SETUP_PACKET_LENGTH,
		p_URB->num_Retries,
		0,
		USB_Host_Device_Manager___Device_Is_Low_Speed_Device(p_URB->port_Number, p_URB->device_Address),
		0,
		USB_Host_Pipes___PID_SETUP,
		pipe_Callback
	);

	USB_Debug___Print_URB(*p_URB);

	USB_Host_Pipes___Begin_Transfer(p_URB->port_Number, pipe_Number);
}

void USB_Host_Transfers___Process_URB_Data_Stage(USB_Host_Transfers___URB_Node_TypeDef* p_URB_Node)
{
	USB_Host_Transfers___URB_TypeDef* 		p_URB = &p_URB_Node->URB;

	p_URB -> busy = true;
	p_URB -> transfer_Status = 0;
	uint8_t pipe_Number;

	uint16_t max_Packet_Size;

	if(p_URB->transfer_Direction == USB_Host_Transfers___URB_DIRECTION_OUT)
	{
		max_Packet_Size = USB_Host_Device_Manager___Device_Get_Out_Endpoint_Max_Packet_Size(p_URB->port_Number, p_URB->device_Address, p_URB->endpoint_Number);
	}
	else
	{
		max_Packet_Size = USB_Host_Device_Manager___Device_Get_In_Endpoint_Max_Packet_Size(p_URB->port_Number, p_URB->device_Address, p_URB->endpoint_Number);
	}

	pipe_Number = USB_Host_Pipes___Create_Pipe
	(
		p_URB->port_Number,
		p_URB->device_Address,
		(void*)p_URB_Node,
		p_URB->transfer_Type,
		p_URB->transfer_Direction,
		p_URB->endpoint_Number,
		max_Packet_Size,
		p_URB->transfer_Buffer,
		p_URB->transfer_Length,
		p_URB->num_Retries,
		p_URB->is_Odd_Frame,
		USB_Host_Device_Manager___Device_Is_Low_Speed_Device(p_URB->port_Number, p_URB->device_Address),
		p_URB->multi_Count,
		USB_Host_Device_Manager___Device_Get_Endpoint_Current_Packet_ID(p_URB->port_Number, p_URB->device_Address, p_URB->transfer_Direction, p_URB->endpoint_Number),
		pipe_Callback
	);

	USB_Debug___Print_URB(*p_URB);

	USB_Host_Pipes___Begin_Transfer(p_URB->port_Number, pipe_Number);
}

void USB_Host_Transfers___Process_URB_Status_Stage(USB_Host_Transfers___URB_Node_TypeDef* p_URB_Node)
{
	USB_Host_Transfers___URB_TypeDef* 		p_URB = &p_URB_Node->URB;

	p_URB -> busy = true;
	p_URB -> transfer_Status = 0;
	uint8_t pipe_Number;

	pipe_Number = USB_Host_Pipes___Create_Pipe
	(
		p_URB->port_Number,
		p_URB->device_Address,
		(void*)p_URB_Node,
		p_URB->transfer_Type,
		!p_URB->transfer_Direction,
		p_URB->endpoint_Number,
		USB_Host_Device_Manager___Device_Get_Out_Endpoint_Max_Packet_Size(p_URB->port_Number, p_URB->device_Address, p_URB->endpoint_Number),
		0,
		0,
		p_URB->num_Retries,
		0,
		USB_Host_Device_Manager___Device_Is_Low_Speed_Device(p_URB->port_Number, p_URB->device_Address),
		0,
		USB_Host_Pipes___PID_DATA1,
		pipe_Callback
	);

	USB_Debug___Print_URB(*p_URB);

	USB_Host_Pipes___Begin_Transfer(p_URB->port_Number, pipe_Number);
}

void USB_Host_Transfers___Process_URB(uint8_t port_Number)
{
	USB_Host_Transfers___URB_Node_TypeDef* current_Node = URB_Queue[port_Number].first_Node;

	uint32_t device_Busy_Mask[4] = {0};

	while(current_Node != NULL)
	{
		uint8_t device_Address = current_Node->URB.device_Address;
		if(!(device_Busy_Mask[device_Address / 32] & (1 << (device_Address % 32))))
		{
			device_Busy_Mask[device_Address / 32] |= (1 << (device_Address % 32));

			if(!(current_Node->URB.busy))
			{
				switch(current_Node->URB.transfer_Stage)
				{
				case USB_Host_Transfers___URB_STAGE_SETUP:
					USB_Host_Transfers___Process_URB_Setup_Stage(current_Node);
					break;
				case USB_Host_Transfers___URB_STAGE_DATA:
					USB_Host_Transfers___Process_URB_Data_Stage(current_Node);
					break;
				case USB_Host_Transfers___URB_STAGE_STATUS:
					USB_Host_Transfers___Process_URB_Status_Stage(current_Node);
					break;
				}
			}
		}

		current_Node = current_Node -> next_Node;
	}
}
