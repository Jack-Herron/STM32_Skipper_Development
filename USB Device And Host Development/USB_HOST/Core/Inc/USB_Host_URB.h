/*
 * USB_Host_URB.h
 *
 *  Created on: Jul 8, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_URB_H_
#define CORE_INC_USB_HOST_URB_H_

#include <stdint.h>

typedef struct{
	uint8_t 							port_Number;
	uint8_t 							transfer_Direction;
	uint8_t 							device_Address;
	uint8_t								transfer_Type;
	uint32_t							transfer_Length;
	uint8_t								control_Setup_Packet[8];
}USB_Host_URB___URB;



#endif /* CORE_INC_USB_HOST_URB_H_ */
