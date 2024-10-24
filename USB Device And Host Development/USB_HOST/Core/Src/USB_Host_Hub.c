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
#include "../Inc/USB_Host_Transfers.h"

#if USB_Host_Config___DYNAMICALLY_ALLOCATE_USB_HUBS == true

	// Function to allocate a new hub node dynamically
	USB_Host_Hub___Hub_Node_TypeDef* USB_Host_Hub___Allocate_Hub_Node()
	{
		USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node;
		// Allocate memory for the hub node
		p_Hub_Node = (USB_Host_Hub___Hub_Node_TypeDef*)malloc(sizeof(USB_Host_Hub___Hub_Node_TypeDef));
		// Mark the node as allocated
		p_Hub_Node -> is_Allocated 	= 1;
		// Initialize next and previous node pointers
		p_Hub_Node -> next_Node 	= NULL;
		p_Hub_Node -> previous_Node = NULL;
		return(p_Hub_Node);
	}

	// Function to free a dynamically allocated hub node
	void USB_Host_Hub___Free_Hub_Node(USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node)
	{
		// Free the allocated memory
		free(p_Hub_Node);
	}

#else

	// Static allocation of hub nodes
	static USB_Host_Hub___Hub_Node_TypeDef USB_Host_Hub___Hub_Node_Pool[USB_Host_Config___MAX_USB_HUBS];

	// Function to allocate a hub node from the static pool
	USB_Host_Hub___Hub_Node_TypeDef* USB_Host_Hub___Allocate_Hub_Node()
	{
		// Search for an unallocated node in the pool
		for(uint32_t i = 0; i < USB_Host_Config___MAX_USB_HUBS; i++)
		{
			if(!USB_Host_Hub___Hub_Node_Pool[i].is_Allocated)
			{
				// Mark the node as allocated and initialize pointers
				USB_Host_Hub___Hub_Node_Pool[i].is_Allocated 	= true;
				USB_Host_Hub___Hub_Node_Pool[i].next_Node 		= NULL;
				USB_Host_Hub___Hub_Node_Pool[i].previous_Node 	= NULL;
				return(&USB_Host_Hub___Hub_Node_Pool[i]);
			}
		}
		// Return NULL if no unallocated node is found
		return(NULL);
	}

	// Function to free a hub node back to the static pool
	void USB_Host_Hub___Free_Hub_Node(USB_Host_Hub___Hub_Node_TypeDef* p_Enumerator_Node)
	{
		// Mark the node as unallocated
		p_Enumerator_Node -> is_Allocated = false;
	}

#endif

// Queue of hub nodes for each port
static USB_Host_Hub___Hub_Queue_TypeDef USB_Host_Hub___Hub_Queue[USB_Host___NUMBER_OF_PORTS];

// Function to create a new hub node and add it to the queue
USB_Host_Hub___Hub_Node_TypeDef* USB_Host_Hub___Create_Hub_Node(uint8_t port_Number)
{
	// Allocate a new hub node
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Allocate_Hub_Node();
	if(p_Hub_Node != NULL)
	{
		// Initialize node pointers
		p_Hub_Node -> next_Node = NULL;
		p_Hub_Node -> previous_Node = NULL;

		// If the queue is empty, set the first and last node
		if(USB_Host_Hub___Hub_Queue[port_Number].first_Node == NULL)
		{
			USB_Host_Hub___Hub_Queue[port_Number].first_Node = p_Hub_Node;
			USB_Host_Hub___Hub_Queue[port_Number].last_Node  = p_Hub_Node;
		}
		else
		{
			// Add the node to the end of the queue
			p_Hub_Node -> previous_Node 									= USB_Host_Hub___Hub_Queue[port_Number].last_Node;
			USB_Host_Hub___Hub_Queue[port_Number].last_Node -> next_Node 	= p_Hub_Node;
			USB_Host_Hub___Hub_Queue[port_Number].last_Node 				= p_Hub_Node;
		}
	}
	return(p_Hub_Node);
}

