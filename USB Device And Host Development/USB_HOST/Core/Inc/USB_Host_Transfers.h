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

typedef struct{
	uint8_t										is_Allocated;
	USB_Host_Device_Manager___Device_TypeDef*	p_Device;
	uint8_t 									transfer_Direction;
	uint8_t										transfer_Type;
	uint32_t									transfer_Length;
	uint8_t										control_Setup_Packet[8];
	uint8_t*									transfer_Buffer;
	void(*URB_Callback)							(uint8_t, uint8_t);
}USB_Host_Transfers___URB_TypeDef;

#endif /* CORE_INC_USB_HOST_TRANSFERS_H_ */
