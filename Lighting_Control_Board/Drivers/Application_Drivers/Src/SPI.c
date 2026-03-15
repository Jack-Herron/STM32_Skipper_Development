/*
 * SPI.c
 *
 *  Created on: Mar 14, 2026
 *      Author: jackh
 */

#include <stdint.h>
#include "stm32f1xx.h"
#include "SPI.h"

void SPI___GPIO_Init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    /* ---------- PB13 : SCLK ---------- */
    GPIOB->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOB->CRH |=  (GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1); // 50 MHz
    GPIOB->CRH |=  GPIO_CRH_CNF13_1;                        // AF Push-Pull

    /* ---------- PB15 : MOSI ---------- */
    GPIOB->CRH &= ~(GPIO_CRH_MODE15 | GPIO_CRH_CNF15);
    GPIOB->CRH |=  (GPIO_CRH_MODE15_0 | GPIO_CRH_MODE15_1); // 50 MHz
    GPIOB->CRH |=  GPIO_CRH_CNF15_1;                        // AF Push-Pull
}

void SPI___Init()
{
	SPI___GPIO_Init();

	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

	SPI2->CR1 = 0;
	SPI2->CR1 |= SPI_CR1_DFF;		// 16 bit format
	SPI2->CR1 |= SPI_CR1_MSTR;		// master mode
	SPI2->CR1 &= ~SPI_CR1_CPOL;		// clock low when idle
	SPI2->CR1 |= SPI_CR1_CPHA;		// sample on falling edge

	SPI2->CR1 |= SPI_CR1_SPE;		// enable SPI

}

void SPI___Transmit(uint16_t data)
{
	while (!(SPI2->SR & SPI_SR_TXE));
	SPI2->DR = data;
	while (!(SPI2->SR & SPI_SR_TXE));
	while (SPI2->SR & SPI_SR_BSY);
}
