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
#include "../../USB_LL_CORE/Inc/USB_LL.h"

struct USB_LL_Device___RX_Endpoint USB_LL_Device___RX_Endpoint[USB_LL_Device___PORT_COUNT][USB_LL_Device___ENDPOINT_COUNT];
struct USB_LL_Device___TX_Endpoint USB_LL_Device___TX_Endpoint[USB_LL_Device___PORT_COUNT][USB_LL_Device___ENDPOINT_COUNT];

void USB_LL_Device___Init(uint8_t port_Number)
{
	USB_OTG_DeviceTypeDef *USB_Device = USB_LL___Get_USB_Device(port_Number);

	USB_Device -> DIEPMSK = UBS_LL_Device___IN_ENDPOINT_INTERRUPT_MASK;
	USB_Device -> DOEPMSK = UBS_LL_Device___OUT_ENDPOINT_INTERRUPT_MASK;

}

void USB_LL_Device___Set_FIFO_Size(uint8_t port_Number, USB_LL_Device___FIFO_Config_TypeDef FIFO_Config)
{
	USB_OTG_GlobalTypeDef* USB = USB_LL___Get_USB(port_Number);
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

	for(uint8_t i = 0; i < num_Endpoints-1; i++)
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

void USB_LL_Device___Handle_Packet(uint8_t port_Number, uint8_t endpoint_Number, uint8_t packet_Type, uint32_t packet_Size)
{
	uint8_t* 	RX_Buffer 		= USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Buffer;
	uint32_t 	RX_Buffer_Size 	= USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Buffer_Size;
	uint32_t 	USB_offset 		= USB_LL___Get_USB_BASE(port_Number);

	if (packet_Size > RX_Buffer_Size)
	{
		RX_Buffer = NULL;
		USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Buffer_Fill_Level = 0;
	}
	else
	{
		USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Buffer_Fill_Level = packet_Size;
	}

	USB_LL___FIFO_Transfer_Out((uint32_t*)(USB_offset + USB_OTG_FIFO_BASE), RX_Buffer, packet_Size);

}

void USB_LL_Device___Set_RX_Callback(uint8_t port_Number, uint8_t endpoint, void (*callback)(USB_LL_Device___RX_CALLBACK_PARAMETERS))
{
	USB_LL_Device___RX_Endpoint[port_Number][endpoint].RX_Callback = callback;
}

void USB_LL_Device___Set_TX_Callback(uint8_t port_Number, uint8_t endpoint, void (*callback)( USB_LL_Device___TX_CALLBACK_PARAMETERS))
{
	USB_LL_Device___TX_Endpoint[port_Number][endpoint].TX_Callback = callback;
}

void USB_LL_Device___Packet_Received(uint8_t port_Number)
{
	USB_OTG_GlobalTypeDef* 		USB = USB_LL___Get_USB(port_Number);

	uint32_t 					RX_Status        	= USB -> GRXSTSP;
	uint8_t 					endpoint_Number     = USB_LL___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_EPNUM_Msk, USB_OTG_GRXSTSP_EPNUM_Pos);
	uint8_t 					packet_Status 		= USB_LL___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_PKTSTS_Msk, USB_OTG_GRXSTSP_PKTSTS_Pos);
	uint8_t 					byte_Count			= USB_LL___GET_BIT_SEGMENT(RX_Status, USB_OTG_GRXSTSP_BCNT_Msk, USB_OTG_GRXSTSP_BCNT_Pos);

	if(packet_Status == USB_LL_Device___RX_PACKET_STATUS_SETUP_PACKET_RECIEVED)
	{
		if(byte_Count > 0)
		{
			USB_LL_Device___Handle_Packet(port_Number, endpoint_Number, USB_LL_Device___PACKET_TYPE_SETUP, byte_Count);
		}
	}
	else if(packet_Status == USB_LL_Device___RX_PACKET_STATUS_DATA_PACKET_RECIEVED)
	{
		if(byte_Count > 0)
		{
			USB_LL_Device___Handle_Packet(port_Number, endpoint_Number, USB_LL_Device___PACKET_TYPE_DATA, byte_Count);
		}
	}
}

