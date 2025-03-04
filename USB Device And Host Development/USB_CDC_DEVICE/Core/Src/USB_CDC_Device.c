/*
 * USB_CDC_Device.c
 *
 *  Created on: Nov 5, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>
#include <stdio.h>

#include "../Inc/USB_CDC_Device.h"
#include <USB_Device.h>

USB_CDC_Device___CDC_Device_TypeDef USB_CDC_Device___CDC_Device[USB_Device___NUM_PORTS];

static uint16_t language_String_Descriptor[] 				= 	{0x0409};
static uint16_t manufacturer_String_Descriptor[] 			= 	{'V', 'I', 'K', 'I' , 'N', 'G', ' ', 'L', 'A', 'B', 'S'};
static uint16_t product_String_Descriptor[] 				= 	{'S', 'K' ,'I', 'P', 'P' , 'E', 'R'};
static uint16_t serial_Number_String_Descriptor[] 			= 	{'0', '1', '1', '0'};

static uint8_t USB_CDC_Device___Device_Descriptor[] =
{
	0x12,							// bLength
	0x01,							// bDescriptorType
	0x00, 0x02,						// bcdUSB
	0x02,							// bDeviceClass
	0x02,							// bDeviceSubClass
	0x00,							// bDeviceProtocol
	0x40,							// bMaxPacketSize0
	0x83, 0x04,						// idVendor
	0x10, 0x00,						// idProduct
	0x00, 0x01,						// bcdDevice
	0x01,							// iManufacturer
	0x02,							// iProduct
	0x03,							// iSerialNumber
	0x01							// bNumConfigurations
};

static uint8_t USB_CDC_Device___Configuration_Descriptor[] =
{
	0x09,							// configurations.bLength
	0x02,							// configurations.bDescriptorType
	0x43, 0x00,						// configurations.wTotalLength
	0x02,							// configurations.bNumInterfaces
	0x01,							// configurations.bConfigurationValue
	0x00,							// configurations.iConfiguration
	0xc0,							// configurations.bmAttributes
	0x32,							// configurations.bMaxPower

	0x09,							// Interface.bLength
	0x04,							// Interface.bDescriptorType
	0x00,							// Interface.bInterfaceNumber
	0x00,							// Interface.bAlternateSetting
	0x01,							// Interface.bNumEndpoints
	0x02,							// Interface.bInterfaceClass
	0x02,							// Interface.bInterfaceSubClass
	0x01,							// Interface.bInterfaceProtocol
	0x00,							// Interface.iInterface

	0x05,							// CS.bLength
	0x24,							// CS.bDescriptorType
	0x00,							// CS.bDescriptorSubtype
	0x10, 0x01,						// CS.CDC

	0x05,							// CS.bLength
	0x24,							// CS.bDescriptorType
	0x01,							// CS.bDescriptorSubtype
	0x00,							// CS.bmCapabilities
	0x01,							// CS.data interface

	0x04,							// CS.bLength
	0x24,							// CS.bDescriptorType
	0x02,							// CS.bDescriptorSubtype
	0x02,							// CS.bmCapabilities

	0x05,							// CS.bLength
	0x24,							// CS.bDescriptorType
	0x06,							// CS.bDescriptorSubtype
	0x00,							// CS.Control Interface
	0x01,							// CS.subordinate interface

	0x07,							// Endpoint.bLength
	0x05,							// Endpoint.bDescriptorType
	0x82,							// Endpoint.bEndpointAddress
	0x03,							// Endpoint.bmAttributes
	0x08, 0x00,						// Endpoint.wMaxPacketSize
	0x10,							// Endpoint.bInterval

	0x09,							// Interface.bLength
	0x04,							// Interface.bDescriptorType
	0x01,							// Interface.bInterfaceNumber
	0x00,							// Interface.bAlternateSetting
	0x02,							// Interface.bNumEndpoints
	0x0a,							// Interface.bInterfaceClass
	0x00,							// Interface.bInterfaceSubClass
	0x00,							// Interface.bInterfaceProtocol
	0x00,							// Interface.iInterface

	0x07,							// Endpoint.bLength
	0x05,							// Endpoint.bDescriptorType
	0x01,							// Endpoint.bEndpointAddress
	0x02,							// Endpoint.bmAttributes
	0x40, 0x00,						// Endpoint.wMaxPacketSize
	0x00,							// Endpoint.bInterval

	0x07,							// Endpoint.bLength
	0x05,							// Endpoint.bDescriptorType
	0x81,							// Endpoint.bEndpointAddress
	0x02,							// Endpoint.bmAttributes
	0x40, 0x00,						// Endpoint.wMaxPacketSize
	0x00							// Endpoint.bInterval
};

uint8_t USB_CDC_Device___Set_Configuration_Solution_Callback		(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS);
uint8_t USB_CDC_Device___Get_Line_Coding_Solution_Callback			(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS);
uint8_t USB_CDC_Device___Set_Control_Line_State_Solution_Callback	(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS);
uint8_t USB_CDC_Device___Set_Line_Coding_Solution_Callback			(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS);
uint8_t USB_CDC_Device___Clear_Stall_Solution_Callback				(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS);

USB_Device___Control_Solution_TypeDef USB_Device___Control_Solution_List[] =
{
	{ { 0x80, 0x06, 0x0100, 0x0000, 0x0000 }, 0b11100, USB_CDC_Device___Device_Descriptor, 				sizeof(USB_CDC_Device___Device_Descriptor), 		NULL },														// Get Device Descriptor
	{ { 0x80, 0x06, 0x0200, 0x0000, 0x0000 }, 0b11100, USB_CDC_Device___Configuration_Descriptor, 		sizeof(USB_CDC_Device___Configuration_Descriptor),	NULL }, 													// Get Configuration 1 Descriptor
	{ { 0x80, 0x06, 0x0300, 0x0000, 0x0000 }, 0b11100, (uint8_t*)language_String_Descriptor, 			sizeof(language_String_Descriptor), 				NULL }, 													// Get language string Descriptor
	{ { 0x80, 0x06, 0x0301, 0x0000, 0x0000 }, 0b11100, (uint8_t*)manufacturer_String_Descriptor, 		sizeof(manufacturer_String_Descriptor), 			NULL }, 													// Get manufacturer string Descriptor
	{ { 0x80, 0x06, 0x0302, 0x0000, 0x0000 }, 0b11100, (uint8_t*)product_String_Descriptor, 			sizeof(product_String_Descriptor), 					NULL }, 													// Get product string Descriptor
	{ { 0x80, 0x06, 0x0303, 0x0000, 0x0000 }, 0b11100, (uint8_t*)serial_Number_String_Descriptor, 		sizeof(serial_Number_String_Descriptor), 			NULL }, 													// Get serial number string Descriptor
	{ { 0x00, 0x05, 0x0000, 0x0000, 0x0000 }, 0b11000, NULL,		 									0, 													USB_Device___Set_Address_Solution_Callback}, 				// Get Configuration 1 Descriptor
	{ { 0x00, 0x09, 0x0000, 0x0000, 0x0000 }, 0b11000, NULL,		 									0, 													USB_CDC_Device___Set_Configuration_Solution_Callback}, 		// set Configuration
	{ { 0xa1, 0x21, 0x0000, 0x0000, 0x0000 }, 0b11000, NULL, 											0, 													USB_CDC_Device___Get_Line_Coding_Solution_Callback}, 		// get Line Coding
	{ { 0x21, 0x22, 0x0000, 0x0000, 0x0000 }, 0b11000, NULL, 											0, 													USB_CDC_Device___Set_Control_Line_State_Solution_Callback}, // set control line state
	{ { 0x21, 0x20, 0x0000, 0x0000, 0x0000 }, 0b11000, NULL, 											0, 													USB_CDC_Device___Set_Line_Coding_Solution_Callback}, 		// set line coding
	{ { 0x02, 0x01, 0x0000, 0x0000, 0x0000 }, 0b11000, NULL, 											0, 													USB_CDC_Device___Clear_Stall_Solution_Callback} 		// set line coding
};

uint8_t USB_CDC_Device___Clear_Stall_Solution_Callback(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS)
{
	USB_Device___Reset_Endpoint(port_Number, (setup_Packet.wIndex & 0x0f), (setup_Packet.wIndex & 0x80) >> 7);
	return (1);
}

uint8_t USB_CDC_Device___Set_Line_Coding_Solution_Callback(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS)
{
	for (uint8_t i = 0; i < 7; i++)
	{
		USB_CDC_Device___CDC_Device[port_Number].line_Coding[i] = data[i];
	}

	return(1);
}

uint8_t USB_CDC_Device___Set_Control_Line_State_Solution_Callback(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS)
{
	USB_CDC_Device___CDC_Device[port_Number].control_Line_State = setup_Packet.wValue;
	return(1);
}

uint8_t USB_CDC_Device___Get_Line_Coding_Solution_Callback(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS)
{
	solution->buffer 		= USB_CDC_Device___CDC_Device[port_Number].line_Coding;
	solution->buffer_Size 	= sizeof(USB_CDC_Device___CDC_Device[port_Number].line_Coding);
	return(1);
}

uint8_t USB_CDC_Device___Is_Enabled(uint8_t port_Number)
{
	return (USB_CDC_Device___CDC_Device[port_Number].is_Enabled);
}

uint8_t USB_CDC_Device___Is_DTE(uint8_t port_Number)
{
	return (USB_CDC_Device___CDC_Device[port_Number].control_Line_State & 0x01);
}

uint8_t USB_CDC_Device___Add_Data_To_TX_Buffer(uint8_t port_Number, char* data, uint16_t length)
{
	uint8_t* TX_Message_Buffer_Start 		= USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer;
	uint8_t* TX_Message_Buffer_Write_Head 	= USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Write_Head;
	uint8_t* TX_Message_Buffer_End 			= TX_Message_Buffer_Start + USB_CDC_Device___TX_MESSAGE_BUFFER_SIZE;

	if (TX_Message_Buffer_Write_Head + length > TX_Message_Buffer_End)
	{
		return (EXIT_FAILURE);
	}

	for (uint16_t i = 0; i < length; i++)
	{
		*TX_Message_Buffer_Write_Head = data[i];
		TX_Message_Buffer_Write_Head++;
	}

	 USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Write_Head = TX_Message_Buffer_Write_Head;

	return(EXIT_SUCCESS);
}



uint8_t USB_CDC_Device___Send_Data(uint8_t port_Number, char *data, uint16_t length)
{
	if(USB_CDC_Device___Is_Enabled(port_Number) && USB_CDC_Device___Is_DTE(port_Number))
	{
		uint8_t* TX_Message_Buffer_Read_Head = USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Read_Head;
		uint8_t* TX_Message_Buffer_Write_Head = USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Write_Head;

		uint8_t is_Buffer_Full 	= 	USB_CDC_Device___Add_Data_To_TX_Buffer(port_Number, data, length);
		uint8_t read_Success 	=	USB_Device___Transfer_In(port_Number, 1, USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Read_Head, TX_Message_Buffer_Write_Head - TX_Message_Buffer_Read_Head);

		return(!is_Buffer_Full);
	}
	return(0);
}

void USB_CDC_Device___EP1_TX_Callback(USB_Device___TX_CALLBACK_PARAMETERS)
{
	uint8_t *TX_Message_Buffer_Read_Head = USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Read_Head;
	uint8_t *TX_Message_Buffer_Write_Head = USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Write_Head;

	if (TX_Message_Buffer_Read_Head != TX_Message_Buffer_Write_Head)
	{
		USB_Device___Transfer_In(port_Number, 1, TX_Message_Buffer_Read_Head, TX_Message_Buffer_Write_Head - TX_Message_Buffer_Read_Head); USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Read_Head = TX_Message_Buffer_Write_Head;
	}
	else
	{
		USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Read_Head = USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Write_Head = USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer;
	}
}


void USB_CDC_Device___Set_Interrupt_Char(uint8_t port_Number, char interrupt_Char)
{
	USB_CDC_Device___CDC_Device[port_Number].interrupt_Char[interrupt_Char / 32] |= (1 << (interrupt_Char % 32));
}

void USB_CDC_Device___Set_Message_Received_Callback(uint8_t port_Number, void callback(USB_CDC_Device___MESSAGE_RECEIVED_CALLBACK_PARAMETERS))
{
	USB_CDC_Device___CDC_Device[port_Number].message_Received_Callback = callback;
}

void USB_CDC_Device___EP1_RX_Callback(USB_Device___RX_CALLBACK_PARAMETERS)
{
	uint8_t* 	RX_Message_Buffer 			= USB_CDC_Device___CDC_Device[port_Number].RX_Message_Buffer;
	uint8_t** 	RX_Message_Buffer_Head 		= &USB_CDC_Device___CDC_Device[port_Number].RX_Message_Buffer_Head;

	for(uint16_t i = 0; i < length; i++)
	{
		**RX_Message_Buffer_Head = data[i];
		*RX_Message_Buffer_Head+=1;

		if (*RX_Message_Buffer_Head >= RX_Message_Buffer + USB_CDC_Device___RX_MESSAGE_BUFFER_SIZE)
		{
			USB_CDC_Device___CDC_Device[port_Number].RX_Message_Buffer_Overflow_Flag = 1;
			*RX_Message_Buffer_Head = RX_Message_Buffer;
		}

		if((1 << (data[i] % 32)) & USB_CDC_Device___CDC_Device[port_Number].interrupt_Char[data[i] / 32])
		{
			if (USB_CDC_Device___CDC_Device[port_Number].message_Received_Callback != NULL)
			{
				USB_CDC_Device___CDC_Device[port_Number].message_Received_Callback(port_Number, USB_CDC_Device___CDC_Device[port_Number].RX_Message_Buffer_Overflow_Flag, data[i], RX_Message_Buffer, *RX_Message_Buffer_Head - RX_Message_Buffer);
			}
			*RX_Message_Buffer_Head = RX_Message_Buffer;
		}
	}

	USB_Device___Open_RX_Endpoint(port_Number, 1, 0, USB_CDC_Device___CDC_Device[port_Number].RX_Packet_Buffer, USB_CDC_Device___RX_PACKET_BUFFER_SIZE);
}

uint8_t USB_CDC_Device___Set_Configuration_Solution_Callback(USB_Device___CONTROL_SOLUTION_CALLBACK_PARAMETERS)
{
	if (setup_Packet.wValue == 1)
	{
		USB_CDC_Device___CDC_Device[port_Number].current_Configuration = 1;
		USB_Device___Initialize_Endpoint(port_Number, 2, USB_Device___ENDPOINT_DERECTION_IN, 	USB_Device___ENDPOINT_TYPE_INTERRUPT, 	0x08);
		USB_Device___Initialize_Endpoint(port_Number, 1, USB_Device___ENDPOINT_DERECTION_IN, 	USB_Device___ENDPOINT_TYPE_BULK, 		0x40);
		USB_Device___Initialize_Endpoint(port_Number, 1, USB_Device___ENDPOINT_DERECTION_OUT, 	USB_Device___ENDPOINT_TYPE_BULK, 		0x40);
		USB_Device___Set_Endpoint_TX_FIFO_Size(port_Number, 1, 0x100);
		USB_Device___Set_Endpoint_TX_FIFO_Size(port_Number, 2, 0x20);
		USB_Device___Set_Endpoint_TX_Callback(port_Number, 1, USB_CDC_Device___EP1_TX_Callback);
        USB_Device___Set_Endpoint_RX_Callback(port_Number, 1, USB_CDC_Device___EP1_RX_Callback);
        USB_Device___Open_RX_Endpoint(port_Number, 1, 0, USB_CDC_Device___CDC_Device[port_Number].RX_Packet_Buffer, USB_CDC_Device___RX_PACKET_BUFFER_SIZE);
		USB_Device___Set_Nak(port_Number, 2, USB_Device___ENDPOINT_DERECTION_IN);
		USB_Device___Set_Nak(port_Number, 1, USB_Device___ENDPOINT_DERECTION_IN);
		USB_CDC_Device___CDC_Device[port_Number].is_Enabled = 1;
	}
	else
	{
		USB_CDC_Device___CDC_Device[port_Number].current_Configuration = 0;
	}

	return(1);
}

void USB_CDC_Device___Init(uint8_t port_Number)
{
	USB_Device___Set_Conrol_Solutions(port_Number, 0, USB_Device___Control_Solution_List, sizeof(USB_Device___Control_Solution_List) / sizeof(USB_Device___Control_Solution_List[0]));
	USB_Device___Init(port_Number);
	USB_CDC_Device___CDC_Device[port_Number].current_Configuration 				= 0;
	USB_CDC_Device___CDC_Device[port_Number].control_Line_State 				= 0;
	USB_CDC_Device___CDC_Device[port_Number].line_Coding[0] 					= 0x00;
	USB_CDC_Device___CDC_Device[port_Number].line_Coding[1] 					= 0x00;
	USB_CDC_Device___CDC_Device[port_Number].is_Enabled 						= 0;
	USB_CDC_Device___CDC_Device[port_Number].RX_Message_Buffer_Head 			= USB_CDC_Device___CDC_Device[port_Number].RX_Message_Buffer;
	USB_CDC_Device___CDC_Device[port_Number].RX_Message_Buffer_Overflow_Flag 	= 0;
	USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Write_Head 		= USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer;
	USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer_Read_Head 		= USB_CDC_Device___CDC_Device[port_Number].TX_Message_Buffer;
	for (uint8_t i = 0; i < 8; i++)
	{
		USB_CDC_Device___CDC_Device[port_Number].interrupt_Char[i] = 0;
	}
}
