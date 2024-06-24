
/*
 * Date 			[dd-mm-yyyy]
 * Author: 			[your name]
 * Project Name:  	[project name]
 *
 * Description		[description]
 */

#include <stdlib.h>  									// Include C standard library
#include <stdint.h>										// Include C library for fixed-width integer types
#include <stm32f4xx.h>									// Include STM32F4 specific definitions

void GPIO_init(void)
{
	RCC->AHB1ENR 	|= (RCC_AHB1ENR_GPIODEN); 			// Enable GPIOD clock

	//Enable on board LED
	GPIOD -> MODER 	|= (GPIO_MODER_MODER4_0); 			// Set PD4 to output
	GPIOD -> ODR 	|= (GPIO_ODR_ODR_4);				// Set PD4 HIGH
}

int main(void)
{
	GPIO_init();										// Initiate the GPIO's to be used in this program

	for(;;)
	{

	}

	return(EXIT_SUCCESS);
}
