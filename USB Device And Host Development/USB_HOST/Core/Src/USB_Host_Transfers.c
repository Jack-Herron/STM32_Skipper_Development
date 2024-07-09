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
	static USB_Host_Transfers___URB_TypeDef* USB_Host_Transfers___URB_Pointers[USB_Host_Config___MAX_USB_REQUEST_BLOCKS];

	USB_Host_Transfers___URB_TypeDef* USB_Host_URB___Allocate_URB()
	{
		USB_Host_Transfers___URB_TypeDef* p_URB;
		p_URB = (USB_Host_Transfers___URB_TypeDef*)malloc(sizeof(USB_Host_Transfers___URB_TypeDef));
		if(p_URB != NULL)
		{
			for(uint32_t i = 0; i < USB_Host_Config___MAX_USB_REQUEST_BLOCKS; i++)
			{
				if(USB_Host_Transfers___URB_Pointers[i] != NULL)
				{
					USB_Host_Transfers___URB_Pointers[i] = p_URB;
					p_URB -> URB_ID = i;
					return(p_URB);
				}
			}
			free(p_URB);
		}
		return(NULL);
	}

	void USB_Host_Transfers___Free_URB(uint8_t URB_ID)
	{
		USB_Host_Transfers___URB_Pointers[URB_ID] = NULL;
		free(USB_Host_Transfers___URB_Pointers[URB_ID]);
	}
#else
	static USB_Host_Transfers___URB_TypeDef USB_Host_Transfers___URB_Pool[USB_Host_Config___MAX_USB_REQUEST_BLOCKS];
	static uint8_t USB_Host_Transfers___URB_Is_Allocated[USB_Host_Config___MAX_USB_REQUEST_BLOCKS];
	USB_Host_Transfers___URB_TypeDef* USB_Host_Transfers___Allocate_URB()
	{
		for(uint32_t i = 0; i < USB_Host_Config___MAX_USB_REQUEST_BLOCKS; i++)
		{
			if(!USB_Host_Transfers___URB_Is_Allocated[i])
			{
				USB_Host_Transfers___URB_Is_Allocated[i] = true;
				USB_Host_Transfers___URB_Pool[i].URB_ID = i;
				return(&USB_Host_Transfers___URB_Pool[i]);
			}
		}
		return(NULL);
	}

	void USB_Host_Transfers___Free_URB(uint8_t URB_ID)
	{
		USB_Host_Transfers___URB_Is_Allocated[URB_ID] = false;
	}
#endif

int8_t USB_Host_Transfers___Isochronous_Transfer_Out()
{
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Interrupt_Transfer_Out()
{
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Bulk_Transfer_Out()
{
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Control_Transfer_Out()
{
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Isochronous_Transfer_In()
{
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Interrupt_Transfer_In()
{
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Bulk_Transfer_In()
{
	return(EXIT_FAILURE);
}

int8_t USB_Host_Transfers___Control_Transfer_In()
{
	return(EXIT_FAILURE);
}
