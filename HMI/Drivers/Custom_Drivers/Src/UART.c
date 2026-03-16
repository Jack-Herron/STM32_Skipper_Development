/*
 * UART.c
 *
 *  Created on: Mar 15, 2026
 *      Author: jackh
 */

#include "stm32f4xx.h"
#include "Clock.h"
#include "UART.h"


void UART___Init(uint32_t BR)
{
	UART4->CR1 = 0;
	UART4->CR1 |= USART_CR1_UE;
}
