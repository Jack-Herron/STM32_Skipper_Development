/*
 * USB_CDC_Device.h
 *
 *  Created on: Nov 5, 2024
 *      Author: Jack Herron
 */

#ifndef USB_CDC_DEVICE_H_
#define USB_CDC_DEVICE_H_

void USB_CDC_Device___Init(uint8_t port_Number);

typedef struct USB_CDC_Device___CDC_Device
{
	uint8_t is_Enabled;
	uint8_t current_Configuration;
	uint8_t interface_Status[2];
	uint8_t line_Coding[7];
	uint8_t control_Line_State;
}USB_CDC_Device___CDC_Device_TypeDef;

void 	USB_CDC_Device___Send_Data(uint8_t port_Number, char *data, uint16_t length);
uint8_t USB_CDC_Device___Is_Enabled(uint8_t port_Number);
#endif /* USB_CDC_DEVICE_H_ */
