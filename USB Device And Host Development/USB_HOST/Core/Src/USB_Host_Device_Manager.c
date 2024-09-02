/*
 * USB_Host_Device_Manager.c
 *
 *  Created on: Jun 28, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <stdbool.h>

#include "../Inc/USB_Host.h"
#include "../../Config/USB_Host_Config.h"
#include "../Inc/USB_Host_Device_Manager.h"



static USB_Host_Device_Manager___Port_TypeDef  	 USB_Host_Device_Manager___Port[USB_Host___NUMBER_OF_PORTS] = {0};

#if USB_Host_Config___DYNAMICALLY_ALLOCATE_DEVICES == false

	USB_Host_Device_Manager___Device_TypeDef device_Pool[USB_Host_Device_Manager___DEVICE_POOL_SIZE] = {0};

	USB_Host_Device_Manager___Device_TypeDef* USB_Host_Device_Manager___Allocate_Device()
	{
		for(uint16_t i = 0; i < USB_Host_Device_Manager___DEVICE_POOL_SIZE; i++)
		{
			if(device_Pool[i].is_Allocated == false)
			{
				device_Pool[i].is_Allocated = true;
				return(&device_Pool[i]);
			}
		}
		return(NULL);
	}

	void USB_Host_Device_Manager___Free_Device(USB_Host_Device_Manager___Device_TypeDef* p_Device)
	{
		p_Device -> is_Allocated = false;
	}

#else

	USB_Host_Device_Manager___Device_TypeDef* USB_Host_Device_Manager___Allocate_Device()
	{
		USB_Host_Device_Manager___Device_TypeDef* p_Device = NULL;

		p_Device = (USB_Host_Device_Manager___Device_TypeDef*)malloc(sizeof(USB_Host_Device_Manager___Device_TypeDef));
		if(p_Device != NULL)
		{
			p_Device -> is_Allocated = true;
		}
		return(p_Device);
	}

	void USB_Host_Device_Manager___Free_Device(USB_Host_Device_Manager___Device_TypeDef* p_Device)
	{
		free(p_Device);
	}

#endif // --------------------------------------------------------------------------------------------------------

USB_Host_Device_Manager___Polling_Device_List_Typedef	USB_Host_Device_Manager___Polling_List[2] = {0};

void USB_Host_Device_Manager___Add_Polling_Device(uint8_t port_Number, uint8_t device_Address, uint16_t polling_Period, void callback(uint8_t port_Number , uint8_t device_Address))
{
	USB_Host_Device_Manager___Polling_Device_Node_TypeDef* p_Polling_Node = &(USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->polling_Process);

	if(p_Polling_Node != NULL)
	{
		p_Polling_Node -> enabled														= true;
		p_Polling_Node -> device_Address 												= device_Address;
		p_Polling_Node -> polling_Target 												= polling_Period;
		p_Polling_Node -> polling_Counter 												= 0;
		p_Polling_Node -> callback 														= callback;

		p_Polling_Node -> next_Node														= NULL;
		p_Polling_Node -> previous_Node													= NULL;

		if(USB_Host_Device_Manager___Polling_List->first_Node != NULL)
		{
			USB_Host_Device_Manager___Polling_List[port_Number].last_Node -> next_Node 	= p_Polling_Node;
			USB_Host_Device_Manager___Polling_List[port_Number].last_Node 				= p_Polling_Node;
			p_Polling_Node->previous_Node												= USB_Host_Device_Manager___Polling_List[port_Number].last_Node;
		}
		else
		{
			USB_Host_Device_Manager___Polling_List->last_Node 							= p_Polling_Node;
			USB_Host_Device_Manager___Polling_List->first_Node 							= p_Polling_Node;
		}
	}
}

uint8_t USB_Host_Device_Manager___Get_Device_Class(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.p_Device_Descriptor->bDeviceClass);
}

USB_Host___Configuration_Descriptor_TypeDef* USB_Host_Device_Manager___Device_Get_Configuration_Descriptor(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.configuration[configuration_Index].p_Configuration_Descriptor);
}

USB_Host___Interface_Descriptor_TypeDef* USB_Host_Device_Manager___Device_Get_Interface_Descriptor(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index, uint8_t interface_Index)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.configuration[configuration_Index].interface[interface_Index].p_Interface_Descriptor);
}

USB_Host___Endpoint_Descriptor_TypeDef* USB_Host_Device_Manager___Device_Get_Endpoint_Descriptor(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index, uint8_t interface_Index, uint8_t endpoint_Index)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.configuration[configuration_Index].interface[interface_Index].endpoint[endpoint_Index].p_Endpoint_Descriptor);
}

uint8_t* USB_Host_Device_Manager___Get_Device_Descriptor_Buffer(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.device_Descriptor_Buffer);
}

uint16_t* USB_Host_Device_Manager___Get_Language_ID_List(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.p_Language_ID_List);
}

uint16_t* USB_Host_Device_Manager___Get_Manufacturer_String(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.p_Manufacturer_String);
}

uint16_t* USB_Host_Device_Manager___Get_Product_String(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.p_Product_String);
}

uint16_t* USB_Host_Device_Manager___Get_Serial_Number_String(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.p_Serial_Number_String);
}

uint8_t USB_Host_Device_Manager___Get_Serial_Number_String_Length(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.serial_Number_String_Length);
}

uint8_t USB_Host_Device_Manager___Get_Manufacturer_String_Length(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.manufacturer_String_Length);
}

uint8_t USB_Host_Device_Manager___Get_Product_String_Length(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.product_String_Length);
}

uint8_t USB_Host_Device_Manager___Get_Language_ID_List_Length(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.language_ID_List_Length);
}

uint8_t USB_Host_Device_Manager___Get_String_Descriptor_Length(uint8_t port_Number, uint8_t device_Address, uint8_t string_Type)
{
	switch (string_Type)
	{
	case USB_Host_Device_Manager___STRING_TYPE_LANGUAGE_ID:
	{
		return (USB_Host_Device_Manager___GET_MIN(USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.language_ID_Descriptor_Buffer[0], USB_Host_Config___DEVICE_LANGUAGE_ID_STRING_MAX_LENGTH));
	}
	case USB_Host_Device_Manager___STRING_TYPE_MANUFACTURER:
	{
		return (USB_Host_Device_Manager___GET_MIN(USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.manufacturer_String_Descriptor_Buffer[0], USB_Host_Config___DEVICE_MANUFACTURER_STRING_MAX_LENGTH));
	}
	case USB_Host_Device_Manager___STRING_TYPE_PRODUCT:
	{
		return (USB_Host_Device_Manager___GET_MIN(USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.product_String_Descriptor_Buffer[0], USB_Host_Config___DEVICE_PRODUCT_STRING_MAX_LENGTH));
	}
	case USB_Host_Device_Manager___STRING_TYPE_SERIAL_NUMBER:
	{
		return (USB_Host_Device_Manager___GET_MIN(USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.serial_Number_String_Descriptor_Buffer[0], USB_Host_Config___DEVICE_SERIAL_NUMBER_STRING_MAX_LENGTH));
	}
	}
}

uint16_t USB_Host_Device_Manager___Device_Get_Configuration_Descriptor_Total_Length(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.configuration[configuration_Index].p_Configuration_Descriptor->wTotalLength);
}

void USB_Host_Device_Manager___Device_Update_Configuration_Descriptor(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Index)
{
	uint16_t total_Length 				= USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.configuration[configuration_Index].p_Configuration_Descriptor->wTotalLength;
	uint16_t i 							= 0;
	uint8_t interface_Descriptor_Number	= 0;
	uint8_t endpoint_Descriptor_Number  = 0;
	while(i <  total_Length)
	{
		uint8_t descriptor_Type 	= USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.configuration_Descriptor_Buffer[configuration_Index][i+1];
		uint8_t descriptor_Length 	= USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.configuration_Descriptor_Buffer[configuration_Index][i];
		uint8_t* p_Current_Position = &(USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.configuration_Descriptor_Buffer[configuration_Index][i]);

		switch (descriptor_Type)
		{
		case USB_Host___CONFIGURATION_DESCRIPTOR_TYPE:
			USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.configuration[configuration_Index].p_Configuration_Descriptor = (USB_Host___Configuration_Descriptor_TypeDef*)p_Current_Position;
			USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.configuration[configuration_Index].configuration_Buffer_Index = i;
			break;
		case USB_Host___INTERFACE_DESCRIPTOR_TYPE:
			USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.configuration[configuration_Index].interface[interface_Descriptor_Number].p_Interface_Descriptor 		= (USB_Host___Interface_Descriptor_TypeDef*)p_Current_Position;
			USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.configuration[configuration_Index].interface[interface_Descriptor_Number].configuration_Buffer_Index 	= i;
			interface_Descriptor_Number++;
			break;
		case USB_Host___ENDPOINT_DESCRIPTOR_TYPE:
            USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.configuration[configuration_Index].interface[interface_Descriptor_Number-1].endpoint[endpoint_Descriptor_Number].p_Endpoint_Descriptor 		= (USB_Host___Endpoint_Descriptor_TypeDef*)p_Current_Position;
            USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.configuration[configuration_Index].interface[interface_Descriptor_Number-1].endpoint[endpoint_Descriptor_Number].configuration_Buffer_Index 	= i;
            endpoint_Descriptor_Number++;
            break;
		}

		i += descriptor_Length;
	}
}

uint8_t* USB_Host_Device_Manager___Get_Configuration_Descriptor_Buffer(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Idex)
{
	return(USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.configuration_Descriptor_Buffer[configuration_Idex]);
}

uint8_t* USB_Host_Device_Manager___Get_Language_ID_Descriptor_Buffer(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.language_ID_Descriptor_Buffer);
}

uint8_t* USB_Host_Device_Manager___Device_Get_Manufacturer_String_Descriptor_Buffer(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.manufacturer_String_Descriptor_Buffer);
}

uint8_t* USB_Host_Device_Manager___Device_Get_Product_String_Descriptor_Buffer(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.product_String_Descriptor_Buffer);
}

uint8_t* USB_Host_Device_Manager___Device_Get_Serial_Number_String_Descriptor_Buffer(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.serial_Number_String_Descriptor_Buffer);
}

void USB_Host_Device_Manager___Update_String_Length(uint8_t port_Number, uint8_t device_Address, uint8_t string_Type)
{
	uint8_t string_Length = USB_Host_Device_Manager___Get_String_Descriptor_Length(port_Number, device_Address, string_Type);

	if(string_Length != 0)
	{
		string_Length = (string_Length -2)/2;
	}

	switch(string_Type)
	{
	case USB_Host_Device_Manager___STRING_TYPE_LANGUAGE_ID:
	{
		USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.language_ID_List_Length = string_Length;
		break;
	}
	case USB_Host_Device_Manager___STRING_TYPE_MANUFACTURER:
	{
		USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.manufacturer_String_Length = string_Length;
		break;
	}
	case USB_Host_Device_Manager___STRING_TYPE_PRODUCT:
	{
		USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.product_String_Length = string_Length;
		break;
	}
	case USB_Host_Device_Manager___STRING_TYPE_SERIAL_NUMBER:
	{
		USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.serial_Number_String_Length = string_Length;
		break;
	}
	}
}

uint16_t USB_Host_Device_Manager___Device_Get_Language_ID(uint8_t port_Number, uint8_t device_Address, uint8_t language_Index)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->strings.p_Language_ID_List[language_Index]);
}

USB_Host___Device_Descriptor_TypeDef USB_Host_Device_Manager___Device_Get_Device_Descriptor(uint8_t port_Number, uint8_t device_Address)
{
	return (*(USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.p_Device_Descriptor));
}

void USB_Host_Device_Manager___Device_Set_In_Endpoint_Max_Packet_Size(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint16_t max_Packet_Size)
{
	USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->in_Endpoint_Status[endpoint_Number].max_Packet_Size = max_Packet_Size;
}

void USB_Host_Device_Manager___Device_Set_Out_Endpoint_Max_Packet_Size(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint16_t max_Packet_Size)
{
	USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->out_Endpoint_Status[endpoint_Number].max_Packet_Size = max_Packet_Size;
}

uint32_t USB_Host_Device_Manager___Device_Get_Out_Endpoint_Max_Packet_Size(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->out_Endpoint_Status[endpoint_Number].max_Packet_Size);
}

uint32_t USB_Host_Device_Manager___Device_Get_In_Endpoint_Max_Packet_Size(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->in_Endpoint_Status[endpoint_Number].max_Packet_Size);
}

uint8_t USB_Host_Device_Manager___Device_Get_Endpoint_Current_Packet_ID(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Direction, uint8_t endpoint_Number)
{
	if (endpoint_Direction == USB_Host___TRANSFER_DIRECTION_OUT)
	{
		return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->out_Endpoint_Status[endpoint_Number].current_Packet_ID);
	}
	else
	{
		return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->in_Endpoint_Status[endpoint_Number].current_Packet_ID);
	}
}

void USB_Host_Device_Manager___Device_Set_Endpoint_Current_Packet_ID(uint8_t port_Number, uint8_t device_Address, uint8_t endpoint_Number, uint8_t endpoint_Direction, uint8_t Packet_ID)
{
	if (endpoint_Direction == USB_Host___TRANSFER_DIRECTION_IN)
	{
		USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->out_Endpoint_Status[endpoint_Number].current_Packet_ID = Packet_ID;
	}
	else
	{
		USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->in_Endpoint_Status[endpoint_Number].current_Packet_ID = Packet_ID;
	}
}

uint8_t USB_Host_Device_Manager___Device_Is_Low_Speed_Device(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->status.is_Low_Speed_Device);
}

void USB_Host_Device_Manager___Port_Remove_Device(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];
	USB_Host_Device_Manager___Free_Device(p_Device);
	USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address] = NULL;
}

void USB_Host_Device_Manager___Device_Set_Is_Root_Device(uint8_t port_Number, uint8_t device_Address, uint8_t is_Root_Device)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];
	if(p_Device != NULL)
	{
		p_Device -> status.is_Root_Device = is_Root_Device;
	}
}

int8_t USB_Host_Device_Manager___Port_Set_Device_To_Address(uint8_t port_Number, uint8_t device_Address, USB_Host_Device_Manager___Device_TypeDef* p_Device)
{
	if(USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address] == NULL)
	{
		USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address] = p_Device;
		return(EXIT_SUCCESS);
	}

	return(EXIT_FAILURE);
}

uint8_t USB_Host_Device_Manager___Port_Get_Free_Device_Address(uint8_t port_Number)
{
	for (uint8_t i = 1; i < USB_Host_Device_Manager___PORT_DEVICE_LIMIT+1; i++)
	{
		if (USB_Host_Device_Manager___Port[port_Number].p_Device[i] == NULL)
		{
			return (i);
		}
	}
	return (0);
}

void USB_Host_Device_Manager___Port_Copy_Device_To_Address(uint8_t port_Number, uint8_t device_Address, uint8_t new_Device_Address)
{
	USB_Host_Device_Manager___Port[port_Number].p_Device[new_Device_Address] = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];
}

uint8_t USB_Host_Device_Manager___Reserve_New_Device_Address(uint8_t port_Number, uint8_t current_Device_Address)
{
	uint8_t new_Address = USB_Host_Device_Manager___Port_Get_Free_Device_Address(port_Number);
	if (new_Address != 0)
	{
		USB_Host_Device_Manager___Port_Copy_Device_To_Address(port_Number, current_Device_Address, new_Address);
	}
	return (new_Address);
}

void USB_Host_Device_Manager___Device_Change_Current_USB_Address(uint8_t port_Number, uint8_t current_Device_Address, uint8_t new_Device_Address)
{
	if(current_Device_Address == USB_Host_Device_Manager___Port[port_Number].port_Status.root_Device_Address)
	{
		USB_Host_Device_Manager___Port[port_Number].port_Status.root_Device_Address = new_Device_Address;
	}
	USB_Host_Device_Manager___Port[port_Number].p_Device[new_Device_Address] = USB_Host_Device_Manager___Port[port_Number].p_Device[current_Device_Address];
	USB_Host_Device_Manager___Port[port_Number].p_Device[current_Device_Address] = NULL;
	USB_Host_Device_Manager___Port[port_Number].p_Device[new_Device_Address]->status.current_USB_Address = new_Device_Address;
}

void USB_Host_Device_Manager___Port_Clear_Device_Connected_Or_Disconnected_Flag(uint8_t port_Number)
{
	USB_Host_Device_Manager___Port[port_Number].port_Status.device_Connected_Or_Disconnected_Flag = false;
}

void USB_Host_Device_Manager___Port_Set_Device_Connected_Or_Disconnected_Flag(uint8_t port_Number)
{
	USB_Host_Device_Manager___Port[port_Number].port_Status.device_Connected_Or_Disconnected_Flag = true;
}


uint8_t USB_Host_Device_Manager__Port_Is_Device_Connected_Or_Disconnected_Flag(uint8_t port_Number)
{
	return(USB_Host_Device_Manager___Port[port_Number].port_Status.device_Connected_Or_Disconnected_Flag);
}

void USB_Host_Device_Manager___Device_Disconnected(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	if(p_Device != NULL)
	{
		USB_Host_Device_Manager___Port[port_Number].port_Status.device_Connected_Or_Disconnected_Flag 	= true;
		p_Device->status.connection_Flag 																= true;
		p_Device->status.is_Connected 																	= false;
	}
}

void USB_Host_Device_Manager___Device_Connected(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	if(p_Device != NULL)
	{
		USB_Host_Device_Manager___Port[port_Number].port_Status.device_Connected_Or_Disconnected_Flag 	= true;
		p_Device->status.connection_Flag 																= true;
		p_Device->status.is_Connected 																	= true;
	}
}

uint8_t USB_Host_Device_Manager___Is_Device_Connected(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	if(p_Device != NULL)
	{
		return(p_Device -> status.is_Connected);
	}
	return(0);
}

uint8_t USB_Host_Device_Manager___Device_Connection_Flag(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	if(p_Device != NULL)
	{
		return(p_Device -> status.connection_Flag);
	}

	return(0);
}

void USB_Host_Device_Manager___Clear_Device_Connection_Flag(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	if(p_Device != NULL)
	{
		p_Device -> status.connection_Flag = false;
	}
}

void USB_Host_Device_Manager___Device_Set_Port_Number(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	if(p_Device != NULL)
	{
		p_Device -> status.port_Number = port_Number;
	}
}

void USB_Host_Device_Manager___Device_Set_Speed(uint8_t port_Number, uint8_t device_Address, uint8_t device_Speed)
{
	USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	if(p_Device != NULL)
	{
		if(device_Speed == USB_Host_Device_Manager___LOW_SPEED_DEVICE)
		{
			p_Device -> status.is_Low_Speed_Device = 1;
			p_Device -> status.is_High_Speed_Device = 0;
		}
		else if(device_Speed == USB_Host_Device_Manager___HIGH_SPEED_DEVICE)
		{
			p_Device -> status.is_Low_Speed_Device = 0;
			p_Device -> status.is_High_Speed_Device = 1;
		}
		else
		{
			p_Device -> status.is_Low_Speed_Device = 0;
			p_Device -> status.is_High_Speed_Device = 0;
		}
	}
}

uint8_t USB_Host_Device_Manager___Port_Get_Root_Device_Address(uint8_t port_Number)
{
	return(USB_Host_Device_Manager___Port[port_Number].port_Status.root_Device_Address);
}

void USB_Host_Device_Manager___Device_Initialize_Buffers(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_TypeDef *p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	if (p_Device != NULL)
	{
		p_Device -> descriptors.p_Device_Descriptor = (USB_Host___Device_Descriptor_TypeDef*)	(p_Device -> descriptor_Buffers.device_Descriptor_Buffer);
		p_Device -> strings.p_Language_ID_List 		= (uint16_t*)								&(p_Device -> descriptor_Buffers.language_ID_Descriptor_Buffer[2]);
		p_Device -> strings.p_Manufacturer_String 	= (uint16_t*)								&(p_Device -> descriptor_Buffers.manufacturer_String_Descriptor_Buffer[2]);
		p_Device -> strings.p_Product_String 		= (uint16_t*)								&(p_Device -> descriptor_Buffers.product_String_Descriptor_Buffer[2]);
		p_Device -> strings.p_Serial_Number_String 	= (uint16_t*)								&(p_Device -> descriptor_Buffers.serial_Number_String_Descriptor_Buffer[2]);
		for(uint8_t i = 0; i < USB_Host_Config___DEVICE_MAX_NUMBER_OF_CONFIGURATIONS; i++)
		{
			p_Device -> descriptors.configuration[i].p_Configuration_Descriptor = (USB_Host___Configuration_Descriptor_TypeDef*) (p_Device -> descriptor_Buffers.configuration_Descriptor_Buffer[i]);
		}
	}
}

void USB_Host_Device_Manager___Handle_Start_Of_Frame(uint8_t port_Number)
{
	if(USB_Host_Device_Manager___Polling_List->first_Node != NULL)
	{
		USB_Host_Device_Manager___Polling_Device_Node_TypeDef* p_Polling_Node = USB_Host_Device_Manager___Polling_List->first_Node;
		while (p_Polling_Node != NULL)
		{
			p_Polling_Node->polling_Counter++;
			if (p_Polling_Node->polling_Counter >= (p_Polling_Node -> polling_Target -1))
			{
				p_Polling_Node->polling_Counter = 0;
				p_Polling_Node->callback(port_Number, p_Polling_Node -> device_Address);
			}
			p_Polling_Node = p_Polling_Node->next_Node;
		}
	}
}

void USB_Host_Device_Manager___Initialize_Device(uint8_t port_Number, uint8_t device_Address, uint8_t is_Root_Device, uint8_t device_Speed)
{
	USB_Host_Device_Manager___Device_Set_Is_Root_Device							(port_Number, device_Address, is_Root_Device);
	USB_Host_Device_Manager___Device_Initialize_Buffers							(port_Number, device_Address);
	USB_Host_Device_Manager___Device_Set_Port_Number							(port_Number, 0);
	USB_Host_Device_Manager___Device_Set_Speed									(port_Number, 0, device_Speed);
	USB_Host_Device_Manager___Device_Connected									(port_Number, 0);
}

int8_t USB_Host_Device_Manager___Allocate_Device_At_Address_Zero(uint8_t port_Number, uint8_t device_Speed, uint8_t is_Root_Device)
{
	if(USB_Host_Device_Manager___Port[port_Number].p_Device[0] == NULL)
	{
		USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Allocate_Device();
		if(USB_Host_Device_Manager___Port_Set_Device_To_Address(port_Number, 0, p_Device) == EXIT_SUCCESS)
		{
			USB_Host_Device_Manager___Initialize_Device(port_Number, 0, is_Root_Device, device_Speed);

			return(EXIT_SUCCESS);
		}
		return(EXIT_FAILURE);
	}
	return(EXIT_FAILURE);
}
