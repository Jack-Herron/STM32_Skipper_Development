/*
 * QSPI.h
 *
 *  Created on: Jan 3, 2026
 *      Author: jackh
 */

#ifndef INC_QSPI_H_
#define INC_QSPI_H_

#include "stm32f4xx.h"
#include "Clock.h"
#include "stdint.h"

// Config values
#define QSPI___MAX_FREQ				108000000ul



// Calculated values
#define QSPI___PRESCALER_VALUE		(((uint32_t)QSPI_FREQ + (uint32_t)QSPI___MAX_FREQ - 1ul)/(uint32_t)QSPI___MAX_FREQ) - 1ul
#define QSPI___REAL_FREQ			(uint32_t)QSPI_FREQ/(QSPI___PRESCALER_VALUE+1ul)

#endif /* INC_QSPI_H_ */
