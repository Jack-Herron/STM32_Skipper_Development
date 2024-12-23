/*
 * USART_LL_Driver.h
 *
 *  Created on: Nov 12, 2024
 *      Author: Jack Herron
 */

#ifndef USART_LL_INC_USART_LL_DRIVER_H_
#define USART_LL_INC_USART_LL_DRIVER_H_

#define USART_INTERRUPTS_MSK        (USART_SR_RXNE | USART_SR_TC | USART_SR_LBD | USART_SR_CTS | USART_SR_ORE | USART_SR_PE)
#define USART_LL_Driver___NUM_USARTS	6
#define USART_LL_Driver___ERROR_BYTE	255

#define USART_LL_Driver___PARITY_NONE	0
#define USART_LL_Driver___PARITY_ODD	1
#define USART_LL_Driver___PARITY_EVEN	2

#define USART_LL_Driver___STOP_BITS_HALF		1
#define USART_LL_Driver___STOP_BITS_ONE			0
#define USART_LL_Driver___STOP_BITS_ONE_HALF	3
#define USART_LL_Driver___STOP_BITS_TWO			2

#define USART_1							1
#define USART_2							2
#define USART_3							3
#define USART_4							4
#define USART_5							5
#define USART_6							6

void USART_LL_Driver___Set_Baud_Rate(uint8_t USART_Number, uint32_t baud_Rate);
void USART_LL_Driver___Init(uint8_t USART_Number);
void USART_LL_Driver___Set_RX_Callback(void callback(uint8_t, uint8_t));
void USART_LL_Driver___Set_TX_Callback(void callback(uint8_t));
void USART_LL_Driver___Transfer_Out(uint8_t USART_Number, uint8_t data);
void USART_LL_Driver___Process(uint8_t USART_Number);
void USART_LL_Driver___Set_Parity(uint8_t USART_Number, uint8_t parity);
void USART_LL_Driver___Set_Stop_Bits(uint8_t USART_Number, uint8_t stop_Bits);

#endif /* USART_LL_INC_USART_LL_DRIVER_H_ */