// Function to delete a hub node from the queue
void USB_Host_Hub___Delete_Hub_Node(uint8_t port_Number, USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node)
{
	if(p_Hub_Node != NULL)
	{
		if (p_Hub_Node -> previous_Node != NULL)
		{
			// Adjust the previous node's next pointer
			p_Hub_Node -> previous_Node -> next_Node = p_Hub_Node -> next_Node;
			// Adjust the next node's previous pointer
			p_Hub_Node -> next_Node -> previous_Node = p_Hub_Node -> previous_Node;
		}
		else
		{
			// Update the first node in the queue
			USB_Host_Hub___Hub_Queue[port_Number].first_Node = p_Hub_Node -> next_Node;
		}

		if (USB_Host_Hub___Hub_Queue[port_Number].last_Node == p_Hub_Node)
		{
			// Update the last node in the queue
			USB_Host_Hub___Hub_Queue[port_Number].last_Node = p_Hub_Node->previous_Node;
		}

		// Free the hub node
		USB_Host_Hub___Free_Hub_Node(p_Hub_Node);
	}
}

// Function to retrieve a hub node based on device address
USB_Host_Hub___Hub_Node_TypeDef* USB_Host_Hub___Get_Hub_Node_From_Device_Address(uint8_t port_Number, uint8_t device_Address)
{
	if (USB_Host_Hub___Hub_Queue[port_Number].first_Node != NULL)
	{
		USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Hub_Queue[port_Number].first_Node;
		while (p_Hub_Node != NULL)
		{
			// Check if the device address matches
			if (p_Hub_Node->hub.device_Address == device_Address)
			{
				return (p_Hub_Node);
			}
			p_Hub_Node = p_Hub_Node->next_Node;
		}
	}
	return(NULL);
}

// Forward declaration of the URB setup callback function
void USB_Host_Hub___URB_Setup_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS);

// Function to set the configuration of the hub
void USB_Host_Hub___Set_Configuration(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet setup_Packet;

	// Prepare the setup packet for SET_CONFIGURATION request
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_STANDARD_HOST_TO_DEVICE;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_SET_CONFIGURATION;
	setup_Packet.wValue 		= configuration_Index;
	setup_Packet.wIndex 		= 0;
	setup_Packet.wLength 		= 0;

	// Initiate the control transfer
	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_OUT, setup_Packet, 0, 0, USB_Host_Hub___STANDARD_NUMBER_OF_RETRIES, callback);
}

// Function to get the hub descriptor
void USB_Host_Hub___Get_Hub_Descriptor(uint8_t port_Number, uint8_t device_Address, uint8_t* p_Buffer, uint8_t descriptor_Length, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet setup_Packet;

	// Prepare the setup packet for GET_DESCRIPTOR request
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_DEVICE_TO_HOST;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_DESCRIPTOR;
	setup_Packet.wValue 		= (USB_Host_Hub___HUB_DESCRIPTOR_TYPE << 8);
	setup_Packet.wIndex 		= 0;
	setup_Packet.wLength 		= descriptor_Length;

	// Initiate the control transfer
	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_IN, setup_Packet, p_Buffer, descriptor_Length, USB_Host_Hub___STANDARD_NUMBER_OF_RETRIES, callback);
}

// Function to set a feature on a hub port
void USB_Host_Hub___Set_Port_Feature(uint8_t port_Number, uint8_t device_Address, uint8_t hub_Port_Number, uint8_t main_Feature_Selector, uint8_t sub_Feature_Selector, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet setup_Packet;

	// Prepare the setup packet for SET_FEATURE request
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_HOST_TO_OTHER;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_SET_FEATURE;
	setup_Packet.wValue 		= main_Feature_Selector;
	setup_Packet.wIndex 		= hub_Port_Number | (sub_Feature_Selector << 8);
	setup_Packet.wLength 		= 0;

	// Initiate the control transfer
	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_OUT, setup_Packet, 0, 0, USB_Host_Hub___STANDARD_NUMBER_OF_RETRIES, callback);
}

// Function to clear a feature on a hub port
void USB_Host_Hub___Clear_Port_Feature(uint8_t port_Number, uint8_t device_Address, uint8_t hub_Port_Number, uint8_t main_Feature_Selector, uint8_t sub_Feature_Selector, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet setup_Packet;

	// Prepare the setup packet for CLEAR_FEATURE request
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_HOST_TO_OTHER;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_CLEAR_FEATURE;
	setup_Packet.wValue 		= main_Feature_Selector;
	setup_Packet.wIndex 		= hub_Port_Number | (sub_Feature_Selector << 8);
	setup_Packet.wLength 		= 0;

	// Initiate the control transfer
	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_OUT, setup_Packet, 0, 0, USB_Host_Hub___STANDARD_NUMBER_OF_RETRIES, callback);
}

