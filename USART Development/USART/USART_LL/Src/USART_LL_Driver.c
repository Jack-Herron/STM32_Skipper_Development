/*
 * USART_LL_Driver.c
 *
 *  Created on: Nov 12, 2024
 *      Author: Jack Herron
 */

#include <stm32f4xx.h>									// Include STM32F4 specific definitions
#include <stdlib.h>
#include <USART_LL_Driver.h>

USART_TypeDef* USART_LL_Driver___Get_USART(uint8_t USART_Number)
{
	switch (USART_Number)
	{
	case 1:
		return USART1;
	case 2:
		return USART2;
	case 3:
		return USART3;
	case 4:
		return UART4;
	case 5:
		return UART5;
	case 6:
		return USART6;
	default:
		return NULL;
	}
}

void USART_LL_Driver___Enable_USART_Clock(uint8_t USART_Number)
{
	switch (USART_Number)
	{
	case 1:
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		break;
	case 2:
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		break;
	case 3:
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
		break;
	case 4:
		RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
		break;
	case 5:
		RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
		break;
	case 6:
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
		break;
	}
}

void USART_LL_Driver___Enable_GPIO(uint8_t USART_Number)
{
	switch (USART_Number)
	{
	case 1:
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
		GPIOB->MODER |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;
		break;
	case 2:

		break;
	case 3:

		break;
	case 4:

		break;
	case 5:

		break;
	case 6:

		break;
	}
}


void USART_LL_Driver___Init(uint8_t USART_Number)
{
	USART_TypeDef *USART = USART_LL_Driver___Get_USART(USART_Number);

	if (USART != NULL)
	{
		USART_LL_Driver___Enable_USART_Clock(USART_Number);

		USART->CR1 |= USART_CR1_UE; 	// Enable USART
		USART->CR1 |= USART_CR1_TE; 	// Enable Transmitter
		USART->CR1 |= USART_CR1_RE; 	// Enable Receiver
		USART->CR1 |= USART_CR1_RXNEIE; // Enable RXNE interrupt
		USART->CR1 |= USART_CR1_TCIE; 	// Enable TC interrupt
		USART->CR1 |= USART_CR1_TXEIE; 	// Enable TXE interrupt
	}
}
