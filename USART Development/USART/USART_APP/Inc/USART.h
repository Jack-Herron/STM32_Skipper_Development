/*
 * USART.h
 *
 *  Created on: Nov 12, 2024
 *      Author: Jack Herron
 */

#ifndef USART_APP_INC_USART_H_
#define USART_APP_INC_USART_H_

#define USART___PARITY_ODD			USART_LL_Driver___PARITY_ODD
#define USART___PARITY_EVEN			USART_LL_Driver___PARITY_EVEN
#define USART___PARITY_NONE			USART_LL_Driver___PARITY_NONE

#define USART___STOP_BITS_HALF		USART_LL_Driver___STOP_BITS_HALF
#define USART___STOP_BITS_ONE		USART_LL_Driver___STOP_BITS_ONE
#define USART___STOP_BITS_ONE_HALF	USART_LL_Driver___STOP_BITS_ONE_HALF
#define USART___STOP_BITS_TWO		USART_LL_Driver___STOP_BITS_TWO

#define USART___ENABLE_STDIO 		true
#define USART___STDIO_BUFFER_SIZE	128
#define USART___STDIO_USART_NUMBER 	1

#define USART___DEFAULT_BAUD_RATE 	9600
#define USART___DEFAULT_PARITY		USART_LL_Driver___PARITY_NONE
#define USART___DEFAULT_STOP_BITS	USART_LL_Driver___STOP_BITS_ONE

typedef struct{
	char*		RX_Buffer;
	uint32_t 	RX_Buffer_Size;
	uint32_t 	RX_Buffer_Read_Index;
	uint32_t 	RX_Buffer_Write_Index;

	char*		TX_Buffer_Start;
	char*		TX_Buffer_End;

	uint32_t	interrupt_Char[8];
	void (*interrupt_Callback)(uint8_t, char, uint32_t);
}USART___USART_TypeDef;

void USART___Init(uint8_t USART_Number);
void USART___Set_Baud_Rate(uint8_t USART_Number, uint32_t Baud_Rate);
int USART___Read(uint8_t USART_Number, char *buffer, uint32_t buffer_Size);
void USART___Write(uint8_t USART_Number, char *buffer, uint32_t buffer_Size);
void USART___Set_Interrupt_Char(uint8_t USART_Number, char interrupt_Char);
void USART___Set_Interrupt_Callback(uint8_t USART_Number, void callback(uint8_t, char, uint32_t));
void USART___Read_Data_To_Buffer(uint8_t USART_Number, char* buffer, uint32_t data_Length);
void USART___Set_Parity(uint8_t USART_Number, uint8_t parity);
void USART___Set_Stop_Bits(uint8_t USART_Number, uint8_t stop_Bits);

#endif /* USART_APP_INC_USART_H_ */
