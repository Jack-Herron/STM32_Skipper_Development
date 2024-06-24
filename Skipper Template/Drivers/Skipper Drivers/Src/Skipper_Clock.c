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

uint8_t Skipper_Clock___Get_APB_Value_From_Divisor(uint8_t Divisor)
{
	uint8_t APB_Value;

	switch(Divisor)
	{
		case Skipper_Clock___APB_DIV_1:
			APB_Value = Skipper_Clock___APB_DIV_1_Value;
			break;

		case Skipper_Clock___APB_DIV_2:
			APB_Value = Skipper_Clock___APB_DIV_2_Value;
			break;

		case Skipper_Clock___APB_DIV_4:
			APB_Value = Skipper_Clock___APB_DIV_4_Value;
			break;

		case Skipper_Clock___APB_DIV_8:
			APB_Value = Skipper_Clock___APB_DIV_8_Value;
			break;

		case Skipper_Clock___APB_DIV_16:
			APB_Value = Skipper_Clock___APB_DIV_16_Value;
			break;

		default:
			assert(1);													// APB value invalid
			break;
	}

	return(APB_Value);
}

uint8_t Skipper_Clock___Get_AHB_Value_From_Divisor(uint16_t Divisor)
{
	uint8_t AHB_Value;

	switch(Divisor)
	{
		case Skipper_Clock___AHB_DIV_1:
			AHB_Value = Skipper_Clock___AHB_DIV_1_VALUE;
			break;

		case Skipper_Clock___AHB_DIV_2:
			AHB_Value = Skipper_Clock___AHB_DIV_2_VALUE;
			break;

		case Skipper_Clock___AHB_DIV_4:
			AHB_Value = Skipper_Clock___AHB_DIV_4_VALUE;
			break;

		case Skipper_Clock___AHB_DIV_8:
			AHB_Value = Skipper_Clock___AHB_DIV_8_VALUE;
			break;

		case Skipper_Clock___AHB_DIV_16:
			AHB_Value = Skipper_Clock___AHB_DIV_16_VALUE;
			break;

		case Skipper_Clock___AHB_DIV_64:
			AHB_Value = Skipper_Clock___AHB_DIV_64_VALUE;
			break;

		case Skipper_Clock___AHB_DIV_128:
			AHB_Value = Skipper_Clock___AHB_DIV_128_VALUE;
			break;

		case Skipper_Clock___AHB_DIV_256:
			AHB_Value = Skipper_Clock___AHB_DIV_256_VALUE;
			break;

		case Skipper_Clock___AHB_DIV_512:
			AHB_Value = Skipper_Clock___AHB_DIV_512_VALUE;
			break;

		default:
			assert(1);													// AHB value invalid
			break;
	}

	return(AHB_Value);
}

