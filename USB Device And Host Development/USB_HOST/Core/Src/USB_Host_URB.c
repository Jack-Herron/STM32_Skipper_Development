/*
 * USB_Host_URB.c
 *
 *  Created on: Jul 8, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stdbool.h>

#include "../Inc/USB_Host_URB.h"
#include "../../Config/USB_Host_Config.h"

#if USB_Host_Config___DYNAMICALLY_ALLOCATE_URB
	USB_Host_URB___URB_TypeDef* USB_Host_URB___Allocate_URB()
	{
		USB_Host_URB___URB_TypeDef* p_URB;
		p_URB = (USB_Host_URB___URB_TypeDef*)malloc(sizeof(USB_Host_URB___URB_TypeDef));
		return(p_URB);
	}

	void USB_Host_URB___Free_URB(USB_Host_URB___URB_TypeDef* p_URB)
	{
		free(p_URB);
	}
#else
	static USB_Host_URB___URB_TypeDef USB_Host_URB___URB_Pool[USB_Host_Config___URB_POOL_SIZE];

	USB_Host_URB___URB_TypeDef* USB_Host_URB___Allocate_URB()
	{
		for(uint32_t i = 0; i < USB_Host_Config___URB_POOL_SIZE; i++)
		{
			if(!USB_Host_URB___URB_Pool[i].is_Allocated)
			{
				USB_Host_URB___URB_Pool[i].is_Allocated = true;
				return(&USB_Host_URB___URB_Pool[i]);
			}
		}
		return(NULL);
	}

	void USB_Host_URB___Free_URB(USB_Host_URB___URB_TypeDef* p_URB)
	{
		p_URB -> is_Allocated = false;
	}
#endif
