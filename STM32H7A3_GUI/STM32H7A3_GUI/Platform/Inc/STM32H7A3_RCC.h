/*
 * STM32H7A3_RCC.h
 *
 *  Created on: May 21, 2026
 *      Author: Jack Herron
 */

#ifndef STM32H7A3_RCC_H_
#define STM32H7A3_RCC_H_

void RCC_Init(void);
int32_t RCC_GetKernelFreq(void *peripheral);

#endif /* STM32H7A3_RCC_H_ */
