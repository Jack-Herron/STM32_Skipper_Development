/*
 * QSPI.c
 *
 *  Created on: Jan 3, 2026
 *      Author: jackh
 */

#include "stm32f4xx.h"
#include "Clock.h"
#include "QSPI.h"

void QSPI___Pin_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t function)
{
	GPIOx->MODER &= ~(3 << (GPIO_Pin * 2));							// clear mode bits
	GPIOx->AFR[GPIO_Pin/8] &= ~(0xF << ((GPIO_Pin & 0x07) * 4));	// clear alternate function bits
	GPIOx->OSPEEDR &= ~(3 << (GPIO_Pin * 2));						// clear speed bits
	GPIOx->PUPDR &= ~(3 << (GPIO_Pin * 2));							// clear pull-up/pull-down bits

	GPIOx->AFR[GPIO_Pin/8] |= (function << ((GPIO_Pin & 0x07) * 4));		// set alternate function #
	GPIOx->MODER |= (2 << (GPIO_Pin * 2));							// set mode to alternate function
	GPIOx->OSPEEDR |= (3 << (GPIO_Pin * 2));						// set speed to high
}

void QSPI___GPIO_Init()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN; // Enable GPIOF clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable GPIOB clock

	QSPI___Pin_Init(GPIOF, 6, 9);		// PF6 -> AF9
	QSPI___Pin_Init(GPIOF, 7, 9);		// PF7 -> AF9
	QSPI___Pin_Init(GPIOF, 8, 10);		// PF8 -> AF10
	QSPI___Pin_Init(GPIOF, 9, 10);		// PF9 -> AF10
	QSPI___Pin_Init(GPIOF, 10, 9);		// PF10 -> AF9
	QSPI___Pin_Init(GPIOB, 6, 10);		// PB6 -> AF10
}


void QSPI___Init()
{
	QSPI___GPIO_Init();
	RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN;

	QUADSPI->CR = 0;		// reset CR
	QUADSPI->DCR = 4;		// reset DCR (reset value = 4)
	QUADSPI->CR |= QSPI___PRESCALER_VALUE << QUADSPI_CR_PRESCALER_Pos;
	QUADSPI->DCR |= QSPI___FSIZE << QUADSPI_DCR_FSIZE_Pos;
	QUADSPI->DCR |= QSPI___CSHT << QUADSPI_DCR_CSHT_Pos;



	QUADSPI->CR |= QUADSPI_CR_EN;


}
