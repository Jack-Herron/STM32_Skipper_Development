/*
 * ADC.c
 *
 *  Created on: Mar 19, 2026
 *      Author: Jack Herron
 */

#include <stdint.h>
#include "stm32f1xx.h"
#include "ADC.h"

volatile uint16_t 	ADC___Channel_Value[3];
const 	 uint8_t 	ADC___Channel[4] = {0,0,1,2};


void ADC___GPIO_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;   // GPIOA clock

	GPIOA->CRL &= ~(
		  GPIO_CRL_MODE3 | GPIO_CRL_CNF3
		| GPIO_CRL_MODE4 | GPIO_CRL_CNF4
		| GPIO_CRL_MODE5 | GPIO_CRL_CNF5
	);
}

void ADC___DMA_Init()
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    DMA1_Channel1->CCR &= ~DMA_CCR_EN;   // disable before config
    DMA1_Channel1->CPAR  = (uint32_t)&ADC1->DR;
    DMA1_Channel1->CMAR  = (uint32_t)ADC___Channel_Value;
    DMA1_Channel1->CNDTR = 3;

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

    ADC1->SQR1 = (2 << 20); // 3 channels in conversion

    ADC1->SQR3 =			// Regular sequence: CH0, CH1, CH2, CH3, CH4, CH5
          (3 << 0)
        | (4 << 5)
        | (5 << 10);

    ADC1->SQR2 = 0;

    ADC1->SMPR2 =										// sample times
          (ADC_SMPR2_SMP3_1 | ADC_SMPR2_SMP3_0)
        | (ADC_SMPR2_SMP4_1 | ADC_SMPR2_SMP4_0)
        | (ADC_SMPR2_SMP5_1 | ADC_SMPR2_SMP5_0);



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
	uint16_t code = ADC___Channel_Value[ADC___Channel[channel]];
	float voltage;

	voltage = (((float)code) / 4095) * 3.3f;

	return(voltage);
}
