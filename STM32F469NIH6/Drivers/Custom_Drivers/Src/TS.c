/*
 * TS.c
 *
 *  Created on: Jan 1, 2026
 *      Author: jackh
 */

#include "TS.h"
#include "I2C.h"
#include "ft6x06.h"
#include "Clock.h"
#include <stdlib.h>

void (*event_Callback)(void) = NULL;

void TS___I2C_Pin_Init(void)
{
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOBEN;				// Enable GPIOB clock
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOJEN;				// Enable GPIOB clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;   			// enable SYSCFG clock

	GPIOB -> MODER &= ~GPIO_MODER_MODER8_Msk;			// Clear mode fields for PB8, PB9
	GPIOB -> MODER &= ~GPIO_MODER_MODER9_Msk;
	GPIOB -> MODER |= GPIO_MODER_MODER8_1;				// Set PB8, PB9 to alt function mode
	GPIOB -> MODER |= GPIO_MODER_MODER9_1;

	GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL8_Msk;
	GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL9_Msk;
	GPIOB->AFR[1] |= 4 << GPIO_AFRH_AFSEL8_Pos;			// Set PB8, PB9 to I2C mode (AF4)
	GPIOB->AFR[1] |= 4 << GPIO_AFRH_AFSEL9_Pos;			// Set PB8, PB9 to I2C mode (AF4)

	GPIOB->OTYPER &= ~GPIO_OTYPER_OT8_Msk;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT9_Msk;
	GPIOB->OTYPER |= GPIO_OTYPER_OT8;					// set PB8, PB9 to open drain
	GPIOB->OTYPER |= GPIO_OTYPER_OT9;

	GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8_Msk;
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED9_Msk;
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1;			// set PB8, PB9 to high speed mode
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED9_1;

	GPIOJ->MODER &= ~GPIO_MODER_MODER5_Msk;				// Clear PJ5 mode (input mode) this is the int pin from TS

	GPIOJ->PUPDR &= ~GPIO_PUPDR_PUPD5_Msk;
	GPIOJ->PUPDR |=  GPIO_PUPDR_PUPD5_0;  			  	// pull-up PJ5

	EXTI->IMR |= EXTI_IMR_MR5;							// enable Px5 interrupt
	EXTI->FTSR |= EXTI_FTSR_TR5;						// enable PJ5 falling edge interrupt

	SYSCFG->EXTICR[1] &= ~SYSCFG_EXTICR2_EXTI5_Msk;
	SYSCFG->EXTICR[1] |=  SYSCFG_EXTICR2_EXTI5_PJ;  	// route EXTI5 to Port J

	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_SetPriority(EXTI9_5_IRQn, 14);
}

void TS___I2C_Init(void)
{
	I2C___Conf_TypeDef I2C_Conf;

	I2C_Conf.Freq 			= TS___I2C_FREQ;
	I2C_Conf.Rise_Time_ns 	= TS___I2C_RISE_TIME_ns;
	I2C_Conf.Fm_Mode		= 0;

	I2C___Init(TS___I2C_PORT, &I2C_Conf);
}

void TS___Get_Point(uint16_t* x, uint16_t* y)
{
	ft6x06_ts_drv.GetXY(TS___I2C_ADDRESS, x, y);
}

uint8_t TS___Get_Num_Points_Pressed()
{
	return(ft6x06_ts_drv.DetectTouch(TS___I2C_ADDRESS));
}

void TS___Init(void)
{
	TS___I2C_Pin_Init();
	TS___I2C_Init();
	ft6x06_ts_drv.Init(TS___I2C_ADDRESS);
	ft6x06_ts_drv.Start(TS___I2C_ADDRESS);
	ft6x06_ts_drv.EnableIT(TS___I2C_ADDRESS);
}

void TS_IO_Init(void)
{
	if(!I2C___Is_Initilized(TS___I2C_PORT))
	{
		TS___I2C_Init();
	}
}

void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
	uint8_t data[2] = {Reg, Value};
	I2C___Write(TS___I2C_PORT, Addr, data, 2);
}

uint8_t TS_IO_Read(uint8_t Addr, uint8_t Reg)
{
	uint8_t temp;

	uint8_t ret;
	ret = I2C___Read_Reg(TS___I2C_PORT, Addr, Reg, &temp, 1);

	while(ret);

	return(temp);
}

uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length)
{
	return((uint16_t)I2C___Read_Reg(TS___I2C_PORT, Addr, Reg, Buffer, Length));
}

void TS_IO_Delay(uint32_t Delay)
{
	clock___delay_ms(Delay);
}

void TS___Set_Event_Callback(void (*callback)(void))
{
	event_Callback = callback;
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR5) {
		EXTI->PR = EXTI_PR_PR5;   // clear interrupt
		if(event_Callback != NULL)
		{
			event_Callback();
		}
	}
}


