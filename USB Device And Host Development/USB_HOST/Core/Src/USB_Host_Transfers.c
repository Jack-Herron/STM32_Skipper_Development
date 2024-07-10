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

#if USB_Host_Config___DYNAMICALLY_ALLOCATE_USB_REQUEST_BLOCKS

	USB_Host_Transfers___URB_TypeDef* USB_Host_URB___Allocate_URB()
	{
		USB_Host_Transfers___URB_TypeDef* p_URB;
		p_URB = (USB_Host_Transfers___URB_TypeDef*)malloc(sizeof(USB_Host_Transfers___URB_TypeDef));
		return(p_URB);
	}

	void USB_Host_Transfers___Free_URB(USB_Host_Transfers___URB_TypeDef* p_URB)
	{
		free(p_URB);
	}

#else
	static USB_Host_Transfers___URB_TypeDef USB_Host_Transfers___URB_Pool[USB_Host_Config___MAX_USB_REQUEST_BLOCKS];

	USB_Host_Transfers___URB_TypeDef* USB_Host_Transfers___Allocate_URB()
	{
		for(uint32_t i = 0; i < USB_Host_Config___MAX_USB_REQUEST_BLOCKS; i++)
		{
			if(!USB_Host_Transfers___URB_Pool[i].is_Allocated)
			{
				USB_Host_Transfers___URB_Pool[i].is_Allocated = true;
				return(&USB_Host_Transfers___URB_Pool[i]);
			}
		}
		return(NULL);
	}

	void USB_Host_Transfers___Free_URB(USB_Host_Transfers___URB_TypeDef* p_URB)
	{
		p_URB -> is_Allocated = false;
	}
#endif

int8_t USB_Host_Transfers___Isochronous_Transfer_Out(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length)
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Allocate_URB();
	if(p_URB != NULL)
	{
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_ISOCHRONOUS;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_OUT;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Interrupt_Transfer_Out(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length)
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Allocate_URB();
	if(p_URB != NULL)
	{
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_INTERRUPT;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_OUT;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Bulk_Transfer_Out(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length)
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Allocate_URB();
	if(p_URB != NULL)
	{
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_BULK;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_OUT;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Control_Transfer_Out(uint8_t device_Address, uint8_t* setup_Packet, uint8_t* transfer_Buffer, uint32_t transfer_Length)
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Allocate_URB();
	if(p_URB != NULL)
	{
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_CONTROL;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_OUT;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_SETUP;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;

		for(uint8_t i = 0; i < USB_Host_Transfers___CONTROL_SETUP_PACKET_LENGTH; i++)
		{
			p_URB -> control_Setup_Packet[i] = setup_Packet[i];
		}

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Isochronous_Transfer_In(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length)
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Allocate_URB();
	if(p_URB != NULL)
	{
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_ISOCHRONOUS;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_IN;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Interrupt_Transfer_In(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length)
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Allocate_URB();
	if(p_URB != NULL)
	{
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_INTERRUPT;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_IN;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Bulk_Transfer_In(uint8_t device_Address, uint8_t* transfer_Buffer, uint32_t transfer_Length)
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Allocate_URB();
	if(p_URB != NULL)
	{
		p_URB -> transfer_Type		= USB_Host_Transfers___URB_TYPE_BULK;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_IN;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_DATA;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Control_Transfer_In(uint8_t device_Address, uint8_t* setup_Packet, uint8_t* transfer_Buffer, uint32_t transfer_Length)
{
	USB_Host_Transfers___URB_TypeDef* p_URB = USB_Host_Transfers___Allocate_URB();
	if(p_URB != NULL)
	{
		p_URB -> transfer_Type 		= USB_Host_Transfers___URB_TYPE_CONTROL;
		p_URB -> transfer_Direction = USB_Host_Transfers___URB_DIRECTION_IN;
		p_URB -> transfer_Stage		= USB_Host_Transfers___URB_STAGE_SETUP;
		p_URB -> busy				= false;
		p_URB -> device_Address 	= device_Address;
		p_URB -> transfer_Buffer 	= transfer_Buffer;
		p_URB -> transfer_Length 	= transfer_Length;

		for(uint8_t i = 0; i < USB_Host_Transfers___CONTROL_SETUP_PACKET_LENGTH; i++)
		{
			p_URB -> control_Setup_Packet[i] = setup_Packet[i];
		}

		return(EXIT_SUCCESS);
	}
	return(EXIT_FAILURE);
}
