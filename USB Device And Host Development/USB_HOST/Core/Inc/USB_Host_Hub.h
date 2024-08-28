/*
 * USB_Host_Hub.h
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_HUB_H_
#define CORE_INC_USB_HOST_HUB_H_

#include <stdint.h>
#include "../../Config/USB_Host_Config.h"

#define USB_Host_Hub___QUICK_DIV_ROOF(number, diviser) 				((number + (diviser-1)) / diviser)

#define USB_Host_Hub___HUB_DEVICE_CLASS								0x09
#define USB_Host_Hub___NUMBER_OF_BITS_IN_BYTE						(0x08)
#define USB_Host_Hub___HUB_PORT_MASK_SIZE_IN_BYTES					USB_Host_Hub___QUICK_DIV_ROOF(USB_Host_Config___MAX_HUB_PORTS, USB_Host_Hub___NUMBER_OF_BITS_IN_BYTE)

#define USB_Host_Hub___HUB_SETUP_STAGE_SET_CONFIGURATION            0x00
#define USB_Host_Hub___HUB_SETUP_STAGE_GET_SHORT_HUB_DESCRIPTOR     0x01
#define USB_Host_Hub___HUB_SETUP_STAGE_GET_FULL_HUB_DESCRIPTOR      0x02
#define USB_Host_Hub___HUB_SETUP_STAGE_ENABLE_PORT					0x03

void USB_Host_Hub___Initiate_Hub(uint8_t port_Number, uint8_t device_Address);

typedef struct __attribute__((__packed__)){
	uint8_t  bDescriptorLength;
	uint8_t  bDescriptorType;
	uint8_t  bNumberOfPorts;
	uint16_t wHubCharacteristics;
	uint8_t  bPowerOnToPowerGood;
	uint8_t  bHubControlCurrent;
	uint8_t  bRemoveAndPowerMask[USB_Host_Hub___HUB_PORT_MASK_SIZE_IN_BYTES * 2];
}USB_Host_Hub___Hub_Descriptor_TypeDef;

typedef struct{
	uint8_t                                 port_Number;
	uint8_t 								address;
	USB_Host_Hub___Hub_Descriptor_TypeDef 	descriptor;
	uint8_t 								hub_Setup_Stage;

}USB_Host_Hub___Hub_TypeDef;

typedef struct USB_Host_Hub___Hub_Node
{
	uint8_t is_Allocated;
	USB_Host_Hub___Hub_TypeDef      hub;
	struct USB_Host_Hub___Hub_Node* next_Node;
	struct USB_Host_Hub___Hub_Node* previous_Node;
} USB_Host_Hub___Hub_Node_TypeDef;

typedef struct {
	USB_Host_Hub___Hub_Node_TypeDef* 	first_Node;
	USB_Host_Hub___Hub_Node_TypeDef* 	last_Node;
}USB_Host_Hub___Hub_Queue_TypeDef;

#endif /* CORE_INC_USB_HOST_HUB_H_ */
