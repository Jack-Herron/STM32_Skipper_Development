/*
 * USB_LL_Device.c
 *
 *  Created on: Jun 26, 2024
 *      Author: Jack Herron
 */

#include "../../USB_LL_CORE/Inc/USB_LL_Device.h"

#include <stdlib.h>						// include c standard library
#include <stdint.h>						// Include C library for fixed-width integer types
#include <stm32f4xx.h>					// include MCU specific definitions
#include <Skipper_Clock.h>
#include <stdio.h>

#include "../../USB_LL_CORE/Inc/USB_LL_Definitions.h"
#include "../../USB_LL_CORE/Inc/USB_LL_Hardware.h"
#include "../../USB_LL_CORE/Inc/USB_LL_Interrupts.h"

void USB_LL_Device___Set_FIFO_Size(uint8_t port_Number, USB_LL_Device___FIFO_Config_TypeDef FIFO_Config)
{
	USB_OTG_GlobalTypeDef* USB = USB_LL_Hardware___Get_USB(port_Number);
	uint32_t Start_Address = 0;
	uint8_t num_Endpoints;

	if(port_Number == USB_LL_Device___PORT_ZERO)
	{
		num_Endpoints = USB_LL_Device___PORT_ZERO_NUM_ENDPOINTS;
	}
	else
	{
		num_Endpoints = USB_LL_Device___PORT_ONE_NUM_ENDPOINTS;
	}

	USB -> GRXFSIZ = (FIFO_Config.RX_FIFO_Depth);
	Start_Address += FIFO_Config.RX_FIFO_Depth;
	USB -> DIEPTXF0_HNPTXFSIZ = ((FIFO_Config.Endpoint_TX_FIFO_Depth[0] << USB_OTG_TX0FD_Pos) | (Start_Address << USB_OTG_TX0FSA_Pos));
	Start_Address += FIFO_Config.Endpoint_TX_FIFO_Depth[0];

	for(uint8_t i = 0; i < num_Endpoints; i++)
	{
		if(FIFO_Config.Endpoint_TX_FIFO_Depth[i+1])
		{
			USB -> DIEPTXF[i] 	= 	((FIFO_Config.Endpoint_TX_FIFO_Depth[i+1] << USB_OTG_DIEPTXF_INEPTXFD_Pos) | (Start_Address << USB_OTG_DIEPTXF_INEPTXSA_Pos));
			Start_Address 		+= 	FIFO_Config.Endpoint_TX_FIFO_Depth[i+1];
		}
		else
		{
			USB -> DIEPTXF[i] = 0;
		}
	}
}

void USB_LL_Device___Receive_Setup_Packet(uint8_t port_Number, uint8_t endpoint_Number, uint32_t packet_Size)
{
	uint8_t temp[packet_Size];
	uint32_t 	USB_offset 				= USB_LL_Hardware___Get_USB_BASE(port_Number);
	USB_LL_Hardware___FIFO_Transfer_Out((uint32_t*)(USB_offset + USB_OTG_FIFO_BASE),temp,packet_Size);
	printf("Setup packet received: ");
	for(uint16_t i = 0; i < packet_Size; i++)
	{
		printf("%2.2x ", temp[i]);
	}
	printf("\n");
}

void USB_LL_Device___Receive_Data_Packet(uint8_t port_Number, uint8_t endpoint_Number, uint32_t packet_Size)
{
	uint8_t temp[packet_Size];
	uint32_t 	USB_offset 				= USB_LL_Hardware___Get_USB_BASE(port_Number);
	USB_LL_Hardware___FIFO_Transfer_Out((uint32_t*)(USB_offset + USB_OTG_FIFO_BASE),temp,packet_Size);
	printf("Data packet received: ");
	for(uint16_t i = 0; i < packet_Size; i++)
	{
		printf("%2.2x ", temp[i]);
	}
	printf("\n");
}
