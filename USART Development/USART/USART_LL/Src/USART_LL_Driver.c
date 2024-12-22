/*
 * USART_LL_Driver.c
 *
 *  Created on: Nov 12, 2024
 *      Author: Jack Herron
 */

#include <stm32f4xx.h>									// Include STM32F4 specific definitions
#include <stdlib.h>
#include <USART_LL_Driver.h>
#include <Skipper_Clock.h>

void (*USART_LL_Driver___TX_Callback)(uint8_t port_Number);
void (*USART_LL_Driver___RX_Callback)(uint8_t port_Number, uint8_t data);

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
		RCC   -> AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
		GPIOB -> MODER   |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;
		GPIOB -> AFR[0]  |= (7 << 24) | (7 << 28);
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

void USART_LL_Driver___Enable_Interrupt(uint8_t USART_Number)
{
	switch (USART_Number)
	{
	case 1:
		NVIC_EnableIRQ(USART1_IRQn);
		NVIC_SetPriority(USART1_IRQn, 1);
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

void USART_LL_Driver___Set_Baud_Rate(uint8_t USART_Number, uint32_t baud_Rate)
{
	USART_TypeDef *USART = USART_LL_Driver___Get_USART(USART_Number);
	uint32_t pClk;
	if(USART_Number == 1 || USART_Number == 6)
	{
		pClk = Skipper_Clock___APB2_PERIPHERAL_FREQUENCY;
	}
	else
	{
		pClk = Skipper_Clock___APB1_PERIPHERAL_FREQUENCY;
	}
	uint32_t divisor 	= pClk 		/ baud_Rate;
	uint16_t mantissa 	= divisor 	/ 16;
	uint16_t fractional = divisor 	% 16;

	USART -> BRR = (mantissa << USART_BRR_DIV_Mantissa_Pos) | fractional;
}

void USART_LL_Driver___Set_TX_Callback(void callback(uint8_t))
{
	USART_LL_Driver___TX_Callback = callback;
}

void USART_LL_Driver___Set_RX_Callback(void callback(uint8_t, uint8_t))
{
	USART_LL_Driver___RX_Callback = callback;
}

void USART_LL_Driver___Init(uint8_t USART_Number)
{
	USART_TypeDef *USART = USART_LL_Driver___Get_USART(USART_Number);

	if (USART != NULL)
	{
		USART_LL_Driver___Enable_USART_Clock(USART_Number);
		USART_LL_Driver___Enable_GPIO(USART_Number);
		USART_LL_Driver___Enable_Interrupt(USART_Number);

		USART->CR1 |= USART_CR1_UE; 	// Enable USART
		USART->CR1 |= USART_CR1_TE; 	// Enable Transmitter
		USART->CR1 |= USART_CR1_RE; 	// Enable Receiver
		USART->CR1 |= USART_CR1_RXNEIE; // Enable RXNE interrupt
		USART->CR1 |= USART_CR1_TCIE; 	// Enable TC interrupt
	}
}

void USART_LL_Driver___Transfer_Out(uint8_t USART_Number, uint8_t data)
{
	USART_TypeDef *USART = USART_LL_Driver___Get_USART(USART_Number);
	USART->DR=data;
}

void USART_Interrupt_Handler(uint8_t USART_Number)
{
	USART_TypeDef *USART = USART_LL_Driver___Get_USART(USART_Number);

	while(USART -> SR & USART_INTERRUPTS_MSK)
	{
		switch (POSITION_VAL(USART -> SR & USART_INTERRUPTS_MSK))
		{
		case (USART_SR_RXNE_Pos):
			USART->SR &= ~(USART_SR_RXNE);

			uint8_t received_Byte = USART->DR;
			if (USART_LL_Driver___RX_Callback != NULL)
			{
				USART_LL_Driver___RX_Callback(USART_Number, received_Byte);
			}

			break;
		case (USART_SR_TC_Pos):
			USART->SR &= ~(USART_SR_TC);
			if (USART_LL_Driver___TX_Callback != NULL)
			{
				USART_LL_Driver___TX_Callback(USART_Number);
			}

			break;
		case (USART_SR_LBD_Pos):
			USART->SR &= ~(USART_SR_LBD);
			break;
		case (USART_SR_CTS_Pos):
			USART->SR &= ~(USART_SR_CTS);
			break;
		}
	}
}

void USART1_IRQHandler(void)
{
	USART_Interrupt_Handler(1);
}

void USART2_IRQHandler(void)
{

}

void USART3_IRQHandler(void)
{

}

void UART4_IRQHandler(void)
{

}

void UART5_IRQHandler(void)
{

}

void USART6_IRQHandler(void)
{

}
