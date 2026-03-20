/*
 * ADC.c
 *
 *  Created on: Mar 19, 2026
 *      Author: Jack Herron
 */

#include <stdint.h>
#include "stm32f1xx.h"
#include "ADC.h"

volatile uint16_t ADC_Channel[9];

void ADC___GPIO_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // GPIOA clock
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;   // Enable GPIOB clock

	GPIOA->CRL &= ~(
		  GPIO_CRL_MODE0 | GPIO_CRL_CNF0
		| GPIO_CRL_MODE1 | GPIO_CRL_CNF1
		| GPIO_CRL_MODE2 | GPIO_CRL_CNF2
		| GPIO_CRL_MODE3 | GPIO_CRL_CNF3
		| GPIO_CRL_MODE4 | GPIO_CRL_CNF4
		| GPIO_CRL_MODE5 | GPIO_CRL_CNF5
		| GPIO_CRL_MODE6 | GPIO_CRL_CNF6
		| GPIO_CRL_MODE7 | GPIO_CRL_CNF7
	);

	GPIOB->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
}

void ADC___DMA_Init()
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    DMA1_Channel1->CCR &= ~DMA_CCR_EN;   // disable before config
    DMA1_Channel1->CPAR  = (uint32_t)&ADC1->DR;
    DMA1_Channel1->CMAR  = (uint32_t)ADC_Channel;
    DMA1_Channel1->CNDTR = 9;

    DMA1_Channel1->CCR =
          DMA_CCR_MINC      // increment memory
        | DMA_CCR_MSIZE_0   // memory size = 16-bit
        | DMA_CCR_PSIZE_0   // peripheral size = 16-bit
        | DMA_CCR_CIRC;     // circular mode

    DMA1_Channel1->CCR |= DMA_CCR_EN;
}

void ADC___Init(void)
{
	ADC___GPIO_Init();

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;   // ADC1 clock

    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;

    ADC1->CR1 = ADC_CR1_SCAN; // scan mode

    ADC1->CR2 = ADC_CR2_CONT | ADC_CR2_DMA | ADC_CR2_EXTSEL;				// enable DMA mode, continuous

    ADC1->SQR1 = (8 << 20); // 9 channels in conversion

    ADC1->SQR3 =			// Regular sequence: CH0, CH1, CH2, CH3, CH4, CH5
          (0 << 0)
        | (1 << 5)
        | (2 << 10)
        | (3 << 15)
        | (4 << 20)
        | (5 << 25);

    ADC1->SQR2 =			// Regular sequence continued: CH6, CH7, CH8
          (6 << 0)
        | (7 << 5)
		| (8 << 10);

    ADC1->SMPR2 =										// sample times
          (ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_0)
        | (ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0)
        | (ADC_SMPR2_SMP2_1 | ADC_SMPR2_SMP2_0)
        | (ADC_SMPR2_SMP3_1 | ADC_SMPR2_SMP3_0)
        | (ADC_SMPR2_SMP4_1 | ADC_SMPR2_SMP4_0)
        | (ADC_SMPR2_SMP5_1 | ADC_SMPR2_SMP5_0)
        | (ADC_SMPR2_SMP6_1 | ADC_SMPR2_SMP6_0)
        | (ADC_SMPR2_SMP7_1 | ADC_SMPR2_SMP7_0)
		| (ADC_SMPR2_SMP8_1 | ADC_SMPR2_SMP8_0);



    ADC1->CR2 |= ADC_CR2_ADON;           // power on ADC
    for(volatile int i = 0; i < 1000; i++);

    ADC1->CR2 |= ADC_CR2_RSTCAL;         // reset calibration
    while(ADC1->CR2 & ADC_CR2_RSTCAL);

    ADC1->CR2 |= ADC_CR2_CAL;            // start calibration
    while(ADC1->CR2 & ADC_CR2_CAL);

    ADC___DMA_Init();

    ADC1->CR2 |= ADC_CR2_EXTTRIG;
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

float ADC___Get_Voltage(uint8_t channel)
{
	uint16_t code = ADC_Channel[channel];
	float voltage;

	if(channel != 8)
	{
		voltage = (((float)code) / 4095) * 3.333f * 3.3f;
	}
	else
	{
		voltage = (((float)code) / 4095)*31.060241f * 3.3f;
	}
	return(voltage);
}
