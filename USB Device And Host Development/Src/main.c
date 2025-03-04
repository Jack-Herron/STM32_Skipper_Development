
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
#include <stdio.h>
#include <USB_Host.h>
#include <USB_VICE_Host.h>
#include <USART.h>
#include <USB_CDC_Device.h>
#include <string.h>
#include <USB_Host_Device_Manager.h>

char CDC_VICE_Command_Buffer[0x80];

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

	GPIOC->MODER |= (1<<0); 		// set PC0 to output
	GPIOC->ODR &= ~(1<<0);			// set PC0 LOW

	GPIOC->MODER |= (1<<2); 		// set PC1 to output
	GPIOC->ODR &= ~(1<<1);			// set PC1 LOW
}

void USB_CDC_Device_RX_Callback(USB_CDC_Device___MESSAGE_RECEIVED_CALLBACK_PARAMETERS)
{

	data[length-1] = 0;
	printf("%s\n", data);
}

void VICE_Interface_Connected_Callback(USB_VICE_Host___INTERFACE_CONNECTED_CALLBACK_PARAMETERS)
{
	USB_VICE_Host___Register_Interface(port_Number, device_Address, interface_Number);
	if(USB_CDC_Device___Is_Enabled(1))
	{
		uint16_t* product_Name = USB_Host_Device_Manager___Get_Product_String(port_Number, device_Address);
		uint16_t product_Name_Length = USB_Host_Device_Manager___Get_Product_String_Length(port_Number, device_Address);
		char product_String[0x40];

		for (uint8_t i = 0; i < product_Name_Length; i++)
		{
			product_String[i] = (uint8_t)product_Name[i];
		}

		product_String[product_Name_Length] = 0;

		snprintf(CDC_VICE_Command_Buffer, 0x80, "/connect {\"Device_ID\": %d,\"Device_Name\": \"%s\"}\n", device_Address, product_String);
		USB_CDC_Device___Send_Data(1, CDC_VICE_Command_Buffer, strlen(CDC_VICE_Command_Buffer));
	}
}

void VICE_Interface_Disconnected_Callback( USB_VICE_Host___INTERFACE_DISCONNECTED_CALLBACK_PARAMETERS)
{
	if (USB_CDC_Device___Is_Enabled(1))
	{
		snprintf(CDC_VICE_Command_Buffer, 0x80, "/disconnect {\"Device_ID\": %d}\n", device_Address);
		USB_CDC_Device___Send_Data(1, CDC_VICE_Command_Buffer, strlen(CDC_VICE_Command_Buffer));
	}
}

int main(void) {
	Skipper_Clock___Init();			// initiate the clock
	Skipper_Clock___Systick_Init();
	GPIO_init();					//initiate the GPIO to be used in this program.
	USART___Init(1);
	USART___Set_Baud_Rate(1, 2000000);

	USB_CDC_Device___Init(1);
	//USB_CDC_Device___Set_Interrupt_Char(1, '\n');
	USB_CDC_Device___Set_Interrupt_Char(1, '\r');
	USB_CDC_Device___Set_Message_Received_Callback(1, USB_CDC_Device_RX_Callback);
	USB_Host___Init(0);
	USB_VICE_Host___Init(0);
	USB_VICE_Host___Add_Interface_Connected_Callback(0, VICE_Interface_Connected_Callback);
	USB_VICE_Host___Add_Interface_Disconnected_Callback(0, VICE_Interface_Disconnected_Callback);
	for(;;)
	{
		USB_Host___Process(0);
	}
}