// Function to advance the hub setup stage
void USB_Host_Hub___Set_Next_Setup_Stage(USB_Host_Hub___Hub_Node_TypeDef* p_USB_Hub_Node)
{
	switch(p_USB_Hub_Node->hub.setup_Stage)
	{
	case USB_Host_Hub___HUB_SETUP_STAGE_SET_CONFIGURATION:
		p_USB_Hub_Node->hub.setup_Stage = USB_Host_Hub___HUB_SETUP_STAGE_GET_SHORT_HUB_DESCRIPTOR;
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_GET_SHORT_HUB_DESCRIPTOR:
		p_USB_Hub_Node->hub.setup_Stage = USB_Host_Hub___HUB_SETUP_STAGE_GET_FULL_HUB_DESCRIPTOR;
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_GET_FULL_HUB_DESCRIPTOR:
		p_USB_Hub_Node->hub.setup_Stage = USB_Host_Hub___HUB_SETUP_STAGE_ENABLE_PORTS;
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_ENABLE_PORTS:
		p_USB_Hub_Node->hub.setup_Stage = USB_Host_Hub___HUB_SETUP_STAGE_COMPLETE;
		break;
	}
}

// Generic URB callback function for hub operations
void USB_Host_Hub___Generic_URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	// Retrieve the hub node associated with the URB
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(URB.port_Number, URB.device_Address);

	if(p_Hub_Node != NULL)
	{
		// Placeholder for additional processing
		uint8_t i = 0;
	}
}

// Function to handle changes in port reset status
void USB_Host_Hub___Handle_Change_In_Port_Reset_Status(uint8_t port_Number, uint8_t device_Address, uint8_t hub_Port_Number, uint16_t wPortChange, uint16_t wPortStatus)
{
	// Clear the C_PORT_RESET feature
	USB_Host_Hub___Clear_Port_Feature(port_Number, device_Address, hub_Port_Number, USB_Host_Hub___FEATURE_C_PORT_RESET, 0, USB_Host_Hub___Generic_URB_Callback);
<<<<<<< Upstream, based on origin/main
	// Enable the device on the port
	USB_Host_Device_Manager___Enable_Device(port_Number, 0);
=======
	if(wPortStatus & USB_Host_Hub___wPortStatus_PORT_CONNECTION_Msk)
	{
		USB_Host_Device_Manager___Enable_Device(port_Number, 0);
	}
>>>>>>> bdbfeb5 debugging, not finished
}

// Function to handle port connection events
void USB_Host_Hub___Handle_Port_Connection(uint8_t port_Number, uint8_t device_Address, uint8_t hub_Port_Number, uint16_t wPortChange, uint16_t wPortStatus)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(port_Number, device_Address);

	if(USB_Host_Device_Manager___Is_Port_Open(port_Number))
	{
		uint8_t device_Speed;

		// Determine the device speed based on port status
		if(((wPortStatus & USB_Host_Hub___wPortStatus_PORT_LOW_SPEED_Msk) >> USB_Host_Hub___wPortStatus_PORT_LOW_SPEED_Pos) == true)
		{
			device_Speed = USB_Host_Device_Manager___LOW_SPEED_DEVICE;
		}
		else if(((wPortStatus & USB_Host_Hub___wPortStatus_PORT_HIGH_SPEED_Msk) >> USB_Host_Hub___wPortStatus_PORT_HIGH_SPEED_Pos) == true)
		{
			device_Speed = USB_Host_Device_Manager___HIGH_SPEED_DEVICE;
		}
		else
		{
			device_Speed = USB_Host_Device_Manager___FULL_SPEED_DEVICE;
		}

		// Allocate a device at address zero
		int8_t allocation_Result = USB_Host_Device_Manager___Allocate_Device_At_Address_Zero(port_Number, device_Speed, false);

		if(allocation_Result == EXIT_SUCCESS)
		{
			// Assign the device to the hub port
			p_Hub_Node->hub.port[hub_Port_Number].p_Device = USB_Host_Device_Manager___Get_Device_Pointer(port_Number, 0);
			// Clear the C_PORT_CONNECTION feature
			USB_Host_Hub___Clear_Port_Feature(port_Number, device_Address, hub_Port_Number, USB_Host_Hub___FEATURE_C_PORT_CONNECTION, 0, USB_Host_Hub___Generic_URB_Callback);
			// Set the PORT_RESET feature
			USB_Host_Hub___Set_Port_Feature(port_Number, device_Address, hub_Port_Number, USB_Host_Hub___FEATURE_PORT_RESET, 0, USB_Host_Hub___Generic_URB_Callback);
		}
	}
}

