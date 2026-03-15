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

#define DAC_CODE_ZEROSCALE	100
#define DAC_CODE_FULLSCALE	2482
#define DAC_SPAN		DAC_CODE_FULLSCALE - DAC_CODE_ZEROSCALE

#define V_FULLSCALE 	59.079f
#define V_ZEROSCALE 	29.791f
#define V_SPAN			(V_FULLSCALE - V_ZEROSCALE)

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

void boost___DAC_Init()
{

}

void boost___Select_CS()
{
	GPIOB->ODR &= ~GPIO_ODR_ODR11;					// Set boost CS
}

void boost___Deselect_CS()
{
	GPIOB->ODR |= GPIO_ODR_ODR11;					// Set boost CS
}

void boost___Enable()
{
	GPIOB->ODR |= GPIO_ODR_ODR2;					// Clear boost EN
}

void boost___Set_Voltage(float V_Set)
{
	if(V_Set <= 60.0 && V_Set >= 30.0)
	{
		uint16_t DAC_Code = (( (1-(V_Set - V_ZEROSCALE)/V_SPAN)) * (DAC_SPAN) + DAC_CODE_ZEROSCALE);
		boost___Select_CS();
		SPI___Transmit(DAC_Code << 2);
		//SPI___Transmit(DAC_CODE_FULLSCALE << 2);
		boost___Deselect_CS();
	}
}

void boost___Init()
{
	boost___GPIO_Init();
	SPI___Init();
	boost___Set_Voltage(40.0);
	boost___Enable();
}
