/*
 * Skipper.cpp
 *
 *  Created on: Feb. 25, 2023
 *      Author: jackherron
 */
#include <stdint.h>
#include <stm32f4xx.h>
#include <Skipper.h>
#include <Skipper.h>

Clock clock;

uint32_t Clock::getCoreTimerFreq(){
	return coreTimerFreq;
}

uint32_t Clock::getCoreFreq(){
	return coreFreq;
}

uint32_t Clock::getAPB2PeripheralFreq(){
	return APB2PeripheralFreq;
}

uint32_t Clock::getAPB2TimerFreq(){
	return APB2TimerFreq;
}

uint32_t Clock::getAPB1PeripheralFreq(){
	return APB1PeripheralFreq;
}

uint32_t Clock::getAPB1TimerFreq(){
	return APB1TimerFreq;
}

void Clock::updateValues(){
	uint8_t tim1Multiplier;
	uint8_t tim2Multiplier;

	if(APB1 == 1){	tim1Multiplier = 1;	}
	else{			tim1Multiplier = 2;	}

	if(APB2 == 1){	tim2Multiplier = 1;	}
	else{			tim2Multiplier = 2;	}

	SysClk = (16000000/M*N/P);
	HClk = SysClk/AHB;
	coreTimerFreq = HClk;
	coreFreq = HClk;
	APB1PeripheralFreq = HClk/APB1;
	APB1TimerFreq = HClk/APB1*tim1Multiplier;
	APB2PeripheralFreq = HClk/APB2;
	APB2TimerFreq = HClk/APB2*tim2Multiplier;
}


void SysTick_Handler() {

}

void Clock::setParams(uint8_t setM,uint8_t setN,uint8_t setP,uint8_t setQ,uint8_t setAHB,uint16_t setAPB1,uint8_t setAPB2){
	M = setM;
	N = setN;
	P = setP;
	Q = setQ;
	AHB = setAHB;
	APB1 = setAPB1;
	APB2 = setAPB2;
}

