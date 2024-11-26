/*
 * USART.c
 *
 *  Created on: Nov 12, 2024
 *      Author: Jack Herron
 */

#include <stdint.h>
#include <USART.h>
#include <USART_LL_Driver.h>

#define TEST_BUFFER_SIZE 4
uint8_t buffer[TEST_BUFFER_SIZE];
uint32_t next_Read_Byte = 0;

#if USART___ENABLE_STDIO == true

	int _write(int file, char *data, int len)
	{
		USART___Write(USART___STDIO_USART_NUMBER, data, len);
		return(0);
	}

	int _read(int file, char *data, int len)
	{
		int length = USART___Read(USART___STDIO_USART_NUMBER, data, len);
		return length;
	}

#endif

void USART___Set_Baud_Rate(uint8_t USART_Number, uint32_t Baud_Rate)
{
	USART_LL_Driver___Set_Baud_Rate(USART_Number, Baud_Rate);
}

void USART___Init(uint8_t USART_Number)
{
	USART_LL_Driver___Init(USART_Number);
	USART___Set_Baud_Rate(USART_Number, USART___DEFAULT_BAUD_RATE);
	USART_LL_Driver___Set_RX_Buffer(USART_Number, buffer, TEST_BUFFER_SIZE);
}

char USART___Read_Next_Byte()
{
	uint32_t return_Byte;

	if (next_Read_Byte < TEST_BUFFER_SIZE - 1)
	{
		return_Byte = next_Read_Byte++;
	}
	else
	{
		return_Byte = next_Read_Byte;
		next_Read_Byte = 0;
	}
	return (buffer[return_Byte]);
}

int USART___Read(uint8_t USART_Number, char *buffer, uint32_t buffer_Size)
{
	uint32_t Read_Buffer_Index = 0;
	uint8_t cont = 1;
	while(cont == 1)
	{
		if(next_Read_Byte != USART_LL_Driver___Get_RX_Buffer_Index(USART_Number))
		{
			buffer[Read_Buffer_Index] = USART___Read_Next_Byte();
			if (buffer[Read_Buffer_Index] == '\n' || buffer[Read_Buffer_Index] == '\r')
			{
				buffer[Read_Buffer_Index] = '\n';
				cont = 0;
			}
			Read_Buffer_Index++;
		}
	}
	return Read_Buffer_Index;
}

void USART___Write(uint8_t USART_Number, char *buffer, uint32_t buffer_Size)
{
	while(USART_LL_Driver___Is_TX_Busy(USART_Number));

	USART_LL_Driver___Set_TX_Buffer(USART_Number, (uint8_t*)buffer, buffer_Size);
	USART_LL_Driver___Start_TX_Transfer(USART_Number);

	while(USART_LL_Driver___Is_TX_Busy(USART_Number));

}
