/*
 * Skipper_Clock.c
 *
 *  Created on: Jun 23, 2024
 *      Author: Jack Herron
 */

#include <stdlib.h>  									// Include C standard library
#include <stdint.h>										// Include C library for fixed-width integer types
#include <stm32f4xx.h>									// Include STM32F4 specific definitions
#include <assert.h>										// Include assert
#include "../Inc/Skipper_Clock.h"

uint8_t Skipper_Clock___Get_RCC_CFGR_APB_Value_From_Prescaler_Value(uint8_t Prescaler_Value)
{
	uint8_t APB_Value = 0;

	switch(Prescaler_Value)
	{
		case Skipper_Clock___APB_DIV_1:
			APB_Value = Skipper_Clock___RCC_CFGR_APB_DIV_1;
			break;

		case Skipper_Clock___APB_DIV_2:
			APB_Value = Skipper_Clock___RCC_CFGR_APB_DIV_2;
			break;

		case Skipper_Clock___APB_DIV_4:
			APB_Value = Skipper_Clock___RCC_CFGR_APB_DIV_4;
			break;

		case Skipper_Clock___APB_DIV_8:
			APB_Value = Skipper_Clock___RCC_CFGR_APB_DIV_8;
			break;

		case Skipper_Clock___APB_DIV_16:
			APB_Value = Skipper_Clock___RCC_CFGR_APB_DIV_16;
			break;
	}

	return(APB_Value);
}

uint8_t Skipper_Clock___Get_RCC_CFGR_AHB_Value_From_Prescaler_Value(uint16_t Prescaler_Value)
{
	uint8_t AHB_Value = 0;

	switch(Prescaler_Value)
	{
		case Skipper_Clock___AHB_DIV_1:
			AHB_Value = Skipper_Clock___RCC_CFGR_AHB_DIV_1;
			break;

		case Skipper_Clock___AHB_DIV_2:
			AHB_Value = Skipper_Clock___RCC_CFGR_AHB_DIV_2;
			break;

		case Skipper_Clock___AHB_DIV_4:
			AHB_Value = Skipper_Clock___RCC_CFGR_AHB_DIV_4;
			break;

		case Skipper_Clock___AHB_DIV_8:
			AHB_Value = Skipper_Clock___RCC_CFGR_AHB_DIV_8;
			break;

		case Skipper_Clock___AHB_DIV_16:
			AHB_Value = Skipper_Clock___RCC_CFGR_AHB_DIV_16;
			break;

		case Skipper_Clock___AHB_DIV_64:
			AHB_Value = Skipper_Clock___RCC_CFGR_AHB_DIV_64;
			break;

		case Skipper_Clock___AHB_DIV_128:
			AHB_Value = Skipper_Clock___RCC_CFGR_AHB_DIV_128;
			break;

		case Skipper_Clock___AHB_DIV_256:
			AHB_Value = Skipper_Clock___RCC_CFGR_AHB_DIV_256;
			break;

		case Skipper_Clock___AHB_DIV_512:
			AHB_Value = Skipper_Clock___RCC_CFGR_AHB_DIV_512;
			break;
	}

	return(AHB_Value);
}

void Skipper_Clock___Turn_On_HSE()
{
	RCC -> CR 		|= 	(RCC_CR_HSEON);
	while(!(RCC -> CR & RCC_CR_HSERDY));
}

void Skipper_Clock___Turn_On_PLL()
{
	RCC -> CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
}

void Skipper_Clock___Turn_Off_HSI()
{
	RCC -> CR &= ~(RCC_CR_HSION);
	while(RCC -> CR & RCC_CR_HSIRDY);
}

void Skipper_Clock___Set_MUX_Source(uint8_t MUX_Source)
{
	if(MUX_Source == Skipper_Clock___MUX_USE_PLL)
	{
		RCC -> CFGR |= (RCC_CFGR_SW_PLL);
		while(!(RCC -> CFGR & RCC_CFGR_SWS_PLL));
	}
	else if(MUX_Source == Skipper_Clock___MUX_USE_HSE)
	{
		RCC -> CFGR |= (RCC_CFGR_SW_HSE);
		while(!(RCC -> CFGR & RCC_CFGR_SWS_HSE));
	}
	else if(MUX_Source == Skipper_Clock___MUX_USE_HSI)
	{
		RCC -> CFGR |= (RCC_CFGR_SW_HSI);
		while(!(RCC -> CFGR & RCC_CFGR_SWS_HSI));
	}
}