void USB_LL_Device___Set_Address(uint8_t port_Number, uint16_t address)
{
	USB_OTG_DeviceTypeDef *USB_Device = USB_LL___Get_USB_Device(port_Number);

	USB_Device->DCFG = (USB_Device->DCFG & ~USB_OTG_DCFG_DAD) | (address << USB_OTG_DCFG_DAD_Pos);
}

void USB_LL_Device___USB_Suspend(uint8_t port_Number)
{
	printf("USB Suspended\n");
}

void USB_LL_Device___Host_Enumerated(uint8_t port_Number)
{
	printf("Host attempting to connect\n");
}

void USB_LL_Device___Endpoint_Set_NAK(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction)
{
	if (endpoint_Direction == USB_LL_Device___ENDPOINT_DERECTION_OUT)
	{
		USB_OTG_OUTEndpointTypeDef* USB_Device_Out_Endpoint = USB_LL___Get_USB_Device_OUT(port_Number, endpoint_Number);
		USB_Device_Out_Endpoint->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
		//while(!(USB_Device_Out_Endpoint->DOEPCTL & USB_OTG_DOEPCTL_NAKSTS));
	}
	else
	{
		USB_OTG_INEndpointTypeDef* USB_Device_In_Endpoint = USB_LL___Get_USB_Device_IN(port_Number, endpoint_Number);
		USB_Device_In_Endpoint->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;
		//while(!(USB_Device_In_Endpoint->DIEPCTL & USB_OTG_DIEPCTL_NAKSTS));
	}
}

void USB_LL_Device___Endpoint_Set_Stall(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction)
{
	if (endpoint_Direction == USB_LL_Device___ENDPOINT_DERECTION_OUT)
	{
		USB_OTG_OUTEndpointTypeDef* USB_Device_Out_Endpoint = USB_LL___Get_USB_Device_OUT(port_Number, endpoint_Number);
		USB_Device_Out_Endpoint->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
	}
	else
	{
		USB_OTG_INEndpointTypeDef* USB_Device_In_Endpoint = USB_LL___Get_USB_Device_IN(port_Number, endpoint_Number);
		USB_Device_In_Endpoint->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
	}
}

void USB_LL_Device___Endpoint_Clear_NAK(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction)
{
	if (endpoint_Direction == USB_LL_Device___ENDPOINT_DERECTION_OUT)
	{
		USB_OTG_OUTEndpointTypeDef* USB_Device_Out_Endpoint = USB_LL___Get_USB_Device_OUT(port_Number, endpoint_Number);
		USB_Device_Out_Endpoint->DOEPCTL |= USB_OTG_DOEPCTL_CNAK;
	}
	else
	{
		USB_OTG_INEndpointTypeDef* USB_Device_In_Endpoint = USB_LL___Get_USB_Device_IN(port_Number, endpoint_Number);
		USB_Device_In_Endpoint->DIEPCTL |= USB_OTG_DIEPCTL_CNAK;
	}
}

void USB_LL_Device___Enable_Endpoint(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction)
{
	if (endpoint_Direction == USB_LL_Device___ENDPOINT_DERECTION_OUT)
	{
		USB_OTG_OUTEndpointTypeDef* USB_Device_Out_Endpoint = USB_LL___Get_USB_Device_OUT(port_Number, endpoint_Number);
		USB_Device_Out_Endpoint->DOEPCTL |= USB_OTG_DOEPCTL_EPENA;
	}
	else
	{
		USB_OTG_INEndpointTypeDef* USB_Device_In_Endpoint = USB_LL___Get_USB_Device_IN(port_Number, endpoint_Number);
		USB_Device_In_Endpoint->DIEPCTL |= USB_OTG_DIEPCTL_EPENA;
	}
}

