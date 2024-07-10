/*
 * USB_Host_URB.h
 *
 *  Created on: Jul 8, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_TRANSFERS_H_
#define CORE_INC_USB_HOST_TRANSFERS_H_

#include <stdint.h>
#include "../Inc/USB_Host_Device_Manager.h"

#define USB_Host_Transfers___CONTROL_SETUP_PACKET_LENGTH 		8

typedef enum
{
	URB_Transfer_Stage___SETUP,
	URB_Transfer_Stage___DATA,
	URB_Transfer_Stage___STATUS,
	URB_Transfer_Stage___COMPLETE
}USB_Host_Transfers___URB_Transfer_Stage_TypeDef;

typedef struct{
	uint16_t											URB_ID;
	uint8_t												device_Address;
	uint8_t 											transfer_Direction;
	USB_Host_Transfers___URB_Transfer_Stage_TypeDef		transfer_Stage;
	uint8_t 											busy;
	uint8_t												transfer_Type;
	uint32_t											transfer_Length;
	uint8_t												control_Setup_Packet[USB_Host_Transfers___CONTROL_SETUP_PACKET_LENGTH];
	uint8_t*											transfer_Buffer;
	void(*URB_Callback)									(uint8_t, uint8_t);
}USB_Host_Transfers___URB_TypeDef;

#endif /* CORE_INC_USB_HOST_TRANSFERS_H_ */
