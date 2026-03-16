/*
 * CCS.c
 *
 *  Created on: Mar 15, 2026
 *      Author: jackh
 */

#include <stdint.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "CCS.h"
#include "SPI.h"

#define CCS___CMD_NOP 				0x00
#define CCS___CMD_WRITE				0x01
#define CCS___CMD_UPDATE			0x02
#define CCS___CMD_WRITE_AND_UPDATE	0x03
#define CCS___CMD_TOGGLE_POWER		0x04
#define CCS___CMD_LDAC_MASK			0x05
#define CCS___CMD_SW_RESET			0x06
#define CCS___CMD_SET_GAIN			0x07
#define CCS___CMD_SETUP_DCEN		0x08
#define CCS___CMD_SETUP_RB			0x09
#define CCS___CMD_UPDATE_ALL		0x0A

void CCS___GPIO_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	/* ---------- PB14 : NLDAC ---------- */
	GPIOB->CRH &= ~GPIO_CRH_MODE14_Msk;				// Clear mode
	GPIOB->CRH &= ~GPIO_CRH_CNF14_Msk;				// clear configuration
	GPIOB->ODR &= ~GPIO_ODR_ODR14;					// Set low
	GPIOB->CRH |= GPIO_CRH_MODE14_0;				// Set to output (slow)

	/* ---------- PB12 : NCS ---------- */
	GPIOB->CRH &= ~GPIO_CRH_MODE12_Msk;				// Clear mode
	GPIOB->CRH &= ~GPIO_CRH_CNF12_Msk;				// clear configuration
	GPIOB->ODR |= GPIO_ODR_ODR12;					// Set high
	GPIOB->CRH |= GPIO_CRH_MODE12_0;				// Set to output (slow)

	/* ---------- PA8 : DAC NRST ---------- */
	GPIOA->CRH &= ~GPIO_CRH_MODE8_Msk;				// Clear mode
	GPIOA->CRH &= ~GPIO_CRH_CNF8_Msk;				// clear configuration
	GPIOA->ODR &= ~GPIO_ODR_ODR8;					// clear low
	GPIOA->CRH |= GPIO_CRH_MODE8_0;					// Set to output (slow)
}

void CCS___Reset()
{
	GPIOA->ODR &= ~GPIO_ODR_ODR8;					// reset low
	clock___Delay_ms(10);
	GPIOA->ODR |= GPIO_ODR_ODR8;					// reset high
}

void CCS___Select_CS()
{
	GPIOB->ODR &= ~GPIO_ODR_ODR12;
}

void CCS___Deselect_CS()
{
	GPIOB->ODR |= GPIO_ODR_ODR12;
}

void CCS___Write_Channel(uint8_t channel, float C_Set)
{
	if((C_Set >= 0.0f) && (C_Set <= CCS___MAX_CURRENT))
	{
		CCS___Select_CS();

		uint8_t tx[3];
		uint16_t C_Code = (uint16_t)(((C_Set/2.5f)*4095.0f) + 0.5f);
		uint16_t packet = (C_Code << 4);
		tx[0] = (CCS___CMD_WRITE_AND_UPDATE << 4) | (channel & 0xf);
		tx[1] = ((packet & 0xff00) >> 8);
		tx[2] = (packet & 0xff);

		for(uint8_t i = 0; i < 3; i++)
		{
			SPI___Transmit(tx[i]);
		}

		CCS___Deselect_CS();
	}
}

void CCS___Init()
{
	CCS___GPIO_Init();
	CCS___Reset();

}