<<<<<<< Upstream, based on origin/main
// Variable to store the address of a disconnected device
uint8_t disconnected_Device_Address = 255;

// Function to handle port disconnection events
=======
>>>>>>> bdbfeb5 debugging, not finished
void USB_Host_Hub___Handle_Port_Disconnection(uint8_t port_Number, uint8_t device_Address, uint8_t hub_Port_Number, uint16_t wPortChange, uint16_t wPortStatus)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(port_Number, device_Address);

	// Clear the C_PORT_CONNECTION feature
	USB_Host_Hub___Clear_Port_Feature(port_Number, device_Address, hub_Port_Number, USB_Host_Hub___FEATURE_C_PORT_CONNECTION, 0, USB_Host_Hub___Generic_URB_Callback);

	if(p_Hub_Node->hub.port[hub_Port_Number].p_Device != NULL)
	{
<<<<<<< Upstream, based on origin/main
		// Retrieve the disconnected device's address
		disconnected_Device_Address = p_Hub_Node->hub.port[hub_Port_Number].p_Device->status.current_USB_Address;
		// Notify the device manager about the disconnection
=======
>>>>>>> bdbfeb5 debugging, not finished
		USB_Host_Device_Manager___Device_Disconnected(port_Number, p_Hub_Node->hub.port[hub_Port_Number].p_Device->status.current_USB_Address);
		// Clear the device pointer
		p_Hub_Node->hub.port[hub_Port_Number].p_Device = NULL;
	}
}

// Function to handle changes in port connection status
void USB_Host_Hub___Handle_Change_In_Port_Connection_Status(uint8_t port_Number, uint8_t device_Address, uint8_t hub_Port_Number, uint16_t wPortChange, uint16_t wPortStatus)
{
	if(wPortStatus & USB_Host_Hub___wPortStatus_PORT_CONNECTION_Msk >> USB_Host_Hub___wPortStatus_PORT_CONNECTION_Pos)
	{
		// Handle port connection
		USB_Host_Hub___Handle_Port_Connection(port_Number, device_Address, hub_Port_Number, wPortChange, wPortStatus);
	}
	else
	{
		// Handle port disconnection
		USB_Host_Hub___Handle_Port_Disconnection(port_Number, device_Address, hub_Port_Number, wPortChange, wPortStatus);
	}
}

// Callback function for GET_PORT_STATUS URB
void USB_Host_Hub___Get_Port_Status_URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(URB.port_Number, URB.device_Address);

	if(p_Hub_Node != NULL)
	{
		// Extract port status and change information
		uint16_t wPortStatus 	= *((uint16_t*)&(URB.transfer_Buffer[0]));
		uint16_t wPortChange 	= *((uint16_t*)&(URB.transfer_Buffer[2]));
		uint8_t	hub_Port_Number = URB.control_Setup_Packet.wIndex;
		while(wPortChange)
		{
			// Handle different port change events
			switch(POSITION_VAL(wPortChange))
			{
			case USB_Host_Hub___wPortChange_C_PORT_CONNECTION_Pos:
				USB_Host_Hub___Handle_Change_In_Port_Connection_Status(URB.port_Number, URB.device_Address, hub_Port_Number, wPortChange, wPortStatus);
				wPortChange &= ~(1 << USB_Host_Hub___wPortChange_C_PORT_CONNECTION_Pos);
				break;
			case USB_Host_Hub___wPortChange_C_PORT_RESET_Pos:
				USB_Host_Hub___Handle_Change_In_Port_Reset_Status(URB.port_Number, URB.device_Address, hub_Port_Number, wPortChange, wPortStatus);
				wPortChange &= ~(1 << USB_Host_Hub___wPortChange_C_PORT_RESET_Pos);
				break;
			}
		}
	}
}

