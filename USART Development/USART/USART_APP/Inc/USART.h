/*
 * USART.h
 *
 *  Created on: Nov 12, 2024
 *      Author: Jack Herron
 */

#ifndef USART_APP_INC_USART_H_
#define USART_APP_INC_USART_H_

#define USART___ENABLE_STDIO 		true
#define USART___STDIO_USART_NUMBER 	1

#define USART___DEFAULT_BAUD_RATE 9600

void USART___Init(uint8_t USART_Number);
void USART___Set_Baud_Rate(uint8_t USART_Number, uint32_t Baud_Rate);
int USART___Read(uint8_t USART_Number, char *buffer, uint32_t buffer_Size);
void USART___Write(uint8_t USART_Number, char *buffer, uint32_t buffer_Size);

#endif /* USART_APP_INC_USART_H_ */