uint32_t* USB_LL_Device___Endpoint_Get_TX_FIFO_Pointer(uint8_t port_Number, uint8_t endpoint_Number)
{
	uint32_t USB_offset = USB_LL___Get_USB_BASE(port_Number);

	return (uint32_t*) (USB_offset + USB_OTG_FIFO_BASE + (USB_OTG_FIFO_SIZE * endpoint_Number));
}

void USB_LL_Device___Setup_Endpoint(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction, uint8_t endpoint_Type, uint16_t max_Packet_Size)
{
	if (endpoint_Number == 0)
	{
		switch(max_Packet_Size)
		{
		case 8:
			max_Packet_Size = 3;
			break;
		case 16:
			max_Packet_Size = 2;
			break;
		case 32:
			max_Packet_Size = 1;
			break;
		case 64:
			max_Packet_Size = 0;
			break;
		default:
			max_Packet_Size = 0;
			break;
		}
	}

	if (endpoint_Direction == USB_LL_Device___ENDPOINT_DERECTION_OUT)
	{
		USB_OTG_DeviceTypeDef *USB_Device = USB_LL___Get_USB_Device(port_Number);
		USB_OTG_OUTEndpointTypeDef *USB_Device_Out = USB_LL___Get_USB_Device_OUT(port_Number, endpoint_Number);

		if (endpoint_Type == USB_LL_Device___ENDPOINT_TYPE_CONTROL)
		{
			USB_Device_Out->DOEPTSIZ = (3 << USB_OTG_DOEPTSIZ_STUPCNT_Pos) | (max_Packet_Size << USB_OTG_DOEPTSIZ_XFRSIZ_Pos);
		}

		USB_Device_Out->DOEPCTL = ((max_Packet_Size << USB_OTG_DOEPCTL_MPSIZ_Pos) | (endpoint_Type << USB_OTG_DOEPCTL_EPTYP_Pos) | USB_OTG_DOEPCTL_USBAEP);
		USB_Device->DAINTMSK |= (1 << endpoint_Number);
	}
	else
	{
		USB_OTG_DeviceTypeDef *USB_Device = USB_LL___Get_USB_Device(port_Number);
		USB_OTG_INEndpointTypeDef *USB_Device_In = USB_LL___Get_USB_Device_IN(port_Number, endpoint_Number);
		USB_Device_In->DIEPCTL = (max_Packet_Size << USB_OTG_DIEPCTL_MPSIZ_Pos) | (endpoint_Type << USB_OTG_DIEPCTL_EPTYP_Pos) | (endpoint_Number << USB_OTG_DIEPCTL_TXFNUM_Pos) | USB_OTG_DIEPCTL_USBAEP;
		USB_Device->DAINTMSK |= (1 << (endpoint_Number + 16));
	}
}

uint16_t USB_LL_Device___Endpoint_Get_Max_Packet_Size(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction)
{
	uint8_t endpoint_Max_Packet_Size_Field;

	if (endpoint_Direction == USB_LL_Device___ENDPOINT_DERECTION_OUT)
	{
		USB_OTG_OUTEndpointTypeDef *USB_Device_Out = USB_LL___Get_USB_Device_OUT(port_Number, endpoint_Number);
		endpoint_Max_Packet_Size_Field = (USB_Device_Out->DOEPCTL & USB_OTG_DOEPCTL_MPSIZ);
	}
	else
	{
		USB_OTG_INEndpointTypeDef *USB_Device_IN = USB_LL___Get_USB_Device_IN(port_Number, endpoint_Number);
		endpoint_Max_Packet_Size_Field = (USB_Device_IN->DIEPCTL & USB_OTG_DIEPCTL_MPSIZ);
	}

	uint16_t size;

	if(endpoint_Number == 0){
		switch((endpoint_Max_Packet_Size_Field & 0x03)){
		case 3:
			size = 8;
			break;
		case 2:
			size = 16;
			break;
		case 1:
			size = 32;
			break;
		case 0:
			size = 64;
			break;
		}
	}else{
		size = endpoint_Max_Packet_Size_Field;
	}
	return size;
}