// Function to get the status of a hub port
void USB_Host_Hub___Get_Port_Status(uint8_t port_Number, uint8_t device_Address, uint8_t hub_Port_Number, uint8_t* p_Buffer, void callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS))
{
	USB_Host_Transfers___Control_Setup_Packet setup_Packet;
	// Prepare the setup packet for GET_STATUS request
	setup_Packet.bmRequestType 	= USB_Host_Transfers___CONTROL_SETUP_PACKET_BMREQUESTTYPE_CLASS_OTHER_TO_HOST;
	setup_Packet.bRequest 		= USB_Host_Transfers___CONTROL_SETUP_PACKET_BREQUEST_GET_STATUS;
	setup_Packet.wValue 		= 0;
	setup_Packet.wIndex 		= hub_Port_Number;
	setup_Packet.wLength 		= 4;

	// Initiate the control transfer
	USB_Host_Transfers___Control_Transfer(port_Number, device_Address, USB_Host___ENDPOINT_ZERO, USB_Host___TRANSFER_DIRECTION_IN, setup_Packet, p_Buffer, 4, USB_Host_Hub___STANDARD_NUMBER_OF_RETRIES, callback);
}

// Callback function for interrupt URB
void USB_Host_Hub___Interrupt_URB_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(URB.port_Number, URB.device_Address);

	if(p_Hub_Node != NULL)
	{
		if(URB.transfer_Status == USB_Host_Transfers___URB_STATUS_SUCCESS)
		{
			// Process the interrupt data
			for(uint8_t i = 0; i < URB.transfer_Length; i++)
			{
				uint8_t port_Mask = URB.transfer_Buffer[i];
				uint8_t hub_Port_Number;
				while(port_Mask)
				{
					// Determine which port has changed
					hub_Port_Number = POSITION_VAL(port_Mask) + (i * 8);
					// Get the status of the port
					USB_Host_Hub___Get_Port_Status(URB.port_Number, URB.device_Address, hub_Port_Number, p_Hub_Node->hub.interrupt_Endpoint_Data_Buffer, USB_Host_Hub___Get_Port_Status_URB_Callback);
					port_Mask &= ~(1 << hub_Port_Number);
				}
			}
		}
	}
}

// Callback function for hub polling
void USB_Host_Hub___Polling_Callback(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(port_Number, device_Address);
	// Determine if the frame number is odd or even
	uint8_t odd_Frame = USB_Host___Get_Frame_Number(port_Number) %2;
	// Initiate an interrupt transfer to poll the hub
	USB_Host_Transfers___Interrupt_Transfer(port_Number, device_Address, p_Hub_Node->hub.interrupt_Endpoint_Number, USB_Host_Transfers___URB_DIRECTION_IN, p_Hub_Node->hub.interrupt_Endpoint_Data_Buffer, p_Hub_Node->hub.interrupt_Endpoint_Packet_Size, 0, odd_Frame, 1, USB_Host_Hub___Interrupt_URB_Callback);
}

