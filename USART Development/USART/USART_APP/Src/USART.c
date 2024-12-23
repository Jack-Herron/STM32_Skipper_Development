/*
 * USART.c
 *
 *  Created on: Nov 12, 2024
 *      Author: Jack Herron
 */

#include <stdint.h>
#include <stdlib.h>
#include <USART.h>
#include <USART_LL_Driver.h>
#include <stdbool.h>

USART___USART_TypeDef	USART___Port[USART_LL_Driver___NUM_USARTS] = {0};

#if USART___ENABLE_STDIO == true
	char STDIO_Buffer[USART___STDIO_BUFFER_SIZE];

	int _write(int file __attribute__((unused)), char *data, int len)
	{
		USART___Write(USART___STDIO_USART_NUMBER, data, len);
		return(0);
	}

	int _read(int file __attribute__((unused)), char *data, int len)
	{
		int length = USART___Read(USART___STDIO_USART_NUMBER, data, len);
		return length;
	}

#endif

void USART___Set_Baud_Rate(uint8_t USART_Number, uint32_t Baud_Rate)
{
	USART_LL_Driver___Set_Baud_Rate(USART_Number, Baud_Rate);
}

void USART___Set_Parity(uint8_t USART_Number, uint8_t parity)
{
	USART_LL_Driver___Set_Parity(USART_Number, parity);
}

void USART___Set_Stop_Bits(uint8_t USART_Number, uint8_t stop_Bits)
{
	USART_LL_Driver___Set_Stop_Bits(USART_Number, stop_Bits);
}

void USART___Set_RX_Buffer(uint8_t USART_Number, char* buffer, uint32_t buffer_Size)
{
	USART___Port[USART_Number].RX_Buffer = buffer;
	USART___Port[USART_Number].RX_Buffer_Size = buffer_Size;
}

void USART___Start_TX_Transfer(uint8_t USART_Number, char* buffer, uint32_t buffer_Size)
{
	USART___Port[USART_Number].TX_Buffer_Start = buffer;
	USART___Port[USART_Number].TX_Buffer_End = buffer+buffer_Size-1;
	USART_LL_Driver___Transfer_Out(USART_Number, *USART___Port[USART_Number].TX_Buffer_Start);
}

uint8_t USART___Is_TX_Busy(uint8_t USART_Number)
{
	if(USART___Port[USART_Number].TX_Buffer_Start != NULL)
	{
		return(1);
	}
	return(0);
}


void USART___TX_Callback(uint8_t USART_Number)
{
	USART___Port[USART_Number].TX_Buffer_Start++;

	if(USART___Port[USART_Number].TX_Buffer_Start <= USART___Port[USART_Number].TX_Buffer_End)
	{
		USART_LL_Driver___Transfer_Out(USART_Number, *USART___Port[USART_Number].TX_Buffer_Start);
	}
	else
	{
		USART___Port[USART_Number].TX_Buffer_Start = USART___Port[USART_Number].TX_Buffer_End = NULL;
	}
}

uint32_t USART___Next_Circular_Index(uint8_t current_Index, uint32_t buffer_Size)
{
	return((current_Index+1)%buffer_Size);
}

uint32_t USART___Get_RX_Buffer_Fill_Level(uint8_t USART_Number)
{
	uint32_t write_Index 		= USART___Port[USART_Number].RX_Buffer_Write_Index;
	uint32_t read_Index 		= USART___Port[USART_Number].RX_Buffer_Read_Index;
	uint32_t buffer_Size 		= USART___Port[USART_Number].RX_Buffer_Size;

	if(write_Index >= read_Index)
	{
		return(write_Index-read_Index);
	}
	else
	{
		return(buffer_Size-(read_Index-write_Index));
	}
}

