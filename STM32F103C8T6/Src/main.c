#include <stdint.h>
#include "stm32f1xx.h"

void Clock_Init(){	// initiate clock ()
	RCC -> CR |= RCC_CR_HSEON;																						// enable HSE Oscillator
	while(!(RCC -> CR & RCC_CR_HSERDY));																			// wait for HSE To Stabilize
	FLASH -> ACR |= FLASH_ACR_LATENCY_2;																			// set flash wait to 2, because 48 MHz < SYSCLK  72 MHz
	RCC -> CFGR &= ~(RCC_CFGR_USBPRE);																				// set USB Pre-scaler to /1.5
	RCC -> CFGR |= 	(RCC_CFGR_PLLXTPRE		) |																		// set PLL Entry Pre-scaler to /2
					(RCC_CFGR_PLLMULL9		) |																		// set PLL Multiplication Factor to *9
					(RCC_CFGR_PLLSRC			) |																		// select HSE as the PLL Source
					(RCC_CFGR_ADCPRE_DIV6	) |																		// set ADC Pre-scaler to /6
					(RCC_CFGR_PPRE1_DIV2	);																		// set APB1 Pre-Scaler to /2
	RCC -> CR |= (RCC_CR_PLLON);																					// enable PLL
	while(!(RCC -> CR & RCC_CR_PLLRDY));																			// wait for PLL To Stabilize
	RCC -> CFGR |= (RCC_CFGR_SW_PLL);																				// switch PLL on
	while(!(RCC -> CFGR & RCC_CFGR_SWS_PLL));																		// Wait for source to switch to PLL
	RCC -> CR &= ~(1<<0);																							// Turn off HSI
}

void GPIO_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;					// Enable GPIOC clock
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;					// Enable GPIOB clock

	GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk;					// Clear PC13 mode
	GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk;					// clear PC13 configuration

	GPIOC->CRH |= GPIO_CRH_MODE13_0;						// Set PC13 to output (slow) for on board LED

	GPIOB->CRH &= ~GPIO_CRH_MODE8_Msk;					// Clear PB8 mode
	GPIOB->CRH &= ~GPIO_CRH_CNF8_Msk;					// clear PB8 configuration

	GPIOB->CRH &= ~GPIO_CRH_MODE9_Msk;					// Clear PB9 mode
	GPIOB->CRH &= ~GPIO_CRH_CNF9_Msk;					// clear PB9 configuration

	GPIOB->CRH |= GPIO_CRH_CNF8_0;						// set PB8 to floating input (CANRX)
	GPIOB->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1; 	// set PB9 to fast output mode (CANTX)
	GPIOB->CRH |= GPIO_CRH_CNF9_1;						// set PB9 to alt function push pull (CANTX)

	AFIO->MAPR &= ~AFIO_MAPR_CAN_REMAP_Msk;				// clear CAN remap
	AFIO->MAPR |= AFIO_MAPR_CAN_REMAP_1;					// remap CAN to PB8,PB9

}


void CAN_Init()
{

}

int main(void)
{
	Clock_Init();
	GPIO_Init();
	CAN_Init();

	for(;;)
	{
		for(uint32_t i = 0; i < 0xfffff; i++);
		GPIOC->ODR |= (1<<13);
		for(uint32_t i = 0; i < 0xfffff; i++);
		GPIOC->ODR &= ~(1<<13);
	}
}