// Function to perform the next setup stage for the hub
void USB_Host_Hub___Do_Setup_Stage(USB_Host_Hub___Hub_Node_TypeDef* p_USB_Hub_Node)
{
	switch(p_USB_Hub_Node->hub.setup_Stage)
	{
	case USB_Host_Hub___HUB_SETUP_STAGE_SET_CONFIGURATION:
		// Set the configuration of the hub
		USB_Host_Hub___Set_Configuration(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, 1, USB_Host_Hub___URB_Setup_Callback);
		USB_Host_Device_Manager___Set_Configuration(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, 0);
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_GET_SHORT_HUB_DESCRIPTOR:
		// Get the short hub descriptor
		USB_Host_Hub___Get_Hub_Descriptor(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, (uint8_t*)&(p_USB_Hub_Node->hub.descriptor), USB_Host_Hub___HUB_DESCRIPTOR_BASE_LENGTH, USB_Host_Hub___URB_Setup_Callback);
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_GET_FULL_HUB_DESCRIPTOR:
		// Get the full hub descriptor
		USB_Host_Hub___Get_Hub_Descriptor(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, (uint8_t*)&(p_USB_Hub_Node->hub.descriptor), p_USB_Hub_Node->hub.descriptor.bDescriptorLength, USB_Host_Hub___URB_Setup_Callback);
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_ENABLE_PORTS:
		// Enable power on all ports except the last one
		for(uint8_t i = 0; i < p_USB_Hub_Node->hub.descriptor.bNumberOfPorts -1; i++)
		{
			USB_Host_Hub___Set_Port_Feature(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, i+1, USB_Host_Hub___FEATURE_PORT_POWER, 0, USB_Host_Hub___Generic_URB_Callback);
		}

		// Enable power on the last port and proceed to the next stage
		USB_Host_Hub___Set_Port_Feature(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, p_USB_Hub_Node->hub.descriptor.bNumberOfPorts, USB_Host_Hub___FEATURE_PORT_POWER, 0, USB_Host_Hub___URB_Setup_Callback);
		break;
	case USB_Host_Hub___HUB_SETUP_STAGE_COMPLETE:
		// Add the hub to the polling list
		USB_Host_Device_Manager___Add_Polling_Device(p_USB_Hub_Node->hub.port_Number, p_USB_Hub_Node->hub.device_Address, p_USB_Hub_Node->hub.polling_Interval, USB_Host_Hub___Polling_Callback);
		break;
	}
}

// Callback function for hub setup URBs
void USB_Host_Hub___URB_Setup_Callback(USB_Host_Transfers___URB_CALLBACK_PARAMETERS)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(URB.port_Number, URB.device_Address);

	if(p_Hub_Node != NULL)
	{
		// Advance to the next setup stage
		USB_Host_Hub___Set_Next_Setup_Stage(p_Hub_Node);
		// Perform the setup stage
		USB_Host_Hub___Do_Setup_Stage(p_Hub_Node);
	}
}

// Callback function for hub disconnection
void USB_Host_Hub___Disconnection_Callback(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Hub___Hub_Node_TypeDef* p_Hub_Node = USB_Host_Hub___Get_Hub_Node_From_Device_Address(port_Number, device_Address);
	// Delete the hub node from the queue
	USB_Host_Hub___Delete_Hub_Node(port_Number, p_Hub_Node);
}

// Function to initiate a hub
void USB_Host_Hub___Initiate_Hub(uint8_t port_Number, uint8_t device_Address)
{
	// Create a new hub node
	USB_Host_Hub___Hub_Node_TypeDef* p_USB_Hub_Node = USB_Host_Hub___Create_Hub_Node(port_Number);
	if(p_USB_Hub_Node != NULL)
	{
		// Get the endpoint descriptor for the hub's interrupt endpoint
		USB_Host___Endpoint_Descriptor_TypeDef* p_Endpoint_Descriptor 	= USB_Host_Device_Manager___Device_Get_Endpoint_Descriptor(port_Number, device_Address, 0, 0, 0);
		// Store the polling interval and endpoint information
		p_USB_Hub_Node->hub.polling_Interval 							= p_Endpoint_Descriptor->bInterval;
		p_USB_Hub_Node->hub.interrupt_Endpoint_Number 					= p_Endpoint_Descriptor->bEndpointAddress & 0x0f;
		p_USB_Hub_Node->hub.interrupt_Endpoint_Packet_Size 				= p_Endpoint_Descriptor->wMaxPacketSize;
		p_USB_Hub_Node->hub.setup_Stage 								= USB_Host_Hub___HUB_SETUP_STAGE_SET_CONFIGURATION;
		p_USB_Hub_Node->hub.device_Address 								= device_Address;
		p_USB_Hub_Node->hub.port_Number 								= port_Number;
		// Add a disconnection callback
		USB_Host_Device_Manager___Device_Add_Disconnection_Callback(port_Number, device_Address, USB_Host_Hub___Disconnection_Callback);
		// Start the setup process
		USB_Host_Hub___Do_Setup_Stage(p_USB_Hub_Node);
	}
}