void Clock::init(){

	uint8_t newP = (P-2)/2;
	uint8_t newAHB;
	uint8_t newAPB1;
	uint8_t newAPB2;

	switch(AHB){
		case 1:
			newAHB = 0b0000;
			break;
		case 2:
			newAHB = 0b1000;
			break;
		case 4:
			newAHB = 0b1001;
			break;
		case 8:
			newAHB = 0b1010;
			break;
		case 16:
			newAHB = 0b1011;
			break;
		case 64:
			newAHB = 0b1100;
			break;
		case 128:
			newAHB = 0b1101;
			break;
		case 256:
			newAHB = 0b1110;
			break;
		case 512:
			newAHB = 0b1111;
			break;
	}

	switch(APB1){
		case 1:
			newAPB1 = 0b000;
		break;

		case 2:
			newAPB1 = 0b100;
		break;

		case 4:
			newAPB1 = 0b101;
		break;

		case 8:
			newAPB1 = 0b110;
		break;

		case 16:
			newAPB1 = 0b111;
		break;

	}

	switch(APB2){
		case 1:
			newAPB2 = 0b000;
		break;

		case 2:
			newAPB2 = 0b100;
		break;

		case 4:
			newAPB2 = 0b101;
		break;

		case 8:
			newAPB2 = 0b110;
		break;

		case 16:
			newAPB2 = 0b111;
		break;

	}

	RCC->PLLCFGR = 0x00000000;																	//reset PLL register
	RCC -> CR |= RCC_CR_HSEON;																	//turn on HSE
	while(!(RCC -> CR & RCC_CR_HSERDY)){} 														//wait for HSE confirmation
	PWR ->CR |= PWR_CR_VOS; 																	// set up power
	FLASH -> ACR |= FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;	//Set up flash
	RCC->PLLCFGR |= (Q << 24) | (newP << 16) | (N << 6) | (M << 0);								// set PLL
	RCC->PLLCFGR |= (1<<22);
	RCC -> CR &= ~(1<<0);
	RCC -> CFGR |= (2 << 0) | (newAHB << 4) | (newAPB1<< 10) | (newAPB2<< 13);
	RCC -> CFGR |= (0 << 24);
	RCC -> CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	RCC -> CFGR |= RCC_CFGR_SW_PLL;
	while((RCC -> CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);
	updateValues();
	SysTick->LOAD = (HClk/1000)-1;
	SysTick->CTRL |= (1<<2);

}

void delay_ms(uint32_t ms){
	SysTick->CTRL = 0b000101;
	for(uint32_t i = 0; i < ms;	i++){
		while(!(SysTick->CTRL & (1<<16))){
			SysTick->CTRL = 0b000101;
		}
	}
	SysTick->CTRL = 0;
}

//SPI functions

void SPI::assignPort(SPI_TypeDef* i){			// this function is used to change the port variable
	port = i;
}



void SPI::setGPIO(){
	if(port == SPI1){
		RCC->AHB1ENR |= (1<<0); // enable GPIOA

		GPIOA->MODER |= (2 << 10);
		GPIOA->MODER |= (2 << 12);
		GPIOA->MODER |= (2 << 14);

		GPIOA->OSPEEDR |= (3 << 10);
		GPIOA->OSPEEDR |= (3 << 12);
		GPIOA->OSPEEDR |= (3 << 14);

		GPIOA->OTYPER &= ~(1<<5);
		GPIOA->OTYPER &= ~(1<<6);
		GPIOA->OTYPER &= ~(1<<7);

		GPIOA->AFR[0] |= (5<<20);
		GPIOA->AFR[0] |= (5<<24);
		GPIOA->AFR[0] |= (5<<28);
	}else if(port == SPI3){
		RCC->AHB1ENR |= (1<<1); // enable GPIOB

		GPIOB->MODER |= (2 << 6);
		GPIOB->MODER |= (2 << 8);
		GPIOB->MODER |= (2 << 10);

		GPIOB->OSPEEDR |= (3 << 6);
		GPIOB->OSPEEDR |= (3 << 8);
		GPIOB->OSPEEDR |= (3 << 10);

		GPIOB->OTYPER &= ~(1<<3);
		GPIOB->OTYPER &= ~(1<<4);
		GPIOB->OTYPER &= ~(1<<5);

		GPIOB->AFR[0] |= (6<<12);
		GPIOB->AFR[0] |= (6<<16);
		GPIOB->AFR[0] |= (6<<20);
	}
}

void SPI::init(void){
	if(port == SPI1){
		RCC->APB2ENR |= (1<<12); 						// enable SPI_1 clock
	}else if(port == SPI3){
		RCC->APB1ENR |= (1<<15); 						// enable SPI_3 clock
	}
	port->CR1 |= (mode<<0 | MSTR<<2 | BR<<3 | 3<<8);	//Configure SPI
	port->CR1 |= (1<<6); 								//Enable SPI
}

void SPI::deInit(void){
	if(port == SPI1){
		RCC->APB2RSTR |= (1<<12); 			// reset SPI_1
		RCC->APB2ENR &= ~(1<<12); 			// disable SPI_1 clock
		RCC->APB2RSTR &= ~(1<<12); 			// clear reset SPI_1
	}else if(port == SPI3){
		RCC->APB1RSTR |= (1<<15); 			// reset SPI_3
		RCC->APB1ENR &= ~(1<<15); 			// disable SPI_3 clock
		RCC->APB1RSTR &= ~(1<<15); 			// clear reset SPI_3
	}
}

void SPI::transmit(uint8_t* data, int size){
	uint8_t temp [[maybe_unused]];			// temporary variable used to force a read-clear on the status register
	for(int i = 0; i<size; i++){			// loop through the data array to transmit each byte
		while(!((port->SR) & (1<<1))){}; 	// wait for TX buffer to be empty
		port->DR = data[i];					// copy data to TX buffer
	}
	while((port->SR) & (1<<7)); 			// wait for busy to clear
	temp = port->DR;						// read-clear data register
	temp = port->SR;						// read-clear status register
}

void SPI::receive(uint8_t* ptr, uint8_t address, int size){
	uint8_t temp [[maybe_unused]];			// temporary variable used to force a read-clear on the status register
	while(!((port->SR) & (1<<1))){}; 		// wait for TX buffer to be empty
	port->DR = address;						// send address
	while(!((port->SR) & (1<<0))){}; 		// wait for read data to be ready
	ptr[0] = port->DR;						// read-clear data register
	for(int i = 1; i<size; i++){
		while(!((port->SR) & (1<<1))){}; 	// wait for TX buffer to be empty
		port->DR = 0; 						// send dummy data
		while(!((port->SR) & (1<<0))){}; 	// wait for read data to be ready
		ptr[i] = ((port->DR)); 				// copy read data to data array
	}
	while(!((port->SR) & (1<<1)));			// wait for TX buffer to be empty
	while((port->SR) & (1<<7)); 			// wait for busy to clear
	temp = port->SR; 						// read-clear status register
}

void SPI::setMode(uint8_t x){
	mode = x;
}



