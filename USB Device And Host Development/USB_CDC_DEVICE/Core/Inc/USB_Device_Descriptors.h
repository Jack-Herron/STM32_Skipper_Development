/*
 * USB_Device_Descriptors.h
 *
 *  Created on: Jan 23, 2025
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_DEVICE_DESCRIPTORS_H_
#define CORE_INC_USB_DEVICE_DESCRIPTORS_H_

#include <stdint.h>

#define USB_Device_Descriptors___DESCRIPTOR_TYPE_DEVICE			(0x01)
#define USB_Device_Descriptors___DESCRIPTOR_TYPE_CONFIGURATION	(0x02)
#define USB_Device_Descriptors___DESCRIPTOR_TYPE_STRING			(0x03)
#define USB_Device_Descriptors___MAX_ENTRIES                    (0x08)

typedef struct USB_Device_Descriptors___Entry
{
	uint8_t 	descriptor_Type;
	uint8_t 	index;
	uint8_t*	descriptor;
	uint16_t 	descriptor_Size;
} USB_Device_Descriptors___Entry_TypeDef;

void USB_Device_Descriptors___Add_Entry(uint8_t port_Number, uint8_t descriptor_Type, uint8_t index, uint8_t* descriptor, uint16_t descriptor_Size);
void USB_Device_Descriptors___Remove_Entry(uint8_t port_Number, uint8_t* descriptor);
USB_Device_Descriptors___Entry_TypeDef* USB_Device_Descriptors___Get_Descriptor(uint8_t port_Number, uint8_t descriptor_Type, uint8_t index);

#endif /* CORE_INC_USB_DEVICE_DESCRIPTORS_H_ */
