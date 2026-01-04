/*
 * Flash.c
 *
 *  Created on: Jan 3, 2026
 *      Author: jackh
 */

#include "STM32F4xx.h"
#include "Flash.h"
#include "Clock.h"
#include "QSPI.h"

void Flash___GPIO_Init(void)
{

}

void Flash___Init(void)
{
	Flash___GPIO_Init();
	QSPI___Init();
}
