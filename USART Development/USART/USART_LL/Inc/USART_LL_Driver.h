/*
 * USART_LL_Driver.h
 *
 *  Created on: Nov 12, 2024
 *      Author: Jack Herron
 */

#ifndef USART_LL_INC_USART_LL_DRIVER_H_
#define USART_LL_INC_USART_LL_DRIVER_H_

#define USART_INTERRUPTS_MSK        (USART_SR_RXNE | USART_SR_TC | USART_SR_LBD | USART_SR_CTS)

typedef struct
{
	uint8_t* TX_Buffer_Head;
	uint8_t* TX_Buffer_Tail;
	uint8_t* RX_Buffer;
	uint32_t RX_Buffer_Size;
	uint32_t RX_Buffer_Index;

} USART_LL_Driver___USART_Port_TypeDef;

void USART_LL_Driver___Set_Baud_Rate(uint8_t USART_Number, uint32_t baud_Rate);
void USART_LL_Driver___Init(uint8_t USART_Number);
void USART_LL_Driver___Set_RX_Buffer(uint8_t USART_Number, uint8_t *buffer, uint32_t buffer_Size);
uint32_t USART_LL_Driver___Get_RX_Buffer_Index(uint8_t USART_Number);
void USART_LL_Driver___Start_TX_Transfer(uint8_t USART_Number);
void USART_LL_Driver___Set_TX_Buffer(uint8_t USART_Number, uint8_t* buffer, uint32_t buffer_Size);
uint8_t USART_LL_Driver___Is_TX_Busy(uint8_t USART_Number);

#endif /* USART_LL_INC_USART_LL_DRIVER_H_ */
