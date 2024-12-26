
/*
 * date [date]
 * Project name - [your project name]
 * Created by: [your name]
 *
 * description[description]
 */

#include <stdint.h>					// Include C library for fixed-width integer types
#include <stm32f4xx.h>				// include MCU specific definitions
#include <Skipper_Clock.h>			// Include Skipper clock configuration
#include <USB_Host.h>
#include <USB_VICE_Host.h>
#include <USART.h>
#include <USB_CDC_Device.h>

void GPIO_init(void){
	RCC->AHB1ENR |= (1<<3); 		// enable GPIOD clock
	RCC->AHB1ENR |= (1<<2); 		// enable GPIOC clock
	RCC->AHB1ENR |= (1<<0); 		// enable GPIOA clock
	//Enable on board LED
	GPIOD->MODER |= (1<<8); 		// set PD4 to output
	GPIOD->ODR &= ~(1<<4);			// set PD4 HIGH

	GPIOA->MODER |= (1<<18);
	GPIOA->ODR |= (1<<9);

	GPIOA->MODER |= (1<<8); 		// set PA4 to output
	GPIOA->ODR &= ~(1<<4);			// set PA4 HIGH

	GPIOA->MODER |= (1<<6); 		// set PA3 to output
	GPIOA->ODR &= ~(1<<3);			// set PA3 HIGH

	GPIOA->MODER |= (1<<4); 		// set PA2 to output
	GPIOA->ODR &= ~(1<<2);			// set PA2 HIGH

	GPIOC->MODER |= (1<<0); 		// set PA2 to output
	GPIOC->ODR &= ~(1<<0);			// set PA2 LOW

}

void USB_Device_Connected_Callback(uint8_t port_Number, uint8_t device_Address)
{
	uint8_t i = 0;
}

int main(void) {
	Skipper_Clock___Init();			// initiate the clock
	Skipper_Clock___Systick_Init();
	GPIO_init();					//initiate the GPIO to be used in this program.
	USART___Init(1);
	USART___Set_Baud_Rate(1, 921600);
	USB_Host___Init(0);
	//USB_CDC_Device___Init(1);
	USB_Host___Set_Device_Connected_Callback(0, USB_Device_Connected_Callback);
	USB_VICE_Host___Setup_Device(0,0);
	for(;;)
	{
		USB_Host___Process(0);
	}
}

