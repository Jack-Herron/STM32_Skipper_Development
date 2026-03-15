/*
 * Indicate.c
 *
 *  Created on: Mar 14, 2026
 *      Author: jackh
 */

#include <stdint.h>
#include "stm32f1xx.h"
#include "indicate.h"

void indicate___GPIO_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;				// Enable GPIOB clock
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;				// Enable GPIOB clock
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;   			// Enable AFIO clock

	// Disable JTAG, keep SWD enabled
	AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG_Msk;
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

	GPIOB->CRL &= ~GPIO_CRL_MODE5_Msk;				// Clear PB5 mode
	GPIOB->CRL &= ~GPIO_CRL_CNF5_Msk;				// clear PB5 configuration

	GPIOB->CRL |= GPIO_CRL_MODE5_0;					// Set PB5 to output (slow) for on board LED

	GPIOB->ODR &= ~GPIO_ODR_ODR5;					// Clear on board LED 1

	GPIOB->CRL &= ~GPIO_CRL_MODE4_Msk;				// Clear PB4 mode
	GPIOB->CRL &= ~GPIO_CRL_CNF4_Msk;				// clear PB4 configuration

	GPIOB->CRL |= GPIO_CRL_MODE4_0;					// Set PB4 to output (slow) for on board LED

	GPIOB->ODR &= ~GPIO_ODR_ODR4;					// Clear on board LED 2

	GPIOA->CRH &= ~GPIO_CRH_MODE15_Msk;				// Clear PA15 mode
	GPIOA->CRH &= ~GPIO_CRH_CNF15_Msk;				// clear PA15 configuration

	GPIOA->CRH |= GPIO_CRH_MODE15_0;				// Set PA15 to output (slow) for on board LED

	GPIOA->ODR &= ~GPIO_ODR_ODR15;					// Clear on board LED 3
}

void indicate___Set_Value(uint8_t value)
{
	GPIOB->ODR &= ~GPIO_ODR_ODR5;
	GPIOB->ODR &= ~GPIO_ODR_ODR4;
	GPIOA->ODR &= ~GPIO_ODR_ODR15;

	GPIOA->ODR |= (!(!(value & 0x04)) << GPIO_ODR_ODR15_Pos);
	GPIOB->ODR |= (!(!(value & 0x02)) << GPIO_ODR_ODR4_Pos);
	GPIOB->ODR |= (!(!(value & 0x01)) << GPIO_ODR_ODR5_Pos);
}

void indicate___Init()
{
	indicate___GPIO_Init();
}
