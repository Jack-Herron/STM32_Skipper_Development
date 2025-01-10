/*
 * USB_LL_Definitions.h
 *
 *  Created on: Jun 26, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_DEFINITIONS_H_
#define INC_USB_LL_DEFINITIONS_H_

#include <stm32f4xx.h>				// include MCU specific definitions

#define USB_LL_Definitions___get_Max(a, b)                          (((a) > (b)) ? (a) : (b))
#define USB_LL_Definitions___get_Min(a, b)                          (((a) < (b)) ? (a) : (b))
#define USB_LL_Definitions___NUMBER_OF_PORTS						(0x02)
#define USB_LL_Definitions___MAX_NUMBER_OF_CHANNELS_PER_PORT		USB_LL_Definitions___get_Min(USB_OTG_HS_HOST_MAX_CHANNEL_NBR, USB_OTG_FS_HOST_MAX_CHANNEL_NBR)

#endif /* INC_USB_LL_DEFINITIONS_H_ */