void Skipper_Clock___Init()
{
	if((Skipper_Clock___MUX_SOURCE == Skipper_Clock___MUX_USE_HSE) || (Skipper_Clock___PLL_SOURCE == Skipper_Clock___PLL_USE_HSE))
	{
		Skipper_Clock___Turn_On_HSE();
	}

	PWR -> CR |= PWR_CR_VOS;
	FLASH -> ACR |= (FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS);

	RCC->PLLCFGR = 0;
	RCC -> PLLCFGR |= 	(Skipper_Clock___PLL_Q 				<< RCC_PLLCFGR_PLLQ_Pos) 	|
						(((Skipper_Clock___PLL_P - 2) / 2) 	<< RCC_PLLCFGR_PLLP_Pos) 	|
						(Skipper_Clock___PLL_N 				<< RCC_PLLCFGR_PLLN_Pos)		|
						(Skipper_Clock___PLL_M 				<< RCC_PLLCFGR_PLLM_Pos)		;

	if(Skipper_Clock___PLL_SOURCE == Skipper_Clock___PLL_USE_HSE)
	{
		RCC -> PLLCFGR 	|= 	(RCC_PLLCFGR_PLLSRC_HSE);
	}
	else
	{
		RCC -> PLLCFGR 	&= 	~(RCC_PLLCFGR_PLLSRC_HSE);
	}

	uint8_t RCC_CFGR_AHB_Value = Skipper_Clock___Get_RCC_CFGR_AHB_Value_From_Prescaler_Value(Skipper_Clock___AHB_PRESCALER);
	uint8_t RCC_CFGR_APB1_Value = Skipper_Clock___Get_RCC_CFGR_APB_Value_From_Prescaler_Value(Skipper_Clock___APB1_PRESCALER);
	uint8_t RCC_CFGR_APB2_Value = Skipper_Clock___Get_RCC_CFGR_APB_Value_From_Prescaler_Value(Skipper_Clock___APB2_PRESCALER);

	RCC -> CFGR |= 		(RCC_CFGR_AHB_Value  << RCC_CFGR_HPRE_Pos) 	|
						(RCC_CFGR_APB1_Value << RCC_CFGR_PPRE1_Pos) |
						(RCC_CFGR_APB2_Value << RCC_CFGR_PPRE2_Pos)	;

	Skipper_Clock___Turn_On_PLL();

	Skipper_Clock___Set_MUX_Source(Skipper_Clock___MUX_SOURCE);

	if((Skipper_Clock___MUX_SOURCE != Skipper_Clock___MUX_USE_HSI) && (Skipper_Clock___PLL_SOURCE != Skipper_Clock___PLL_USE_HSI))
	{
		Skipper_Clock___Turn_Off_HSI();
	}
}

void Skipper_Clock___Systick_Init()
{
	SysTick -> CTRL |= (Skipper_Clock___SYSTICK_ENABLE_INTERRUPT);

	if(Skipper_Clock___ENABLE_SYSTICK_PRESCALER == 0)
	{
		SysTick -> CTRL |= (Skipper_Clock___SYSTICK_CLOKC_SOURCE_SYSTEM_CLOCK);
	}
	else
	{
		SysTick -> CTRL &= ~(Skipper_Clock___SYSTICK_CLOKC_SOURCE_SYSTEM_CLOCK);
	}

	SysTick -> LOAD = ((Skipper_Clock___CORTEX_SYSTEM_TIMER_FREQUENCY / 2) - 1);
	SysTick -> CTRL |= (Skipper_Clock___SYSTICK_ENABLE);
}

void SysTick_Handler()
{

	if(GPIOD -> ODR & (GPIO_ODR_ODR_4))
	{
		GPIOD -> ODR 	&= ~(GPIO_ODR_ODR_4);				// Set PD4 HIGH
	}
	else
	{
		GPIOD -> ODR 	|= (GPIO_ODR_ODR_4);				// Set PD4 HIGH
	}

}
