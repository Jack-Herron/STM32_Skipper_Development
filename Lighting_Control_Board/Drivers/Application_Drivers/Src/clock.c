/*
 * clock.c
 *
 *  Created on: Mar 14, 2026
 *      Author: jackh
 */

#include <stdint.h>
#include "stm32f1xx.h"
#include "clock.h"

volatile uint32_t clock___Millis_Count = 0;

void clock___Setup(){																								// initialize clock
	RCC -> CR |= RCC_CR_HSEON;																						// enable HSE Oscillator
	while(!(RCC -> CR & RCC_CR_HSERDY));																			// wait for HSE To Stabilize
	FLASH -> ACR |= FLASH_ACR_LATENCY_2;																			// set flash wait to 2, because 48 MHz < SYSCLK  72 MHz
	RCC -> CFGR &= ~(RCC_CFGR_USBPRE);																				// set USB Pre-scaler to /1.5
	RCC -> CFGR |=
					(RCC_CFGR_PLLMULL9		) |																		// set PLL Multiplication Factor to *9
					(RCC_CFGR_PLLSRC		) |																		// select HSE as the PLL Source
					(RCC_CFGR_ADCPRE_DIV6	) |																		// set ADC Pre-scaler to /6
					(RCC_CFGR_PPRE1_DIV2	);																		// set APB1 Pre-Scaler to /2
	RCC -> CR |= (RCC_CR_PLLON);																					// enable PLL
	while(!(RCC -> CR & RCC_CR_PLLRDY));																			// wait for PLL To Stabilize
	RCC -> CFGR |= (RCC_CFGR_SW_PLL);																				// switch PLL on
	while(!(RCC -> CFGR & RCC_CFGR_SWS_PLL));																		// Wait for source to switch to PLL
	RCC -> CR &= ~(1<<0);																							// Turn off HSI
}

void clock___TIM2_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   // Enable TIM2 clock

    TIM2->CR1 = 0;                        // Upcounting, edge-aligned, timer disabled for setup
    TIM2->PSC = 71;                       // 72 MHz / (71 + 1) = 1 MHz
    TIM2->ARR = 999;                      // 1 MHz / (999 + 1) = 1 kHz = 1 ms
    TIM2->CNT = 0;                        // Clear counter

    TIM2->DIER |= TIM_DIER_UIE;           // Enable update interrupt
    TIM2->EGR |= TIM_EGR_UG;              // Force update to load PSC immediately

    NVIC_EnableIRQ(TIM2_IRQn);            // Enable TIM2 interrupt in NVIC

    TIM2->CR1 |= TIM_CR1_CEN;             // Start timer
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;          // Clear update interrupt flag
        clock___Millis_Count++;
    }
}

void clock___Delay_ms(uint32_t delay)
{
	if(delay > 0)
	{
		uint32_t current_Millis = clock___Millis_Count;
		while(clock___Millis_Count != current_Millis + delay);
	}
}

void clock___Init()
{
	clock___Setup();
	clock___TIM2_Init();
}
