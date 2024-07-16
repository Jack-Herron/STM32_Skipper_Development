/*
 * USB_Host_Transfers.c
 *
 *  Created on: Jul 8, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stdbool.h>

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

static USB_Host_Transfers___URB_Queue_TypeDef URB_Queue;

USB_Host_Transfers___URB_TypeDef* USB_Host_Transfers___Create_URB()
{
	USB_Host_Transfers___URB_Node_TypeDef* p_URB_Node = USB_Host_Transfers___Allocate_URB_Node();
	p_URB_Node -> next_Node = NULL;
	if(p_URB_Node != NULL)
	{
		if(URB_Queue.current_Node == NULL)
		{
			URB_Queue.current_Node = p_URB_Node;
			URB_Queue.last_Node = p_URB_Node;
		}
		else
		{
			URB_Queue.last_Node -> next_Node = p_URB_Node;
			URB_Queue.last_Node = p_URB_Node;
		}
	}
	return(&p_URB_Node->URB);
}

void USB_Host_Transfers___Delete_Current_URB()
{
	USB_Host_Transfers___URB_Node_TypeDef* p_Current_URB_Node = URB_Queue.current_Node;
	if(p_Current_URB_Node != NULL)
	{
		URB_Queue.current_Node = p_Current_URB_Node -> next_Node;
		USB_Host_Transfers___Free_URB_Node(p_Current_URB_Node);
	}
}

USB_Host_Transfers___URB_TypeDef* USB_Host_Tranfers___Get_Current_URB()
{
	if(URB_Queue.current_Node != NULL)
	{
		return(&URB_Queue.current_Node->URB);
	}
	return(NULL);
}

int8_t USB_Host_Transfers___Isochronous_Transfer_Out(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Create_URB();
	if(p_URB != NULL)
	{
		p_URB -> port_Number 		= port_Number;
		p_URB -> endpoint_Number	= endpoint_Number;
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_ISOCHRONOUS;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_OUT;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;
		p_URB -> URB_Callback		= URB_Callback;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Interrupt_Transfer_Out(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Create_URB();
	if(p_URB != NULL)
	{
		p_URB -> port_Number 		= port_Number;
		p_URB -> endpoint_Number	= endpoint_Number;
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_INTERRUPT;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_OUT;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;
		p_URB -> URB_Callback		= URB_Callback;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Bulk_Transfer_Out(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Create_URB();
	if(p_URB != NULL)
	{
		p_URB -> port_Number 		= port_Number;
		p_URB -> endpoint_Number	= endpoint_Number;
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_BULK;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_OUT;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;
		p_URB -> URB_Callback		= URB_Callback;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Control_Transfer_Out(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, USB_Host_Transfers___Control_Setup_Packet setup_Packet, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Create_URB();
	if(p_URB != NULL)
	{
		p_URB -> port_Number 			= port_Number;
		p_URB -> endpoint_Number		= endpoint_Number;
		p_URB -> transfer_Type 			= USB_Host_Transfers___URB_TYPE_CONTROL;
		p_URB -> transfer_Direction 	= USB_Host_Transfers___URB_DIRECTION_OUT;
		p_URB -> transfer_Stage			= USB_Host_Transfers___URB_STAGE_SETUP;
		p_URB -> busy					= false;
		p_URB -> device_Address 		= device_Address;
		p_URB -> transfer_Buffer 		= transfer_Buffer;
		p_URB -> transfer_Length 		= transfer_Length;
		p_URB -> URB_Callback			= URB_Callback;
		p_URB -> control_Setup_Packet	= setup_Packet;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Isochronous_Transfer_In(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Create_URB();
	if(p_URB != NULL)
	{
		p_URB -> port_Number 		= port_Number;
		p_URB -> endpoint_Number	= endpoint_Number;
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_ISOCHRONOUS;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_IN;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;
		p_URB -> URB_Callback		= URB_Callback;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Interrupt_Transfer_In(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Create_URB();
	if(p_URB != NULL)
	{
		p_URB -> port_Number 		= port_Number;
		p_URB -> endpoint_Number	= endpoint_Number;
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_INTERRUPT;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_IN;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;
		p_URB -> URB_Callback		= URB_Callback;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Bulk_Transfer_In(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Create_URB();
	if(p_URB != NULL)
	{
		p_URB -> port_Number 		= port_Number;
		p_URB -> endpoint_Number	= endpoint_Number;
		p_URB -> transfer_Type		= USB_Host_Transfers___URB_TYPE_BULK;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_IN;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;
		p_URB -> URB_Callback		= URB_Callback;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Control_Transfer_In(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, USB_Host_Transfers___Control_Setup_Packet setup_Packet, uint8_t* transfer_Buffer, uint32_t transfer_Length, void URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Create_URB();
	if(p_URB != NULL)
	{
		p_URB -> port_Number 			= port_Number;
		p_URB -> endpoint_Number		= endpoint_Number;
		p_URB -> transfer_Type 			= USB_Host_Transfers___URB_TYPE_CONTROL;
		p_URB -> transfer_Direction 	= USB_Host_Transfers___URB_DIRECTION_IN;
		p_URB -> transfer_Stage			= USB_Host_Transfers___URB_STAGE_SETUP;
		p_URB -> busy					= false;
		p_URB -> device_Address 		= device_Address;
		p_URB -> transfer_Buffer 		= transfer_Buffer;
		p_URB -> transfer_Length 		= transfer_Length;
		p_URB -> URB_Callback			= URB_Callback;
		p_URB -> control_Setup_Packet	= setup_Packet;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

void USB_Host_Transfers___Process_URB_Setup_Stage(USB_Host_Transfers___URB_TypeDef* p_URB)
{
	p_URB -> busy = true;
	uint8_t pipe_Number = USB_Host_Pipes___Create_Pipe
		(
			p_URB->port_Number,
			p_URB->device_Address,
			0,	//			p_URB->pipe_ID
			p_URB->transfer_Type,
			p_URB->transfer_Direction,
			p_URB->endpoint_Number,
			USB_Host_Device_Manager___Device_Get_Out_Endpoint_Max_Packet_Size(p_URB->port_Number, p_URB->device_Address, p_URB->endpoint_Number),
			NULL,
			USB_Host_Transfers___CONTROL_SETUP_PACKET_LENGTH,
			0,
			USB_Host_Device_Manager___Device_Is_Low_Speed_Device(p_URB->port_Number, p_URB->device_Address),
			0,
			USB_Host_Pipes___PID_SETUP
		);
	USB_Host_Pipes___Enable_Pipe(p_URB->port_Number, pipe_Number);
	USB_Host_Pipes___Push_Transfer(p_URB->port_Number, pipe_Number, (uint8_t*)&p_URB->control_Setup_Packet, USB_Host_Transfers___CONTROL_SETUP_PACKET_LENGTH);
}

void USB_Host_Transfers___Process_URB_Data_Stage()
{

}

void USB_Host_Transfers___Process_URB_Status_Stage()
{

}

void USB_Host_Transfers___Process_URB()
{
	if(URB_Queue.current_Node != NULL)
	{
		if(!URB_Queue.current_Node->URB.busy)
		{
			switch(URB_Queue.current_Node->URB.transfer_Stage)
			{
			case USB_Host_Transfers___URB_STAGE_SETUP:
				USB_Host_Transfers___Process_URB_Setup_Stage(&URB_Queue.current_Node->URB);
				break;
			case USB_Host_Transfers___URB_STAGE_DATA:
				USB_Host_Transfers___Process_URB_Data_Stage();
				break;
			case USB_Host_Transfers___URB_STAGE_STATUS:
				USB_Host_Transfers___Process_URB_Status_Stage();
				break;
			}
		}
	}
}
