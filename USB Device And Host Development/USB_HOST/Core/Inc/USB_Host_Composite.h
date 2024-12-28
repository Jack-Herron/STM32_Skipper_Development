/*
 * USB_Host_Composite.h
 *
 *  Created on: Dec 27, 2024
 *      Author: jackh
 */

#ifndef CORE_INC_USB_HOST_COMPOSITE_H_
#define CORE_INC_USB_HOST_COMPOSITE_H_

#include "../../Config/USB_Host_Config.h"

typedef struct
{
	void (*device_Disconnected_Callback[USB_Host_Config___MAX_COMPOSITE_DEVICE_INTERFACES])(uint8_t port_Number, uint8_t device_Address, uint8_t configuration_Number, uint8_t interface_Number);
} USB_Host_Composite___Composite_Device;



#endif /* CORE_INC_USB_HOST_COMPOSITE_H_ */