void USART___RX_Callback(uint8_t USART_Number, uint8_t data)
{
	if(USART___Port[USART_Number].RX_Buffer != NULL)
	{
		char*	 buffer 			= USART___Port[USART_Number].RX_Buffer;
		uint32_t write_Index 		= USART___Port[USART_Number].RX_Buffer_Write_Index;
		uint32_t read_Index 		= USART___Port[USART_Number].RX_Buffer_Read_Index;
		uint32_t buffer_Size 		= USART___Port[USART_Number].RX_Buffer_Size;
		uint32_t next_Read_Index 	= USART___Next_Circular_Index(read_Index, buffer_Size);
		uint32_t next_Write_Index	= USART___Next_Circular_Index(write_Index, buffer_Size);

		buffer[write_Index]=data;

		if(next_Write_Index == read_Index)
		{
			USART___Port[USART_Number].RX_Buffer_Read_Index = next_Read_Index;
		}

		USART___Port[USART_Number].RX_Buffer_Write_Index = next_Write_Index;

		if(USART___Port[USART_Number].interrupt_Char[data/32] & (1 << (data % 32)))
		{
			if(USART___Port[USART_Number].interrupt_Callback != NULL)
			{
				USART___Port[USART_Number].interrupt_Callback(USART_Number, data, USART___Get_RX_Buffer_Fill_Level(USART_Number));
			}
		}
	}
}

void USART___Init(uint8_t USART_Number)
{
	USART_LL_Driver___Init(USART_Number);
	USART___Set_Baud_Rate(USART_Number, USART___DEFAULT_BAUD_RATE);
	USART___Set_Parity(USART_Number, USART___DEFAULT_PARITY);
	USART___Set_Stop_Bits(USART_Number, USART___DEFAULT_STOP_BITS);
	USART_LL_Driver___Set_RX_Callback(USART___RX_Callback);
	USART_LL_Driver___Set_TX_Callback(USART___TX_Callback);
#if USART___ENABLE_STDIO == true
	USART___Set_RX_Buffer(USART___STDIO_USART_NUMBER, STDIO_Buffer, USART___STDIO_BUFFER_SIZE);
#endif
}

char USART___Read_Next_Byte(uint8_t USART_Number)
{
	char return_Char = USART___Port[USART_Number].RX_Buffer[USART___Port[USART_Number].RX_Buffer_Read_Index];
	uint32_t read_Index 		= USART___Port[USART_Number].RX_Buffer_Read_Index;
	uint32_t buffer_Size 		= USART___Port[USART_Number].RX_Buffer_Size;

	USART___Port[USART_Number].RX_Buffer_Read_Index = USART___Next_Circular_Index(read_Index, buffer_Size);

	return(return_Char);
}

void USART___Read_Data_To_Buffer(uint8_t USART_Number, char* buffer, uint32_t data_Length)
{
	for(uint32_t i = 0; i < data_Length; i++)
	{
		buffer[i] = USART___Read_Next_Byte(USART_Number);
	}
}

void USART___Set_Interrupt_Char(uint8_t USART_Number, char interrupt_Char)
{
	USART___Port[USART_Number].interrupt_Char[interrupt_Char / 32] |= (1 << (interrupt_Char % 32));
}

void USART___Set_Interrupt_Callback(uint8_t USART_Number, void callback(uint8_t, char, uint32_t))
{
	USART___Port[USART_Number].interrupt_Callback = callback;
}

int USART___Read(uint8_t USART_Number, char *buffer, uint32_t buffer_Size)
{
	uint32_t read_Buffer_Index = 0;
	uint8_t cont = 1;
	void (*callback)(uint8_t, char, uint32_t) = USART___Port[USART_Number].interrupt_Callback;
	USART___Port[USART_Number].interrupt_Callback = NULL;

	while(cont == 1)
	{
		if(USART___Get_RX_Buffer_Fill_Level(USART_Number) > 0)
		{
			if(read_Buffer_Index < buffer_Size)
			{
				buffer[read_Buffer_Index] = USART___Read_Next_Byte(USART_Number);
				if (buffer[read_Buffer_Index] == '\n' || buffer[read_Buffer_Index] == '\r')
				{
					buffer[read_Buffer_Index] = '\n';
					cont = 0;
				}
				read_Buffer_Index++;
			}
			else
			{
				cont = 0;
			}
		}
	}

	USART___Port[USART_Number].interrupt_Callback = callback;
	return(read_Buffer_Index);
}

void USART___Write(uint8_t USART_Number, char *buffer, uint32_t buffer_Size)
{
	while(USART___Is_TX_Busy(USART_Number)) USART_LL_Driver___Process(USART_Number);

	USART___Start_TX_Transfer(USART_Number, buffer, buffer_Size);

	while(USART___Is_TX_Busy(USART_Number)) USART_LL_Driver___Process(USART_Number);

}
