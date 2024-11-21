/*
 * USART_LL_Driver.h
 *
 *  Created on: Nov 12, 2024
 *      Author: Jack Herron
 */

#ifndef USART_LL_INC_USART_LL_DRIVER_H_
#define USART_LL_INC_USART_LL_DRIVER_H_

#define USART_INTERRUPTS_MSK        (USART_SR_RXNE | USART_SR_TC | USART_SR_LBD | USART_SR_CTS)


void USART_LL_Driver___Set_Baud_Rate(uint8_t USART_Number, uint32_t baud_Rate);
void USART_LL_Driver___Init(uint8_t USART_Number);

#endif /* USART_LL_INC_USART_LL_DRIVER_H_ */
