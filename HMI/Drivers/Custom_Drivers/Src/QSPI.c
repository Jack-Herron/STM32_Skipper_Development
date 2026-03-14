/*
 * QSPI.c
 *
 *  Created on: Jan 3, 2026
 *      Author: jackh
 */

#include "stm32f4xx.h"
#include "Clock.h"
#include "QSPI.h"

void QSPI___Init()
{
	RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN;



}
