/*
 * FMC_SDRAM.c
 *
 *  Created on: Dec 22, 2025
 *      Author: jackh
 */
#include "FMC_SDRAM.h"
#include "stm32f4xx.h"

void GPIO_Pin_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIOx->MODER &= ~(3 << (GPIO_Pin * 2));							// clear mode bits
	GPIOx->AFR[GPIO_Pin/8] &= ~(0xF << ((GPIO_Pin & 0x07) * 4));	// clear alternate function bits
	GPIOx->OSPEEDR &= ~(3 << (GPIO_Pin * 2));						// clear speed bits
	GPIOx->PUPDR &= ~(3 << (GPIO_Pin * 2));							// clear pull-up/pull-down bits

	GPIOx->AFR[GPIO_Pin/8] |= (12 << ((GPIO_Pin & 0x07) * 4));		// set alternate function 12 (FMC)
	GPIOx->MODER |= (2 << (GPIO_Pin * 2));							// set mode to alternate function
	GPIOx->OSPEEDR |= (3 << (GPIO_Pin * 2));						// set speed to high
	GPIOx->PUPDR |= (1 << (GPIO_Pin * 2));							// set pull-up
}

void FMC_SDRAM___Pin_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enable GPIOB clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable GPIOD clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // Enable GPIOE clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN; // Enable GPIOF clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN; // Enable GPIOG clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN; // Enable GPIOH clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN; // Enable GPIOI clock

	// GPIOC

	GPIO_Pin_Init(GPIOC, 0);

	// GPIOD

	GPIO_Pin_Init(GPIOD, 0);
	GPIO_Pin_Init(GPIOD, 1);
	GPIO_Pin_Init(GPIOD, 8);
	GPIO_Pin_Init(GPIOD, 9);
	GPIO_Pin_Init(GPIOD, 10);
	GPIO_Pin_Init(GPIOD, 14);
	GPIO_Pin_Init(GPIOD, 15);

	// GPIOE

	GPIO_Pin_Init(GPIOE, 0);
	GPIO_Pin_Init(GPIOE, 1);
	GPIO_Pin_Init(GPIOE, 7);
	GPIO_Pin_Init(GPIOE, 8);
	GPIO_Pin_Init(GPIOE, 9);
	GPIO_Pin_Init(GPIOE, 10);
	GPIO_Pin_Init(GPIOE, 11);
	GPIO_Pin_Init(GPIOE, 12);
	GPIO_Pin_Init(GPIOE, 13);
	GPIO_Pin_Init(GPIOE, 14);
	GPIO_Pin_Init(GPIOE, 15);

	// GPIOF

	GPIO_Pin_Init(GPIOF, 0);
	GPIO_Pin_Init(GPIOF, 1);
	GPIO_Pin_Init(GPIOF, 2);
	GPIO_Pin_Init(GPIOF, 3);
	GPIO_Pin_Init(GPIOF, 4);
	GPIO_Pin_Init(GPIOF, 5);
	GPIO_Pin_Init(GPIOF, 11);
	GPIO_Pin_Init(GPIOF, 12);
	GPIO_Pin_Init(GPIOF, 13);
	GPIO_Pin_Init(GPIOF, 14);
	GPIO_Pin_Init(GPIOF, 15);

	// GPIOG

	GPIO_Pin_Init(GPIOG, 0);
	GPIO_Pin_Init(GPIOG, 1);
	GPIO_Pin_Init(GPIOG, 4);
	GPIO_Pin_Init(GPIOG, 5);
	GPIO_Pin_Init(GPIOG, 8);
	GPIO_Pin_Init(GPIOG, 15);

	// GPIOH

	GPIO_Pin_Init(GPIOH, 2);
	GPIO_Pin_Init(GPIOH, 3);
	GPIO_Pin_Init(GPIOH, 8);
	GPIO_Pin_Init(GPIOH, 9);
	GPIO_Pin_Init(GPIOH, 10);
	GPIO_Pin_Init(GPIOH, 11);
	GPIO_Pin_Init(GPIOH, 12);
	GPIO_Pin_Init(GPIOH, 13);
	GPIO_Pin_Init(GPIOH, 14);
	GPIO_Pin_Init(GPIOH, 15);

	// GPIOI

	GPIO_Pin_Init(GPIOI, 0);
	GPIO_Pin_Init(GPIOI, 1);
	GPIO_Pin_Init(GPIOI, 2);
	GPIO_Pin_Init(GPIOI, 3);
	GPIO_Pin_Init(GPIOI, 4);
	GPIO_Pin_Init(GPIOI, 5);
	GPIO_Pin_Init(GPIOI, 6);
	GPIO_Pin_Init(GPIOI, 7);
	GPIO_Pin_Init(GPIOI, 9);
	GPIO_Pin_Init(GPIOI, 10);


}

void FMC_SDRAM___Send_Command(FMC_SDRAM___Command_TypeDef *Command)
{
	uint32_t tmpreg = 0;

	tmpreg |= (	Command->CommandMode 			<< FMC_SDCMR_MODE_Pos);
	tmpreg |= (	Command->CommandTarget			<< FMC_SDCMR_CTB2_Pos);
	tmpreg |= ((Command->AutoRefreshNumber-1) 	<< FMC_SDCMR_NRFS_Pos);
	tmpreg |= (	Command->ModeRegisterDefinition << FMC_SDCMR_MRD_Pos);

	FMC_Bank5_6->SDCMR = tmpreg;				// write command

	while (FMC_Bank5_6->SDSR & FMC_SDSR_BUSY); 	// Wait until command is done
}

