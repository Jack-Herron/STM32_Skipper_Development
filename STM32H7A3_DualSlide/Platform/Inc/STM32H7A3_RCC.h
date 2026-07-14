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
int32_t RCC_EnablePeripheralClock(void *Peripheral);
int32_t RCC_ResetPeripheral(void *Peripheral);
int32_t RCC_DisablePeripheralClock(void *Peripheral);

#endif /* STM32H7A3_RCC_H_ */
