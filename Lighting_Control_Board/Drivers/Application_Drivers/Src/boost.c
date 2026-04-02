/*
 * boost.c
 *
 *  Created on: Mar 14, 2026
 *      Author: jackh
 */

#include <stdint.h>
#include "stm32f1xx.h"
#include "boost.h"
#include "SPI.h"

void boost___GPIO_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;				// Enable GPIOB clock

	GPIOB->CRL &= ~GPIO_CRL_MODE2_Msk;				// Clear PB2 mode
	GPIOB->CRL &= ~GPIO_CRL_CNF2_Msk;				// clear PB2 configuration

	GPIOB->ODR &= ~GPIO_ODR_ODR2;					// Clear boost EN

	GPIOB->CRL |= GPIO_CRL_MODE2_0;					// Set PB2 to output (slow) for boost en

	GPIOB->CRH &= ~GPIO_CRH_MODE11_Msk;				// Clear PB11 mode
	GPIOB->CRH &= ~GPIO_CRH_CNF11_Msk;				// clear PB11 configuration

	GPIOB->ODR |= GPIO_ODR_ODR11;					// Set boost CS

	GPIOB->CRH |= GPIO_CRH_MODE11_0;				// Set PB11 to output (slow) for boost CS
}

void boost___Select_CS()
{
	GPIOB->ODR &= ~GPIO_ODR_ODR11;
}

void boost___Deselect_CS()
{
	GPIOB->ODR |= GPIO_ODR_ODR11;
}

void boost___Enable()
{
	GPIOB->ODR |= GPIO_ODR_ODR2;					// Clear boost EN
}

void boost___Set_Voltage(float V_Set)
{
	if (V_Set <= 60.0f && V_Set >= 30.0f)
	{
		uint16_t DAC_Code = (uint16_t)(((1.0f - (V_Set - V_ZEROSCALE) / V_SPAN) * DAC_SPAN) + DAC_CODE_ZEROSCALE);
		uint16_t transmission = (uint16_t)(DAC_Code << 2);

		uint8_t tx_hi = (uint8_t)((transmission >> 8) & 0xFF);
		uint8_t tx_lo = (uint8_t)(transmission & 0xFF);

		__disable_irq();

		boost___Select_CS();

		SPI___Transmit(tx_hi);
		SPI___Transmit(tx_lo);

		while (SPI2->SR & SPI_SR_BSY);

		boost___Deselect_CS();

		__enable_irq();
	}
}

void boost___Init()
{
	boost___GPIO_Init();
	SPI___Init();
	boost___Set_Voltage(40.0);

}
