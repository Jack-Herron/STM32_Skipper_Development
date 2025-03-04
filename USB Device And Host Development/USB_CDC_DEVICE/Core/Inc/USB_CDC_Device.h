/*
 * USB_CDC_Device.h
 *
 *  Created on: Nov 5, 2024
 *      Author: Jack Herron
 */

#ifndef USB_CDC_DEVICE_H_
#define USB_CDC_DEVICE_H_

#define USB_CDC_Device___RX_PACKET_BUFFER_SIZE		(0x40)
#define USB_CDC_Device___RX_MESSAGE_BUFFER_SIZE		(0x400)
#define USB_CDC_Device___TX_MESSAGE_BUFFER_SIZE		(0x400)

#define USB_CDC_Device___MESSAGE_RECEIVED_CALLBACK_PARAMETERS    uint8_t port_Number, uint8_t status, uint8_t interrupt_Char, uint8_t *data, uint16_t length

typedef struct USB_CDC_Device___CDC_Device
{
	uint8_t 	is_Enabled;
	uint8_t 	current_Configuration;
	uint8_t 	interface_Status[2];
	uint8_t 	line_Coding[7];
	uint16_t 	control_Line_State;
	uint8_t     RX_Message_Buffer[USB_CDC_Device___RX_MESSAGE_BUFFER_SIZE];
	uint8_t*    RX_Message_Buffer_Head;
	uint8_t     RX_Message_Buffer_Overflow_Flag;
	uint8_t 	RX_Packet_Buffer[USB_CDC_Device___RX_PACKET_BUFFER_SIZE];
	uint32_t	interrupt_Char[8];
	uint8_t 	TX_Message_Buffer[USB_CDC_Device___TX_MESSAGE_BUFFER_SIZE];
	uint8_t*	TX_Message_Buffer_Write_Head;
	uint8_t*	TX_Message_Buffer_Read_Head;
	void (*message_Received_Callback)(USB_CDC_Device___MESSAGE_RECEIVED_CALLBACK_PARAMETERS);
}USB_CDC_Device___CDC_Device_TypeDef;

uint8_t USB_CDC_Device___Send_Data						(uint8_t port_Number, char *data, uint16_t length);
uint8_t USB_CDC_Device___Is_Enabled						(uint8_t port_Number);
uint8_t USB_CDC_Device___Is_DTE							(uint8_t port_Number);
void 	USB_CDC_Device___Init							(uint8_t port_Number);
void 	USB_CDC_Device___Set_Interrupt_Char				(uint8_t port_Number, char interrupt_Char);
void 	USB_CDC_Device___Set_Message_Received_Callback	(uint8_t port_Number, void callback(USB_CDC_Device___MESSAGE_RECEIVED_CALLBACK_PARAMETERS));
#endif /* USB_CDC_DEVICE_H_ */
