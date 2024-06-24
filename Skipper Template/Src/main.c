
/*
 * Date 			[dd-mm-yyyy]
 * Author: 			[your name]
 * Project Name:  	[project name]
 *
 * Description		[description]
 */

#include <stdint.h>										// Include C library for fixed-width integer types
#include <stm32f4xx.h>									// include STM32F4 specific definitions

void GPIO_init(void){
	RCC->AHB1ENR 	|= (RCC_AHB1ENR_GPIODEN); 			// enable GPIOD clock

	//Enable on board LED
	GPIOD -> MODER 	|= (GPIO_MODER_MODER4_0); 	// set PD4 to output
	GPIOD -> ODR 	|= (GPIO_ODR_ODR_4);				// set PD4 HIGH
}

int main(void) {
	GPIO_init();										// initiate the GPIO to be used in this program.

	for(;;)
	{

	}
}

