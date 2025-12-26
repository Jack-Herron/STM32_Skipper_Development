/*
 * DSI_LCD.c
 *
 *  Created on: Dec 22, 2025
 *      Author: jackh
 */

#include "stm32f4xx.h"
#include "DSI_LCD.h"

void DSI_LCD_Init(void) {
	// Enable DSI clock
	RCC->APB2ENR |= RCC_APB2ENR_DSIEN;
	while (!(RCC->APB2ENR & RCC_APB2ENR_DSIEN)); // Wait for DSI clock to be enabled

}
