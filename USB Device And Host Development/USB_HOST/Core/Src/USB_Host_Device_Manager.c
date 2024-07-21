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

uint8_t* USB_Host_Device_Manager___Get_Device_Descriptor_Buffer(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptor_Buffers.device_Descriptor_Buffer);
}

USB_Host___Device_Descriptor_TypeDef USB_Host_Device_Manager___Device_Get_Device_Descriptor(uint8_t port_Number, uint8_t device_Address)
{
	return (*(USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->descriptors.p_Device_Descriptor));
}

void USB_Host_Device_Manager___Device_Set_Setup_Stage(uint8_t port_Number, uint8_t device_Address, uint8_t setup_Stage)
{
	USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->status.setup_Stage = setup_Stage;
}

uint8_t USB_Host_Device_Manager___Device_Get_Setup_Stage(uint8_t port_Number, uint8_t device_Address)
{
	return (USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->status.setup_Stage);
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

void USB_Host_Device_Manager___Device_Update_Current_USB_Address(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address]->status.current_USB_Address = device_Address;
}

void USB_Host_Device_Manager___Port_remove_Device_From_Address(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address] = NULL;
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
	for(uint8_t i = 0; i < USB_Host_Device_Manager___PORT_DEVICE_LIMIT; i++)
	{
		USB_Host_Device_Manager___Device_TypeDef* p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[i];
		if(p_Device != NULL)
		{
			if(p_Device -> status.is_Root_Device)
			{
				return(i);
			}
		}
	}
	return(0);
}

void USB_Host_Device_Manager___Device_Initialize_Buffers(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Device_Manager___Device_TypeDef *p_Device = USB_Host_Device_Manager___Port[port_Number].p_Device[device_Address];

	if (p_Device != NULL)
	{
		p_Device -> descriptors.p_Device_Descriptor = (USB_Host___Device_Descriptor_TypeDef*)(p_Device -> descriptor_Buffers.device_Descriptor_Buffer);
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
