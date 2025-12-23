/*
 * Clock.c
 *
 *  Created on: Dec 22, 2025
 *      Author: jackh
 */

#include "Clock.h"
#include "stm32f4xx.h"


void clock_Init(void)
{

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable GPIOA clock
	GPIOA->MODER |= GPIO_MODER_MODER8_1; // Set PA8 to AF mode
	RCC->CFGR |= RCC_CFGR_MCO1_1 ; // Select PLL as MCO1 source

	RCC->PLLCFGR = 		(PLLR				<< RCC_PLLCFGR_PLLR_Pos) | 			// PLLR = 6
						(PLLM 				<< RCC_PLLCFGR_PLLM_Pos) | 			// PLLM = 4
						(PLLN 				<< RCC_PLLCFGR_PLLN_Pos) | 			// PLLN = 180
						(((PLLP/2)-1)		<< RCC_PLLCFGR_PLLP_Pos) | 			// PLLP = 2
						(PLLSRC_SELECTOR	<< RCC_PLLCFGR_PLLSRC_Pos) | 							// HSE as PLL source
						(PLLQ 				<< RCC_PLLCFGR_PLLQ_Pos);     		// PLLQ = 6

	RCC->PLLI2SCFGR = 	(PLLI2SN 			<< RCC_PLLI2SCFGR_PLLI2SN_Pos) | 	// I2SN = 192
						(PLLI2SQ1 			<< RCC_PLLI2SCFGR_PLLI2SQ_Pos) |
						(PLLI2SR 			<< RCC_PLLI2SCFGR_PLLI2SR_Pos); 	// I2SP = 4

	RCC->PLLSAICFGR = 	(PLLSAIN 			<< RCC_PLLSAICFGR_PLLSAIN_Pos) | 	// SAIN = 192
						(((PLLSAIP/2)-1)	<< RCC_PLLSAICFGR_PLLSAIP_Pos) | 	// SAIP = 4
						(PLLSAIQ1 			<< RCC_PLLSAICFGR_PLLSAIQ_Pos) |
						(PLLSAIR1 			<< RCC_PLLSAICFGR_PLLSAIR_Pos); 	// SAIR = 4

	RCC->DCKCFGR = 		((PLLI2SQ2-1) 		<< RCC_DCKCFGR_PLLI2SDIVQ_Pos) |
			            ((PLLSAIQ2-1)		<< RCC_DCKCFGR_PLLSAIDIVQ_Pos) |
						((PLLSAIR2-1)		<< RCC_DCKCFGR_PLLSAIDIVR_Pos);

	RCC->CFGR = 		(PPRE1_CODE 		<< RCC_CFGR_PPRE1_Pos) |  			// APB1 prescaler
						(PPRE2_CODE 		<< RCC_CFGR_PPRE2_Pos) |   			// APB2 prescaler
						(HPRE_CODE 			<< RCC_CFGR_HPRE_Pos);      		// AHB prescaler

	RCC->CR |= 			RCC_CR_HSEON; 	// Enable HSE
	while(!(RCC->CR & RCC_CR_HSERDY)); 	// Wait for HSE to be ready
	for(uint32_t i = 0; i < 1000; i++); // Small delay)
	RCC->CR |= 			RCC_CR_PLLON; 	// Enable PLL
	while(!(RCC->CR & RCC_CR_PLLRDY)); 	// Wait for PLL to be ready
	for(uint32_t i = 0; i < 1000; i++); // Small delay)
	RCC->CFGR |= 		RCC_CFGR_SW_1; 	// Select PLL as system clock
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1); // Wait for PLL to be used as system clock
	for(uint32_t i = 0; i < 1000; i++); // Small delay)
	RCC->CR &= 		    ~RCC_CR_HSION;
	while(RCC->CR & RCC_CR_HSIRDY); 		// Wait for HSI to be disabled
	RCC->CR &= 		    ~RCC_CR_HSION;
}
