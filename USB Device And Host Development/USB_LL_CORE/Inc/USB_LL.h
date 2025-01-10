/*
 * USB_LL.h
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_H_
#define INC_USB_LL_H_

#define USB_LL___WORD_32_MSB								0x1f
#define USB_LL___GLOBAL_INTERRUPTS_MASK						0xa33c7c98
#define USB_LL___IN_ENDPOINT_INTERRUPTS_MASK				0x000b
#define USB_LL___OUT_ENDPOINT_INTERRUPTS_MASK				0x000b

#define USB_LL___QUICK_DIV_ROOF(number, diviser) 			((number + (diviser-1)) / diviser)
#define USB_LL___GET_BIT_SEGMENT(word, mask, pos)			((word & mask) >> pos)
#define USB_LL___PORT_0										0
#define USB_LL___PORT_1										1
#define USB_LL___DEVICE_MODE								0
#define USB_LL___HOST_MODE									1
#define USB_LL___USB_ENABLE_DELAY							0x80
#define USB_LL___HCFG_SELECT_48Mhz_PHY_FREQUENCY			0x01
#define USB_LL___DCFG_SELECT_48Mhz_PHY_FREQUENCY			0x03
#define USB_LL___HPRT_Port_Power_Enabled					0x01
#define USB_LL___CHANGE_MODE_DELAY							0x19
#define USB_LL___TRDT_Value									0x06
#define USB_LL___GPIO_ALTERNATE_MODE						0x02
#define USB_LL___GPIO_Alt_FUNCTION_10						0x0a
#define USB_LL___GPIO_Alt_FUNCTION_12						0x0c
#define USB_LL___NUMBER_OF_BYTES_IN_WORD_32					0x04
#define USB_LL___NUMBER_OF_BITS_IN_BYTE						0x08

typedef struct
{
  __IO uint32_t HPRT;          		 /*!< Host Port Register    440h */
} USB_OTG_HostPortTypeDef;

uint32_t		 				USB_LL___Get_USB_BASE			(uint8_t port_number);
USB_OTG_GlobalTypeDef* 			USB_LL___Get_USB				(uint8_t port_Number);
USB_OTG_DeviceTypeDef* 			USB_LL___Get_USB_Device		(uint8_t port_Number);
USB_OTG_HostTypeDef* 			USB_LL___Get_USB_Host			(uint8_t port_Number);
USB_OTG_HostPortTypeDef* 		USB_LL___Get_USB_Host_Port		(uint8_t port_Number);
USB_OTG_HostChannelTypeDef* 	USB_LL___Get_USB_Host_Channel	(uint8_t port_Number, uint8_t channel_Number);
USB_OTG_INEndpointTypeDef* 		USB_LL___Get_USB_Device_IN		(uint8_t port_Number, uint8_t endpoint_Number);
USB_OTG_OUTEndpointTypeDef* 	USB_LL___Get_USB_Device_OUT	(uint8_t port_Number, uint8_t endpoint_Number);
void 							USB_LL___FIFO_Transfer_Out		(uint32_t* Source, uint8_t* Destination, uint32_t size);
void 							USB_LL___FIFO_Transfer_In		(uint8_t* Source, uint32_t* Destination, uint32_t transfer_Size);
void 							USB_LL___Init					(uint8_t port_Number, uint8_t port_Mode);
void 							USB_LL___GPIO_Init				(uint8_t port_Number);

#endif /* INC_USB_LL_H_ */