uint16_t USB_LL_Device___Endpoint_Get_FIFO_Space(uint8_t port_Number, uint8_t endpoint_Number)
{
	USB_OTG_INEndpointTypeDef*	USB_Device_In_Endpoint 	= USB_LL___Get_USB_Device_IN(port_Number, endpoint_Number);

	return(USB_Device_In_Endpoint->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV_Msk);
}

void USB_LL_Device___Endpoint_Enable_TX_FIFO_Empty_Interrupt(uint8_t port_Number, uint8_t endpoint_Number)
{
	USB_OTG_DeviceTypeDef *USB_Device = USB_LL___Get_USB_Device(port_Number);
	USB_Device->DIEPEMPMSK 	|= (1 << endpoint_Number);
}

void USB_LL_Device___Endpoint_Disable_TX_FIFO_Empty_Interrupt(uint8_t port_Number, uint8_t endpoint_Number)
{
	USB_OTG_DeviceTypeDef *USB_Device = USB_LL___Get_USB_Device(port_Number);
	USB_Device->DIEPEMPMSK 	&= ~(1 << endpoint_Number);
}

void USB_LL_Device___Endpoint_Transfer_In(uint8_t port_Number, uint8_t endpoint_Number, uint8_t *data, uint32_t length)
{
	USB_OTG_INEndpointTypeDef*	USB_Device_In_Endpoint 	= USB_LL___Get_USB_Device_IN(port_Number, endpoint_Number);
	uint32_t* 					FIFO_Pointer 			= USB_LL_Device___Endpoint_Get_TX_FIFO_Pointer(port_Number, endpoint_Number);
	uint16_t 					max_Packet_Size 		= USB_LL_Device___Endpoint_Get_Max_Packet_Size(port_Number, endpoint_Number, USB_LL_Device___ENDPOINT_DERECTION_IN);
	uint16_t 					packet_Count 			= (length > 0) ? ((length + max_Packet_Size - 1) / max_Packet_Size) : 1;

	USB_Device_In_Endpoint->DIEPTSIZ = (length << USB_OTG_DIEPTSIZ_XFRSIZ_Pos) | (packet_Count << USB_OTG_DIEPTSIZ_PKTCNT_Pos);
	USB_Device_In_Endpoint->DIEPCTL |= USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_CNAK;

	if(length > 0)
	{
	// if end-point is ISOCHRONOUS, transfer in
	//USB_LL___FIFO_Transfer_In(data, FIFO_Pointer, length);
	// if not ISOCHRONOUS, un-mask TX FIFO empty interrupt
	USB_LL_Device___TX_Endpoint[port_Number][endpoint_Number].TX_Buffer = data;
	USB_LL_Device___TX_Endpoint[port_Number][endpoint_Number].TX_Buffer_Size = length;
	USB_LL_Device___TX_Endpoint[port_Number][endpoint_Number].TX_Progress = 0;
	USB_LL_Device___TX_Endpoint[port_Number][endpoint_Number].TX_Packet_Size = max_Packet_Size;
	USB_LL_Device___Endpoint_Enable_TX_FIFO_Empty_Interrupt(port_Number, endpoint_Number);
	}
	else
	{
		USB_LL___FIFO_Transfer_In(NULL, USB_LL_Device___Endpoint_Get_TX_FIFO_Pointer(port_Number, endpoint_Number), 0);
	}
}

