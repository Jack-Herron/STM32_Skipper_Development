
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
#include <Skipper_Clock.h>								// Include Skipper clock configuration
#include <USART.h>

void GPIO_init(void)
{
	RCC->AHB1ENR 	|= (RCC_AHB1ENR_GPIODEN); 			// Enable GPIOD clock
	RCC->AHB1ENR 	|= (RCC_AHB1ENR_GPIOAEN); 			// Enable GPIOA clock

	//Enable on board LED
	GPIOD -> MODER 	|= (GPIO_MODER_MODER4_0); 			// Set PD4 to output
	GPIOD -> ODR 	|= (GPIO_ODR_ODR_4);				// Set PD4 HIGH

	GPIOA -> MODER 	|= (GPIO_MODER_MODER4_0); 			// Set PA4 to output
	GPIOA -> ODR 	|= (GPIO_ODR_ODR_4);				// Set PA4 HIGH
}

int main(void)
{
	Skipper_Clock___Init();
	Skipper_Clock___Systick_Init();
	GPIO_init();										// Initiate the GPIO's to be used in this program
	for(;;)
	{

		Skipper_Clock___Delay_ms(5);
		GPIOA -> ODR 	|= (GPIO_ODR_ODR_4);				// Set PA4 HIGH
		Skipper_Clock___Delay_ms(5);
		GPIOA -> ODR 	&= ~(GPIO_ODR_ODR_4);				// Set PA4 HIGH
	}

	return(EXIT_SUCCESS);
}
