/*
 * Clock.c
 *
 *  Created on: Dec 22, 2025
 *      Author: jackh
 */

#include "Clock.h"
#include "stm32f4xx.h"

// TODO implement selectors into C file (currently not supported)

uint64_t millis = 0;

void TIM2_Init()
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; 		// Enable TIM2 clock

	TIM2->PSC = 0; 													// Set prescaler for 1ms tick
	TIM2->ARR = (APB1TIMER_FREQ / 1000) - 1;                  		// Auto-reload value for 1ms
	TIM2->CNT = 0;                      							// Reset counter
	TIM2->DIER |= TIM_DIER_UIE;                						// Enable update interrupt
	NVIC_EnableIRQ(TIM2_IRQn);              						// Enable TIM2 interrupt in NVIC
	NVIC_SetPriority(TIM2_IRQn, 11);       							// Set TIM2 interrupt priority
	TIM2->CR1 |= TIM_CR1_CEN;          								// Enable TIM2
}

void millis_Init(void)
{
	millis = 0;
	TIM2_Init();
}

uint32_t clock___millis(void)
{
	return((uint32_t)millis);
}

void clock___delay_ms(uint32_t delay)
{
	uint32_t start = clock___millis();
	while ((clock___millis() - start) < delay);
}

void clock_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_PWREN; // Enable Power interface clock

	FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS; // Enable instruction cache, data cache, prefetch and set latency to 5 wait states

	RCC->PLLCFGR = 		(PLLR				<< RCC_PLLCFGR_PLLR_Pos) | 			// PLLR = 6
						(PLLM 				<< RCC_PLLCFGR_PLLM_Pos) | 			// PLLM = 4
						(PLLN 				<< RCC_PLLCFGR_PLLN_Pos) | 			// PLLN = 180
						(((PLLP/2)-1)		<< RCC_PLLCFGR_PLLP_Pos) | 			// PLLP = 2
						(PLLSRC_SELECTOR	<< RCC_PLLCFGR_PLLSRC_Pos) | 		// HSE as PLL source
						(PLLQ 				<< RCC_PLLCFGR_PLLQ_Pos);     		// PLLQ = 6

	RCC->PLLI2SCFGR = 	(PLLI2SN 			<< RCC_PLLI2SCFGR_PLLI2SN_Pos) | 	// I2SN = 192
						(PLLI2SQ1 			<< RCC_PLLI2SCFGR_PLLI2SQ_Pos) |	// I2SQ = 4
						(PLLI2SR 			<< RCC_PLLI2SCFGR_PLLI2SR_Pos); 	// I2SR = 2

	RCC->PLLSAICFGR = 	(PLLSAIN 			<< RCC_PLLSAICFGR_PLLSAIN_Pos) | 	// SAIN = 192
						(((PLLSAIP/2)-1)	<< RCC_PLLSAICFGR_PLLSAIP_Pos) | 	// SAIP = 4
						(PLLSAIQ1 			<< RCC_PLLSAICFGR_PLLSAIQ_Pos) |	// SAIQ = 2
						(PLLSAIR1 			<< RCC_PLLSAICFGR_PLLSAIR_Pos); 	// SAIR = 2

	RCC->DCKCFGR = 		((PLLI2SQ2-1) 		<< RCC_DCKCFGR_PLLI2SDIVQ_Pos) |
			            ((PLLSAIQ2-1)		<< RCC_DCKCFGR_PLLSAIDIVQ_Pos) |
						((PLLSAIR2_CODE)	<< RCC_DCKCFGR_PLLSAIDIVR_Pos);

	RCC->CFGR = 		(PPRE1_CODE 		<< RCC_CFGR_PPRE1_Pos) |  			// APB1 prescaler
						(PPRE2_CODE 		<< RCC_CFGR_PPRE2_Pos) |   			// APB2 prescaler
						(HPRE_CODE 			<< RCC_CFGR_HPRE_Pos);      		// AHB prescaler

	RCC->CR |= 			RCC_CR_HSEON; 						// Enable HSE
	while(!(RCC->CR & RCC_CR_HSERDY)); 						// Wait for HSE to be ready
	PWR->CR |= PWR_CR_ODEN; 								// Enable OverDrive mode

	RCC->CR |= 			RCC_CR_PLLON; 						// Enable PLL
	while(!(RCC->CR & RCC_CR_PLLRDY)); 						// Wait for PLL to be ready

	RCC->CFGR |= 		RCC_CFGR_SW_1; 						// Select PLL as system clock
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1); 	// Wait for PLL to be used as system clock

	RCC->CR &= ~RCC_CR_HSION; 								// Disable HSI
	while((RCC->CR & RCC_CR_HSIRDY)); 						// Wait for HSI to be disabled

	RCC->CR |= RCC_CR_PLLSAION; 							// Enable PLLSAI
	while(!(RCC->CR & RCC_CR_PLLSAIRDY)); 					// Wait for PLLSAI to be ready
}

void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_UIF) 				// Check if update interrupt flag is set
	{
		millis++; 							// Increment milliseconds counter
		TIM2->SR &= ~TIM_SR_UIF; 			// Clear update interrupt flag
	}
}
