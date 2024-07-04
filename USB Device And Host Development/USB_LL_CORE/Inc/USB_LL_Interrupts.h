/*
 * USB_LL_Interrupts.h
 *
 *  Created on: Jun 21, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_INTERRUPTS_H_
#define INC_USB_LL_INTERRUPTS_H_

#define USB_LL_Interrupts___WORD_32_MSB								0x1f
#define USB_LL_Interrupts___GLOBAL_INTERRUPTS_MASK					0xa33c7c98
#define USB_LL_Interrupts___IN_ENDPOINT_INTERRUPTS_MASK				0x000b
#define USB_LL_Interrupts___OUT_ENDPOINT_INTERRUPTS_MASK			0x000b

typedef struct {
	uint8_t 									start_Of_Frame;

} USB_LL_Interrupts___Status_TypeDef;

#endif /* INC_USB_LL_INTERRUPTS_H_ */
