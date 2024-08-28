/*
 * USB_Host_Hub.cpp
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#include <stdint.h>
#include <stdlib.h>
#include "../Inc/USB_Host_Hub.h"
#include "../Inc/USB_Host_Device_Manager.h"

#if USB_Host_Config___DYNAMICALLY_ALLOCATE_USB_HUBS == true

	USB_Host_Hub___Hub_Node_TypeDef* USB_Host_Hub___Allocate_Hub_Node()
	{
		USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node;
		p_Hub_Node = (USB_Host_Hub___Hub_Node_TypeDef*)malloc(sizeof(USB_Host_Hub___Hub_Node_TypeDef));
		p_Hub_Node -> is_Allocated 	= 1;
		p_Hub_Node -> next_Node 	= NULL;
		p_Hub_Node -> previous_Node = NULL;
		return(p_Hub_Node);
	}

	void USB_Host_Hub___Free_Hub_Node(USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node)
	{
		free(p_Hub_Node);
	}

#else

	static USB_Host_Hub___Hub_Node_TypeDef USB_Host_Hub___Hub_Node_Pool[USB_Host_Config___MAX_USB_HUBS];

	USB_Host_Hub___Hub_Node_TypeDef* USB_Host_Hub___Allocate_Hub_Node()
	{
		for(uint32_t i = 0; i < USB_Host_Config___MAX_USB_HUBS; i++)
		{
			if(!USB_Host_Hub___Hub_Node_Pool[i].is_Allocated)
			{
				USB_Host_Hub___Hub_Node_Pool[i].is_Allocated 	= true;
				USB_Host_Hub___Hub_Node_Pool[i].next_Node 		= NULL;
				USB_Host_Hub___Hub_Node_Pool[i].previous_Node 	= NULL;
				return(&USB_Host_Hub___Hub_Node_Pool[i]);
			}
		}
		return(NULL);
	}

	void USB_Host_Hub___Free_Hub_Node(USB_Host_Hub___Hub_Node_TypeDef* p_Enumerator_Node)
	{
		p_Enumerator_Node -> is_Allocated = false;
	}

#endif

static USB_Host_Hub___Hub_Queue_TypeDef USB_Host_Hub___Hub_Queue[USB_Host___NUMBER_OF_PORTS];

USB_Host_Hub___Hub_Node_TypeDef* USB_Host_Hub___Create_Enumerator(uint8_t port_Number)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Allocate_Hub_Node();
	if(p_Hub_Node != NULL)
	{
		p_Hub_Node -> next_Node = NULL;
		p_Hub_Node -> previous_Node = NULL;

		if(USB_Host_Hub___Hub_Queue[port_Number].first_Node == NULL)
		{
			USB_Host_Hub___Hub_Queue[port_Number].first_Node = p_Hub_Node;
			USB_Host_Hub___Hub_Queue[port_Number].last_Node  = p_Hub_Node;
		}
		else
		{
			USB_Host_Hub___Hub_Queue[port_Number].last_Node -> next_Node 	= p_Hub_Node;
			USB_Host_Hub___Hub_Queue[port_Number].last_Node 				= p_Hub_Node;
			p_Hub_Node -> previous_Node 									= USB_Host_Hub___Hub_Queue[port_Number].last_Node;
		}
	}
	return(p_Hub_Node);
}

void USB_Host_Hub___Delete_Hub_Node(uint8_t port_Number, USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node)
{
	if(p_Hub_Node != NULL)
	{
		if (p_Hub_Node -> previous_Node != NULL)
		{
			p_Hub_Node -> previous_Node -> next_Node = p_Hub_Node -> next_Node;
			p_Hub_Node -> next_Node -> previous_Node = p_Hub_Node -> previous_Node;
		}
		else
		{
			USB_Host_Hub___Hub_Queue[port_Number].first_Node = p_Hub_Node -> next_Node;
		}

		USB_Host_Hub___Free_Hub_Node(p_Hub_Node);
	}
}

void USB_Host_Hub___Do_Setup_Stage(USB_Host_Hub___Hub_Node_TypeDef* p_USB_Hub_Node)
{

}

void USB_Host_Hub___Initiate_Hub(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_USB_Hub_Node = USB_Host_Hub___Allocate_Hub_Node();
	if(p_USB_Hub_Node != NULL)
	{
		p_USB_Hub_Node->hub.hub_Setup_Stage = USB_Host_Hub___HUB_SETUP_STAGE_SET_CONFIGURATION;
		p_USB_Hub_Node->hub.address 		= device_Address;
		p_USB_Hub_Node->hub.port_Number 	= port_Number;
		USB_Host_Hub___Do_Setup_Stage(p_USB_Hub_Node);
	}
}