void FMC_SDRAM___SDRAM_Init(void)
{
	RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN; // Enable FMC clock
	FMC_SDRAM___Pin_Init();

	FMC_Bank5_6->SDCR[0] = 0;	// Reset SDRAM Control Register 1
	FMC_Bank5_6->SDCR[0] |= ((NUM_COLUMN_BITS) 	<< FMC_SDCR1_NC_Pos);		// Set number of column bits
	FMC_Bank5_6->SDCR[0] |= ((NUM_ROW_BITS) 	<< FMC_SDCR1_NR_Pos);		// Set number of row bits
	FMC_Bank5_6->SDCR[0] |= (MEMORY_WIDTH 		<< FMC_SDCR1_MWID_Pos);		// Set memory data bus width
	FMC_Bank5_6->SDCR[0] |= (NUM_INTERNAL_BANKS	<< FMC_SDCR1_NB_Pos);		// Set number of internal banks
	FMC_Bank5_6->SDCR[0] |= (CAS_LATENCY 		<< FMC_SDCR1_CAS_Pos);		// Set CAS latency
	FMC_Bank5_6->SDCR[0] |= (MEM_FREQ_DIV 		<< FMC_SDCR1_SDCLK_Pos);	// Set SDRAM clock period
	FMC_Bank5_6->SDCR[0] |= (READ_BURST_ENABLE	<< FMC_SDCR1_RBURST_Pos);	// Set burst length
	FMC_Bank5_6->SDCR[0] |= (READ_PIPE_DELAY 	<< FMC_SDCR1_RPIPE_Pos);	// Set read pipe delay

	FMC_Bank5_6->SDTR[0] = 0;	// Reset SDRAM Timing Register 1
	FMC_Bank5_6->SDTR[0] |= (TRC_DELAY 			<< FMC_SDTR1_TRC_Pos);		// Set row cycle delay
	FMC_Bank5_6->SDTR[0] |= (TRP_DELAY 			<< FMC_SDTR1_TRP_Pos);		// Set row precharge delay
	FMC_Bank5_6->SDTR[0] |= (TWR_DELAY 			<< FMC_SDTR1_TWR_Pos);		// Set write recovery delay
	FMC_Bank5_6->SDTR[0] |= (TRCD_DELAY 		<< FMC_SDTR1_TRCD_Pos);		// Set row to column delay
	FMC_Bank5_6->SDTR[0] |= (TRAS_DELAY 		<< FMC_SDTR1_TRAS_Pos);		// Set active to precharge delay
	FMC_Bank5_6->SDTR[0] |= (TXSR_DELAY 		<< FMC_SDTR1_TXSR_Pos);		// Set exit self-refresh to active delay
	FMC_Bank5_6->SDTR[0] |= (TMRD_DELAY 		<< FMC_SDTR1_TMRD_Pos);		// Set load mode register to active delay

	FMC_SDRAM___Command_TypeDef Command;
	Command.CommandMode 			= FMC_SDRAM___CMD_CLK_ENABLE;
	Command.CommandTarget 			= FMC_SDRAM___CMD_TARGET_BANK1;
	Command.AutoRefreshNumber 		= 1;
	Command.ModeRegisterDefinition 	= 0;
	FMC_SDRAM___Send_Command(&Command);

	Command.CommandMode 			= FMC_SDRAM___CMD_PALL;
	Command.CommandTarget 			= FMC_SDRAM___CMD_TARGET_BANK1;
	Command.AutoRefreshNumber 		= 1;
	Command.ModeRegisterDefinition 	= 0;
	FMC_SDRAM___Send_Command(&Command);

	Command.CommandMode 			= FMC_SDRAM___CMD_AUTOREFRESH;
	Command.CommandTarget 			= FMC_SDRAM___CMD_TARGET_BANK1;
	Command.AutoRefreshNumber 		= 1;
	Command.ModeRegisterDefinition 	= 0;
	FMC_SDRAM___Send_Command(&Command);

	uint32_t ModeRegisterDefinition = (
			(FMC_SDRAM___MODEREG_BURST_LENGTH       << FMC_SDRAM___MODEREG_BURST_LENGTH_Pos)	|
			(FMC_SDRAM___MODEREG_BURST_TYPE   		<< FMC_SDRAM___MODEREG_BURST_TYPE_Pos)		|
			(FMC_SDRAM___MODEREG_CAS_LATENCY        << FMC_SDRAM___MODEREG_CAS_LATENCY_Pos)		|
			(FMC_SDRAM___MODEREG_OPERATING_MODE		<< FMC_SDRAM___MODEREG_OPERATING_MODE_Pos)	|
			(FMC_SDRAM___MODEREG_WRITEBURST_MODE	<< FMC_SDRAM___MODEREG_WRITEBURST_MODE_Pos));

	Command.CommandMode 			= FMC_SDRAM___CMD_LOAD_MODE;
	Command.CommandTarget 			= FMC_SDRAM___CMD_TARGET_BANK1;
	Command.AutoRefreshNumber 		= 1;
	Command.ModeRegisterDefinition 	= ModeRegisterDefinition;
	FMC_SDRAM___Send_Command(&Command);

	FMC_Bank5_6->SDRTR |= (REFRESH_COUNT << FMC_SDRTR_COUNT_Pos);
}
