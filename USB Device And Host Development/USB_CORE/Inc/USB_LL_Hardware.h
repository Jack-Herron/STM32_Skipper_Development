/*
 * USB_LL_Hardware.h
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_HARDWARE_H_
#define INC_USB_LL_HARDWARE_H_

#define USB_LL_Hardware___GET_BIT_SEGMENT(word, mask, pos)			((word & mask) >> pos)
#define USB_LL_Hardware___PORT_0									0
#define USB_LL_Hardware___PORT_1									1
#define USB_LL_Hardware___DEVICE_MODE								0
#define USB_LL_Hardware___HOST_MODE									1
#define USB_LL_Hardware___USB_ENABLE_DELAY							0x80
#define USB_LL_Hardware___HOST_RESET_DELAY							0x19
#define USB_LL_Hardware___HCFG_SELECT_48Mhz_PHY_FREQUENCY			0x01
#define USB_LL_Hardware___DCFG_SELECT_48Mhz_PHY_FREQUENCY			0x03
#define USB_LL_Hardware___HPRT_Port_Power_Enabled					0x01
#define USB_LL_Hardware___CHANGE_MODE_DELAY							0x19
#define USB_LL_Hardware___TRDT_Value								0x06
#define USB_LL_Hardware___GPIO_ALTERNATE_MODE						0x02
#define USB_LL_Hardware___GPIO_Alt_FUNCTION_10						0x0a
#define USB_LL_Hardware___GPIO_Alt_FUNCTION_12						0x0c
#define USB_LL_Hardware___NON_PERIODIC_TX_FIFO						0x00
#define USB_LL_Hardware___PERIODIC_TX_FIFO							0x01

typedef struct
{
  __IO uint32_t HPRT;          		 /*!< Host Port Register    440h */
} USB_OTG_HostPortTypeDef;

uint32_t USB_LL_Hardware___Get_USB_BASE(uint8_t port_number);
USB_OTG_GlobalTypeDef* USB_LL_Hardware___Get_USB(uint8_t port_Number);
USB_OTG_DeviceTypeDef* USB_LL_Hardware___Get_USB_Device(uint8_t port_Number);
USB_OTG_HostTypeDef* USB_LL_Hardware___Get_USB_Host(uint8_t port_Number);
USB_OTG_HostPortTypeDef* USB_LL_Hardware___Get_USB_Host_Port(uint8_t port_Number);
USB_OTG_HostChannelTypeDef* USB_LL_Hardware___Get_USB_Host_Channel(uint8_t port_Number, uint8_t channel_Number);
USB_OTG_INEndpointTypeDef* USB_LL_Hardware___Get_USB_Device_IN(uint8_t port_Number, uint8_t endpoint_Number);
USB_OTG_OUTEndpointTypeDef* USB_LL_Hardware___Get_USB_Device_OUT(uint8_t port_Number, uint8_t endpoint_Number);


void USB_LL_Hardware___Init(uint8_t port_Number, uint8_t port_Mode);
void USB_LL_Hardware___GPIO_Init(uint8_t port_Number);
uint16_t USB_LL_Hardware___Host_Get_Frame_Number(uint8_t port_Number);
uint32_t USB_LL_Hardware___Host_Get_FIFO_Space_Available(uint8_t port_Number, uint8_t FIFO_Type);
void USB_LL_Hardware___Host_Set_FIFO_Size(uint8_t port_Number, uint32_t RX_FIFO_Depth, uint32_t non_Periodic_TX_FIFO_Depth, uint32_t periodic_TX_FIFO_Depth);

#endif /* INC_USB_LL_HARDWARE_H_ */
