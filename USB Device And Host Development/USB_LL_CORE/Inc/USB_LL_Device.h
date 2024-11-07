/*
 * USB_LL_Device.h
 *
 *  Created on: Jun 26, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_DEVICE_H_
#define INC_USB_LL_DEVICE_H_

#include <stdint.h>

#define USB_LL_Device___PORT_ZERO 					0
#define USB_LL_Device___PORT_ONE 					0
#define USB_LL_Device___PORT_ZERO_NUM_ENDPOINTS 	4
#define USB_LL_Device___PORT_ONE_NUM_ENDPOINTS 		6

typedef struct
{
	uint16_t RX_FIFO_Depth;
	uint16_t Endpoint_TX_FIFO_Depth[6];
} USB_LL_Device___FIFO_Config_TypeDef;

void USB_LL_Device___Set_FIFO_Size(uint8_t port_Number, USB_LL_Device___FIFO_Config_TypeDef FIFO_Config);

#endif /* INC_USB_LL_DEVICE_H_ */
