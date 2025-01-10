/*
 * USB_Host.cpp
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>
#include <stdio.h>

#include <USB_LL.h>
#include <USB_LL_Host.h>
#include "../Inc/USB_Host_Hub.h"
#include "../Inc/USB_Host_Device_Manager.h"
#include "../Inc/USB_Host_Transfers.h"
#include "../Inc/USB_Host_Pipes.h"
#include "../Inc/USB_Host.h"
#include "../Inc/USB_Host_Enumerate.h"

USB_Host___Host_Typedef USB_Host___Host[USB_Host___NUMBER_OF_PORTS];

void USB_Host___Init(uint8_t port_Number)
{
	USB_LL___GPIO_Init(port_Number);
	USB_LL___Init(port_Number, USB_LL___HOST_MODE);
	USB_LL_Host___Set_FIFO_Size(port_Number, 0x80, 0x80, 0x40);
}

uint8_t USB_Host___Convert_USB_LL_Interrupts_Host_Speed_To_USB_Host_Device_Manager_Speed(uint8_t USB_Interrupts_Host_Speed)
{
	if(USB_Interrupts_Host_Speed == USB_LL_Host___LOW_SPEED_VALUE)
	{
		return(USB_Host_Device_Manager___LOW_SPEED_DEVICE);
	}
	else if(USB_Interrupts_Host_Speed == USB_LL_Host___HIGH_SPEED_VALUE)
	{
		return(USB_Host_Device_Manager___HIGH_SPEED_DEVICE);
	}
	else
	{
		return(USB_Host_Device_Manager___FULL_SPEED_DEVICE);
	}
}

void USB_Host___16_Bit_String_To_Char_String(uint16_t* string_16, uint16_t string_Length, char* string_Char)
{
	for(uint16_t i = 0; i < string_Length; i++)
	{
		string_Char[i] = (char)string_16[i];
	}
}

void USB_Debug___Print_Device_Name(uint8_t port_Number, uint8_t device_Address)
{
	uint16_t product_String_Length = USB_Host_Device_Manager___Get_Product_String_Length(port_Number, device_Address);
	uint16_t manufacturer_String_Length = USB_Host_Device_Manager___Get_Manufacturer_String_Length(port_Number, device_Address);
	char product_String[product_String_Length + 1];
	char manufacturer_String[manufacturer_String_Length + 1];
	product_String[product_String_Length] = 0;
	manufacturer_String[manufacturer_String_Length] = 0;
	USB_Host___16_Bit_String_To_Char_String(USB_Host_Device_Manager___Get_Product_String(port_Number, device_Address), product_String_Length, product_String);
	USB_Host___16_Bit_String_To_Char_String(USB_Host_Device_Manager___Get_Manufacturer_String(port_Number, device_Address), manufacturer_String_Length, manufacturer_String);
	printf("%s %s", manufacturer_String, product_String);
}

void USB_Host___Composite_Set_Configuration_Callback(uint8_t port_Number, uint8_t device_Address, uint8_t success)
{
	for(uint8_t i = 0; i < USB_Host_Config___MAX_DEVICE_CONNECTED_CALLBACKS; i++)
	{
		if(USB_Host___Host[port_Number].USB_Host___Device_Connected_Callback[i] != NULL)
		{
			USB_Host___Host[port_Number].USB_Host___Device_Connected_Callback[i](port_Number, device_Address);
		}
	}
}

void USB_Host___Device_Enumeration_Finished(uint8_t port_Number, uint8_t device_Address, uint8_t success)
{
	if(success)
	{
		uint8_t device_Class = USB_Host_Device_Manager___Get_Device_Class(port_Number, device_Address);

		printf("Device Connected: ");
		USB_Debug___Print_Device_Name(port_Number, device_Address);
		printf(": %d\n", device_Address);

		if(device_Class == USB_Host___HUB_DEVICE_CLASS)
		{
			USB_Host_Hub___Initiate_Hub(port_Number, device_Address);
		}
		else if(device_Class == USB_Host___COMPOSITE_DEVICE_CLASS)
		{
			USB_Host___Set_Configuration(port_Number, device_Address, 1, USB_Host___Composite_Set_Configuration_Callback);
		}
		else
		{
			for(uint8_t i = 0; i < USB_Host_Config___MAX_DEVICE_CONNECTED_CALLBACKS; i++)
			{
				if(USB_Host___Host[port_Number].USB_Host___Device_Connected_Callback[i] != NULL)
				{
					USB_Host___Host[port_Number].USB_Host___Device_Connected_Callback[i](port_Number, device_Address);
				}
			}
		}
	}
	else
	{
		USB_Host_Device_Manager___Device_Disconnected(port_Number, device_Address);
	}
}

uint8_t USB_Host___Get_Number_Of_Devices_Connected(uint8_t port_Number)
{
	return(USB_Host_Device_Manager___Get_Number_Of_Devices_Connected(port_Number));
}

void USB_Host___Process_Host_Interrupts(uint8_t port_Number)
{
	if(USB_LL_Host___Is_Root_Device_Connection_Status_Change(port_Number))
	{
		if(USB_LL_Host___Is_Root_Device_Connected(port_Number))
		{
			Skipper_Clock___Delay_ms(50);

			uint8_t USB_LL_Interrupts_Host_Speed = USB_LL_Host___Get_Root_Device_Speed(port_Number);

			uint8_t device_Manager_Speed = USB_Host___Convert_USB_LL_Interrupts_Host_Speed_To_USB_Host_Device_Manager_Speed(USB_LL_Interrupts_Host_Speed);

			if(USB_Host_Device_Manager___Allocate_Device_At_Address_Zero(port_Number, device_Manager_Speed, true) == EXIT_SUCCESS)
			{
				USB_Host_Device_Manager___Enable_Device(port_Number, 0);
				USB_LL_Host___Clear_Connection_Status_Change(port_Number);
			}
		}
		else
		{
			uint8_t root_Device_Address = USB_Host_Device_Manager___Port_Get_Root_Device_Address(port_Number);
			USB_Host_Device_Manager___Device_Disconnected(port_Number, root_Device_Address);
			USB_LL_Host___Clear_Connection_Status_Change(port_Number);
		}
	}
	if(USB_LL_Host___Is_Start_Of_Frame(port_Number))
	{
		USB_Host_Device_Manager___Handle_Start_Of_Frame(port_Number);
		USB_LL_Host___Clear_Start_Of_Frame(port_Number);
	}
}

void USB_Host___Process_Device_Connect(uint8_t port_Number, uint8_t device_Address)
{
	USB_Host_Enumerate___Enumerate_Device(port_Number, device_Address);
}

void USB_Host___Process_Device_Disconnect(uint8_t port_Number, uint8_t device_Address)
{
	printf("Device disconnected: ");
	USB_Debug___Print_Device_Name(port_Number, device_Address);
	printf(": %d\n", device_Address);
}

uint16_t USB_Host___Get_Frame_Number(uint8_t port_Number)
{
	return(USB_LL_Host___Host_Get_Frame_Number(port_Number));
}

uint8_t USB_Host___Add_Device_Disconnected_Callback(uint8_t port_Number, void callback(uint8_t, uint8_t))
{
	for(uint8_t i = 0; i < USB_Host_Config___MAX_DEVICE_CONNECTED_CALLBACKS; i++)
	{
		if(USB_Host___Host[port_Number].USB_Host___Device_Disconnected_Callback[i] == NULL)
		{
			USB_Host___Host[port_Number].USB_Host___Device_Disconnected_Callback[i] = callback;
			return(EXIT_SUCCESS);
		}
	}

	return(EXIT_FAILURE);
}

uint8_t USB_Host___Remove_Device_Disconnected_Callback(uint8_t port_Number, void callback(uint8_t, uint8_t))
{
	for(uint8_t i = 0; i < USB_Host_Config___MAX_DEVICE_CONNECTED_CALLBACKS; i++)
	{
		if(USB_Host___Host[port_Number].USB_Host___Device_Disconnected_Callback[i] == callback)
		{
			USB_Host___Host[port_Number].USB_Host___Device_Disconnected_Callback[i] = NULL;
			return(EXIT_SUCCESS);
		}
	}

	return(EXIT_FAILURE);
}

void USB_Host___Set_Configuration(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Number, void callback(uint8_t, uint8_t, uint8_t))
{
	USB_Host_Device_Manager___Set_Configuration(port_Number, device_Address, configuration_Number, callback);
}

uint8_t USB_Host___Add_Device_Connected_Callback(uint8_t port_Number, void callback(uint8_t, uint8_t))
{
	uint8_t is_Open_Spot 	= 0;
	uint8_t first_Open_Spot = 0;

	for(uint8_t i = 0; i < USB_Host_Config___MAX_DEVICE_CONNECTED_CALLBACKS; i++)
	{
		if(USB_Host___Host[port_Number].USB_Host___Device_Connected_Callback[i] == NULL)
		{
			is_Open_Spot = 1;
			first_Open_Spot = i;
		}
		else if(USB_Host___Host[port_Number].USB_Host___Device_Connected_Callback[i]== callback)
		{
			return (EXIT_SUCCESS);
		}
	}

	if (is_Open_Spot)
	{
		USB_Host___Host[port_Number].USB_Host___Device_Connected_Callback[first_Open_Spot] = callback;
		return(EXIT_SUCCESS);
	}

	return(EXIT_FAILURE);
}

uint8_t USB_Host___Remove_Device_Connected_Callback(uint8_t port_Number, void callback(uint8_t, uint8_t))
{
	for(uint8_t i = 0; i < USB_Host_Config___MAX_DEVICE_CONNECTED_CALLBACKS; i++)
	{
		if(USB_Host___Host[port_Number].USB_Host___Device_Connected_Callback[i] == callback)
		{
			USB_Host___Host[port_Number].USB_Host___Device_Connected_Callback[i] = NULL;
			return(EXIT_SUCCESS);
		}
	}

	return(EXIT_FAILURE);
}

void USB_Host___Process_Device_Manager_Status(uint8_t port_Number)
{
	if(USB_Host_Device_Manager___Port_Is_Device_Connected_Or_Disconnected_Flag(port_Number))
	{
		USB_Host_Device_Manager___Port_Clear_Device_Connected_Or_Disconnected_Flag(port_Number);
		for(uint8_t i = 0; i < USB_Host_Device_Manager___PORT_DEVICE_LIMIT +1; i++)
		{
			if(USB_Host_Device_Manager___Device_Connection_Flag(port_Number, i))
			{
				if(USB_Host_Device_Manager___Is_Device_Connected(port_Number, i))
				{
					USB_Host___Process_Device_Connect(port_Number, i);
				}
				else
				{
					USB_Host___Process_Device_Disconnect(port_Number, i);
					USB_Host_Device_Manager___Port_Remove_Device(port_Number, i);
				}
				USB_Host_Device_Manager___Clear_Device_Connection_Flag(port_Number, i);
			}
		}
	}
}

void USB_Host___Process(uint8_t port_Number)
{
	USB_Host___Process_Host_Interrupts(port_Number);
	USB_Host___Process_Device_Manager_Status(port_Number);
	USB_Host_Pipes___Process_Pipes(port_Number);
	USB_Host_Transfers___Process_All_URBs(port_Number);
}
