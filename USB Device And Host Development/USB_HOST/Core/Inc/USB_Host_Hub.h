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
#define USB_Host_Hub___HUB_DESCRIPTOR_TYPE							0x29
#define USB_Host_Hub___HUB_DESCRIPTOR_BASE_LENGTH					0x07

#define USB_Host_Hub___HUB_SETUP_STAGE_SET_CONFIGURATION            0x00
#define USB_Host_Hub___HUB_SETUP_STAGE_GET_SHORT_HUB_DESCRIPTOR     0x01
#define USB_Host_Hub___HUB_SETUP_STAGE_GET_FULL_HUB_DESCRIPTOR      0x02
#define USB_Host_Hub___HUB_SETUP_STAGE_ENABLE_PORTS					0x03
#define USB_Host_Hub___HUB_SETUP_STAGE_COMPLETE						0x04

#define USB_Host_Hub___wPortChange_C_PORT_CONNECTION_Pos			(0x00)
#define USB_Host_Hub___wPortChange_C_PORT_ENABLE_Pos				(0x01)
#define USB_Host_Hub___wPortChange_C_PORT_SUSPEND_Pos				(0x02)
#define USB_Host_Hub___wPortChange_C_PORT_OVER_CURRENT_Pos			(0x03)
#define USB_Host_Hub___wPortChange_C_PORT_RESET_Pos				 	(0x04)

#define USB_Host_Hub___wPortChange_C_PORT_CONNECTION_Msk			(1 << USB_Host_Hub___wPortChange_C_PORT_CONNECTION_Pos)
#define USB_Host_Hub___wPortChange_C_PORT_ENABLE_Msk				(1 << USB_Host_Hub___wPortChange_C_PORT_ENABLE_Pos)
#define USB_Host_Hub___wPortChange_C_PORT_SUSPEND_Msk				(1 << USB_Host_Hub___wPortChange_C_PORT_SUSPEND_Pos)
#define USB_Host_Hub___wPortChange_C_PORT_OVER_CURRENT_Msk			(1 << USB_Host_Hub___wPortChange_C_PORT_OVER_CURRENT_Pos)
#define USB_Host_Hub___wPortChange_C_PORT_RESET_Msk				 	(1 << USB_Host_Hub___wPortChange_C_PORT_RESET_Pos)

#define USB_Host_Hub___wPortStatus_PORT_CONNECTION_Pos				(0x00)
#define USB_Host_Hub___wPortStatus_PORT_ENABLE_Pos					(0x01)
#define USB_Host_Hub___wPortStatus_PORT_SUSPEND_Pos					(0x02)
#define USB_Host_Hub___wPortStatus_PORT_OVER_CURRENT_Pos			(0x03)
#define USB_Host_Hub___wPortStatus_PORT_RESET_Pos				 	(0x04)
#define USB_Host_Hub___wPortStatus_PORT_POWER_Pos				 	(0x08)
#define USB_Host_Hub___wPortStatus_PORT_LOW_SPEED_Pos				(0x09)
#define USB_Host_Hub___wPortStatus_PORT_HIGH_SPEED_Pos		 		(0x0a)
#define USB_Host_Hub___wPortStatus_PORT_TEST_Pos				 	(0x0b)
#define USB_Host_Hub___wPortStatus_PORT_INDICATOR_Pos			 	(0x0c)

#define USB_Host_Hub___wPortStatus_PORT_CONNECTION_Msk				(1 << USB_Host_Hub___wPortStatus_PORT_CONNECTION_Pos)
#define USB_Host_Hub___wPortStatus_PORT_ENABLE_Msk					(1 << USB_Host_Hub___wPortStatus_PORT_ENABLE_Pos)
#define USB_Host_Hub___wPortStatus_PORT_SUSPEND_Msk					(1 << USB_Host_Hub___wPortStatus_PORT_SUSPEND_Pos)
#define USB_Host_Hub___wPortStatus_PORT_OVER_CURRENT_Msk			(1 << USB_Host_Hub___wPortStatus_PORT_OVER_CURRENT_Pos)
#define USB_Host_Hub___wPortStatus_PORT_RESET_Msk				 	(1 << USB_Host_Hub___wPortStatus_PORT_RESET_Pos)
#define USB_Host_Hub___wPortStatus_PORT_POWER_Msk				 	(1 << USB_Host_Hub___wPortStatus_PORT_POWER_Pos)
#define USB_Host_Hub___wPortStatus_PORT_LOW_SPEED_Msk				(1 << USB_Host_Hub___wPortStatus_PORT_LOW_SPEED_Pos)
#define USB_Host_Hub___wPortStatus_PORT_HIGH_SPEED_Msk		 		(1 << USB_Host_Hub___wPortStatus_PORT_HIGH_SPEED_Pos)
#define USB_Host_Hub___wPortStatus_PORT_TEST_Msk				 	(1 << USB_Host_Hub___wPortStatus_PORT_TEST_Pos)
#define USB_Host_Hub___wPortStatus_PORT_INDICATOR_Msk			 	(1 << USB_Host_Hub___wPortStatus_PORT_INDICATOR_Pos)

#define USB_Host_Hub___FEATURE_C_HUB_LOCAL_POWER	 				(0x00)
#define USB_Host_Hub___FEATURE_C_HUB_OVER_CURRENT		 			(0x01)
#define USB_Host_Hub___FEATURE_PORT_CONNECTION	 				 	(0x00)
#define USB_Host_Hub___FEATURE_PORT_ENABLE		 				 	(0x01)
#define USB_Host_Hub___FEATURE_PORT_SUSPEND		 				 	(0x02)
#define USB_Host_Hub___FEATURE_PORT_OVER_CURRENT	 				(0x03)
#define USB_Host_Hub___FEATURE_PORT_RESET			 				(0x04)
#define USB_Host_Hub___FEATURE_PORT_POWER			 				(0x08)
#define USB_Host_Hub___FEATURE_PORT_LOW_SPEED		 				(0x09)
#define USB_Host_Hub___FEATURE_C_PORT_CONNECTION	 				(0x10)
#define USB_Host_Hub___FEATURE_C_PORT_ENABLE		 				(0x11)
#define USB_Host_Hub___FEATURE_C_PORT_SUSPEND	 				 	(0x12)
#define USB_Host_Hub___FEATURE_C_PORT_OVER_CURRENT			 	 	(0x13)
#define USB_Host_Hub___FEATURE_C_PORT_RESET	 				 	 	(0x14)
#define USB_Host_Hub___FEATURE_PORT_TEST		 				 	(0x15)
#define USB_Host_Hub___FEATURE_PORT_INDICATOR	 				 	(0x16)

#define USB_Host_Hub___STANDARD_NUMBER_OF_RETRIES					0xffff

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
	uint8_t 		is_Powered;
}USB_Host_Hub___Hub_Port_Typedef;

typedef struct{
	uint8_t                                 port_Number;
	uint8_t 								device_Address;
	uint8_t									polling_Interval;
	USB_Host_Hub___Hub_Descriptor_TypeDef 	descriptor;
	uint8_t 								setup_Stage;
	USB_Host_Hub___Hub_Port_Typedef			port[USB_Host_Config___MAX_HUB_PORTS];
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
