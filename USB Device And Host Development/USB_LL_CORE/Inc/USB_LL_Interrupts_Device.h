/*
 * USB_LL_Interrupts_Device.h
 *
 *  Created on: Jun 27, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_INTERRUPTS_DEVICE_H_
#define INC_USB_LL_INTERRUPTS_DEVICE_H_

#include <stdint.h>

#define USB_LL_Interrupts_Device___RX_CALLBACK_PARAMETERS  uint8_t port_Number, uint8_t *data, uint16_t length
#define USB_LL_Interrupts_Device___RX_PACKET_STATUS_DATA_PACKET_RECIEVED	0x02
#define USB_LL_Interrupts_Device___RX_PACKET_STATUS_SETUP_PACKET_RECIEVED	0x06
void USB_LL_Interrupts_Device___Set_RX_Callback(uint8_t port_Number, void (*callback)(USB_LL_Interrupts_Device___RX_CALLBACK_PARAMETERS));
void USB_LL_Interrupts_Device___Packet_Received(uint8_t port_Number);
void USB_LL_Interrupts_Device___Host_Connected(uint8_t port_Number);

#endif /* INC_USB_LL_INTERRUPTS_DEVICE_H_ */
