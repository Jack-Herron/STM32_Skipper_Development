/*
 * TS.c
 *
 *  Created on: Jan 1, 2026
 *      Author: jackh
 */

#include "TS.h"
#include "I2C.h"
#include "gt911.h"
#include "Clock.h"
#include <stdlib.h>

void (*event_Callback)(void) = NULL;

void TS___I2C_Pin_Init(void)
{
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;				// Enable GPIOA clock
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOCEN;				// Enable GPIOC clock

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;   			// enable SYSCFG clock

	GPIOA -> MODER &= ~GPIO_MODER_MODER8_Msk;			// Clear mode fields for PA8, PC9
	GPIOC -> MODER &= ~GPIO_MODER_MODER9_Msk;
	GPIOA -> MODER |= GPIO_MODER_MODER8_1;				// Set PB8, PC9 to alt function mode
	GPIOC -> MODER |= GPIO_MODER_MODER9_1;

	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8_Msk;
	GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL9_Msk;
	GPIOA->AFR[1] |= 4 << GPIO_AFRH_AFSEL8_Pos;			// Set PA8, PC9 to I2C mode (AF4)
	GPIOC->AFR[1] |= 4 << GPIO_AFRH_AFSEL9_Pos;			// Set PA8, PC9 to I2C mode (AF4)

	GPIOA->OTYPER &= ~GPIO_OTYPER_OT8_Msk;
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT9_Msk;
	GPIOA->OTYPER |= GPIO_OTYPER_OT8;					// set PA8, PC9 to open drain
	GPIOC->OTYPER |= GPIO_OTYPER_OT9;

	GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8_Msk;
	GPIOC->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED9_Msk;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1;			// set PA8, PC9 to high speed mode
	GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED9_1;

	GPIOA->MODER &= ~GPIO_MODER_MODER9_Msk;

	GPIOA->MODER |= GPIO_MODER_MODER9_0; 												// Set PA9 to Output mode
	GPIOA->ODR &= ~GPIO_ODR_OD9; 														// Set PA9 Low (while TS Reset active)


	GPIOA->MODER |= GPIO_MODER_MODER10_0; 												// Set PA10 to Output mode
	GPIOA->ODR &= ~GPIO_ODR_OD10; 														// Set PA10 Low (TS Reset active)
	clock___delay_ms(20);					                                            // Delay 20ms
	GPIOA->ODR |= GPIO_ODR_OD10; 														// Set PA10 High (TS Reset inactive)
	clock___delay_ms(200);

	GPIOA->MODER &= ~GPIO_MODER_MODER9_Msk;				// Clear PA9 mode (input mode) this is the int pin from TS

	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD9_Msk;
	GPIOA->PUPDR |=  GPIO_PUPDR_PUPD9_0;  			  	// pull-up PA9

	EXTI->IMR |= EXTI_IMR_MR9;							// enable Px9 interrupt
	EXTI->FTSR |= EXTI_FTSR_TR9;						// enable PJ9 falling edge interrupt

	SYSCFG->EXTICR[2] &= ~SYSCFG_EXTICR3_EXTI9_Msk;
	SYSCFG->EXTICR[2] |=  SYSCFG_EXTICR3_EXTI9_PA;  	// route EXTI5 to Port A

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
	GT911_ts_drv.GetXY(TS___I2C_ADDRESS, x, y);
}

uint8_t TS___Get_Num_Points_Pressed()
{
	return(GT911_ts_drv.DetectTouch(TS___I2C_ADDRESS));
}

void TS___Init(void)
{
	TS___I2C_Pin_Init();
	TS___I2C_Init();
	GT911_ts_drv.ReadID(TS___I2C_ADDRESS);
	GT911_ts_drv.Init(TS___I2C_ADDRESS);
	GT911_ts_drv.Start(TS___I2C_ADDRESS);
	GT911_ts_drv.EnableIT(TS___I2C_ADDRESS);
}

void TS_IO_Init(void)
{
	if(!I2C___Is_Initilized(TS___I2C_PORT))
	{
		TS___I2C_Init();
	}
}

void TS_IO_Write(uint8_t Addr, uint16_t Reg, uint8_t Value)
{
	uint8_t data[3] = {((uint8_t*)&Reg)[1], ((uint8_t*)&Reg)[0], Value};
	I2C___Write(TS___I2C_PORT, Addr, data, 3);
}

uint8_t TS_IO_Read(uint8_t Addr, uint16_t Reg)
{
	uint8_t temp;

	uint8_t ret;
	ret = I2C___Read_Reg16(TS___I2C_PORT, Addr, Reg, &temp, 1);

	if(ret)
	{
		return(0);
	}

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
	if (EXTI->PR & EXTI_PR_PR9) {
		EXTI->PR = EXTI_PR_PR9;   // clear interrupt
		if(event_Callback != NULL)
		{
			event_Callback();
		}
	}
}