void USB_LL_Device___Flush_TX_FIFO(uint8_t port_Number, uint8_t endpoint_Number)
{
	USB_OTG_GlobalTypeDef* USB = USB_LL___Get_USB(port_Number);
	USB->GRSTCTL = (1 << USB_OTG_GRSTCTL_TXFNUM_Pos) | USB_OTG_GRSTCTL_TXFFLSH;
	while(USB->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);
}

void USB_LL_Device___Disable_Endpoint(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction)
{
	if (endpoint_Direction == USB_LL_Device___ENDPOINT_DERECTION_OUT)
	{
		USB_OTG_OUTEndpointTypeDef *USB_Device_Out_Endpoint = USB_LL___Get_USB_Device_OUT(port_Number, endpoint_Number);
		if(USB_Device_Out_Endpoint->DOEPCTL & USB_OTG_DOEPCTL_EPENA)
		{
			USB_Device_Out_Endpoint->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
			while(!(USB_Device_Out_Endpoint->DOEPCTL & USB_OTG_DOEPCTL_NAKSTS));
			USB_Device_Out_Endpoint->DOEPCTL |= USB_OTG_DOEPCTL_EPDIS;
			while (USB_Device_Out_Endpoint->DOEPCTL & USB_OTG_DOEPCTL_EPDIS);
		}
	}
	else
	{
		USB_OTG_INEndpointTypeDef *USB_Device_In_Endpoint = USB_LL___Get_USB_Device_IN(port_Number, endpoint_Number);
		if(USB_Device_In_Endpoint->DIEPCTL & USB_OTG_DIEPCTL_EPENA)
		{
			USB_Device_In_Endpoint->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;
			while(!(USB_Device_In_Endpoint->DIEPCTL & USB_OTG_DIEPCTL_NAKSTS));
			USB_Device_In_Endpoint->DIEPCTL |= USB_OTG_DIEPCTL_EPDIS;
			while(USB_Device_In_Endpoint->DIEPCTL & USB_OTG_DIEPCTL_EPDIS);
			USB_LL_Device___Flush_TX_FIFO(port_Number, endpoint_Number);
		}
	}
}

