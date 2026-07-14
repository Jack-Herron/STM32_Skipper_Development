/*
 * STM32H7_DELAY.h
 *
 *  Created on: Jul 13, 2026
 *      Author: Jack Herron
 */

#ifndef INC_STM32H7A3_DELAY_H_
#define INC_STM32H7A3_DELAY_H_

#include <stdint.h>

typedef enum
{
    DELAY_STATUS_OK = 0U,
    DELAY_STATUS_ERROR,
    DELAY_STATUS_INVALID_PARAMETER

} DELAY_StatusTypeDef;

DELAY_StatusTypeDef DELAY_Init(uint32_t CoreClockHz);

void DELAY_us(uint32_t Microseconds);
void DELAY_ms(uint32_t Milliseconds);

#endif /* INC_STM32H7A3_DELAY_H_ */