void USB_LL_Device___Endpoint_Transfer_Out(uint8_t port_Number, uint8_t endpoint_Number, uint32_t transfer_Size, uint8_t* buffer, uint32_t buffer_Size)
{
	USB_OTG_OUTEndpointTypeDef *USB_Device_Out_Endpoint = USB_LL___Get_USB_Device_OUT(port_Number, endpoint_Number);
	uint16_t max_Packet_Size = USB_LL_Device___Endpoint_Get_Max_Packet_Size(port_Number, endpoint_Number, USB_LL_Device___ENDPOINT_DERECTION_OUT);
	uint16_t packet_Count = (transfer_Size + max_Packet_Size - 1) / max_Packet_Size;

	USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Buffer 			= buffer;
	USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Buffer_Size		= buffer_Size;
	USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Buffer_Fill_Level 	= 0;

	USB_Device_Out_Endpoint->DOEPTSIZ = (transfer_Size << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | (packet_Count << USB_OTG_DOEPTSIZ_PKTCNT_Pos) | (3 << USB_OTG_DOEPTSIZ_STUPCNT_Pos);
	USB_Device_Out_Endpoint->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
}

uint8_t USB_LL_Device___Is_Endpoint_Busy(uint8_t port_Number, uint8_t endpoint_Number, uint8_t endpoint_Direction)
{
	if (endpoint_Direction == USB_LL_Device___ENDPOINT_DERECTION_OUT)
	{
		USB_OTG_OUTEndpointTypeDef* USB_Device_Out_Endpoint = USB_LL___Get_USB_Device_OUT(port_Number, endpoint_Number);
		return(!!(USB_Device_Out_Endpoint->DOEPCTL & (USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_EPDIS)));
	}
	else
	{
		USB_OTG_INEndpointTypeDef* USB_Device_In_Endpoint = USB_LL___Get_USB_Device_IN(port_Number, endpoint_Number);
		return(!!(USB_Device_In_Endpoint->DIEPCTL & (USB_OTG_DIEPCTL_EPENA | USB_OTG_DIEPCTL_EPDIS)));
	}
}

void USB_LL_Device___IN_Endpoint_Interrupt_Handler(uint8_t port_Number)
{
	uint8_t 					endpoint_Number 		= POSITION_VAL(USB_LL___Get_USB_Device(port_Number) -> DAINT & 0xff);
	USB_OTG_INEndpointTypeDef*	USB_Device_In_Endpoint 	= USB_LL___Get_USB_Device_IN(port_Number, endpoint_Number);
	USB_OTG_DeviceTypeDef*		USB_Device 				= USB_LL___Get_USB_Device(port_Number);
	//GPIOC->ODR |= (1<<0);			// set PC0 LOW
	//GPIOC->ODR &= ~(1<<0);			// set PC0 LOW

	uint32_t	check_TXFE = !!(USB_Device->DIEPEMPMSK & (1 << endpoint_Number));

	while(USB_Device_In_Endpoint->DIEPINT & (UBS_LL_Device___IN_ENDPOINT_INTERRUPT_MASK & ((~USB_OTG_DIEPINT_TXFE_Msk) | (check_TXFE << USB_OTG_DIEPINT_TXFE_Pos))))
	{
		switch(POSITION_VAL((USB_Device_In_Endpoint->DIEPINT) & UBS_LL_Device___IN_ENDPOINT_INTERRUPT_MASK))
		{
		case USB_OTG_DIEPINT_XFRC_Pos:
			USB_Device_In_Endpoint->DIEPINT = USB_OTG_DIEPINT_XFRC;
			//printf("USB in endpoint transfer complete\n");

			if (USB_LL_Device___TX_Endpoint[port_Number][endpoint_Number].TX_Callback != NULL)
			{
				USB_LL_Device___TX_Endpoint[port_Number][endpoint_Number].TX_Callback(port_Number, endpoint_Number);
			}

			break;
		case USB_OTG_DIEPINT_EPDISD_Pos:
			USB_Device_In_Endpoint->DIEPINT = USB_OTG_DIEPINT_EPDISD;
			break;
		case USB_OTG_DIEPINT_TOC_Pos:
			USB_Device_In_Endpoint->DIEPINT = USB_OTG_DIEPINT_TOC;
			break;
		case USB_OTG_DIEPINT_INEPNE_Pos:
			USB_Device_In_Endpoint->DIEPINT = USB_OTG_DIEPINT_INEPNE;
			break;
		case USB_OTG_DIEPINT_TXFE_Pos:
			{
				struct USB_LL_Device___TX_Endpoint* TX_Endpoint = &USB_LL_Device___TX_Endpoint[port_Number][endpoint_Number];
				uint16_t transfer_Length = TX_Endpoint->TX_Buffer_Size - TX_Endpoint->TX_Progress;

				if(transfer_Length > TX_Endpoint->TX_Packet_Size)
				{
					transfer_Length = TX_Endpoint->TX_Packet_Size;
				}

				while (((USB_LL_Device___Endpoint_Get_FIFO_Space(port_Number, endpoint_Number)*4) >= transfer_Length) && (TX_Endpoint->TX_Buffer_Size >  TX_Endpoint->TX_Progress))
				{
					USB_LL___FIFO_Transfer_In(&TX_Endpoint->TX_Buffer[TX_Endpoint->TX_Progress], USB_LL_Device___Endpoint_Get_TX_FIFO_Pointer(port_Number, endpoint_Number), transfer_Length);

					TX_Endpoint->TX_Progress += transfer_Length;

					transfer_Length = TX_Endpoint->TX_Buffer_Size - TX_Endpoint->TX_Progress;

					if(transfer_Length > 0)
					{
						uint8_t i = 0;
					}

					if(transfer_Length > TX_Endpoint->TX_Packet_Size)
					{
						transfer_Length = TX_Endpoint->TX_Packet_Size;
					}

					if (TX_Endpoint->TX_Progress >= TX_Endpoint->TX_Buffer_Size)
					{
						USB_LL_Device___Endpoint_Disable_TX_FIFO_Empty_Interrupt(port_Number, endpoint_Number);
					}
				}

			break;
			}
		case USB_OTG_DIEPINT_TXFIFOUDRN_Pos:
			USB_Device_In_Endpoint->DIEPINT = USB_OTG_DIEPINT_TXFIFOUDRN;
			break;
		case USB_OTG_DIEPINT_NAK_Pos:
			USB_Device_In_Endpoint->DIEPINT = USB_OTG_DIEPINT_NAK;
			printf("USB in endpoint NAK\n");
			break;
		}
		check_TXFE = !!(USB_Device->DIEPEMPMSK & (1 << endpoint_Number));
	}
}

void USB_LL_Device___OUT_Endpoint_Interrupt_Handler(uint8_t port_Number)
{
	uint8_t 					endpoint_Number 		= POSITION_VAL(USB_LL___Get_USB_Device(port_Number) -> DAINT >> 16);
	USB_OTG_OUTEndpointTypeDef*	USB_Device_Out_Endpoint = USB_LL___Get_USB_Device_OUT(port_Number, endpoint_Number);

	//GPIOC->ODR |= (1<<0);			// set PC0 LOW
	//GPIOC->ODR &= ~(1<<0);			// set PC0 LOW

	while(USB_Device_Out_Endpoint->DOEPINT & UBS_LL_Device___OUT_ENDPOINT_INTERRUPT_MASK)
	{
		switch(POSITION_VAL((USB_Device_Out_Endpoint->DOEPINT) & UBS_LL_Device___OUT_ENDPOINT_INTERRUPT_MASK))
		{
		case USB_OTG_DOEPINT_XFRC_Pos:
			USB_Device_Out_Endpoint->DOEPINT = USB_OTG_DOEPINT_XFRC;
			printf("USB out endpoint transfer complete\n");

			if (USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Callback != NULL)
			{

				USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Callback(port_Number, endpoint_Number, USB_LL_Device___PACKET_TYPE_DATA, USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Buffer, USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Buffer_Fill_Level);

			}

			break;
		case USB_OTG_DOEPINT_EPDISD_Pos:
			USB_Device_Out_Endpoint->DOEPINT = USB_OTG_DOEPINT_EPDISD;
			break;
		case USB_OTG_DOEPINT_STUP_Pos:
			USB_Device_Out_Endpoint->DOEPINT = USB_OTG_DOEPINT_STUP;
			printf("USB out endpoint STUP\n");

			if (USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Callback != NULL)
			{
				//GPIOC->ODR |= (1<<1);			// set PC1 LOW

				USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Callback(port_Number, endpoint_Number, USB_LL_Device___PACKET_TYPE_SETUP, USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Buffer, USB_LL_Device___RX_Endpoint[port_Number][endpoint_Number].RX_Buffer_Fill_Level);

				//GPIOC->ODR &= ~(1<<1);			// set PC1 LOW
			}

			break;
		case USB_OTG_DOEPINT_OTEPDIS_Pos:
			USB_Device_Out_Endpoint->DOEPINT = USB_OTG_DOEPINT_OTEPDIS;
			//printf("USB out endpoint OTEPDIS%d\n", endpoint_Number);
			break;
		case USB_OTG_DOEPINT_B2BSTUP_Pos:
			USB_Device_Out_Endpoint->DOEPINT = USB_OTG_DOEPINT_B2BSTUP;
			break;
		case USB_OTG_DOEPINT_NAK_Pos:
			USB_Device_Out_Endpoint->DOEPINT = USB_OTG_DOEPINT_NAK;
			printf("USB out endpoint NAK\n");
			break;
		}
	}
}
