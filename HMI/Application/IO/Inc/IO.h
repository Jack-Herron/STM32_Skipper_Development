/*
 * IO.h
 *
 *  Created on: Jan 5, 2026
 *      Author: Jack Herron
 */

#ifndef IO_INC_IO_H_
#define IO_INC_IO_H_

typedef struct
{
	uint8_t ID;
	uint8_t data[8];
	uint8_t data_Length;
}App___IO_RX_Data_Typedef;

typedef struct
{
	uint8_t ID;
	uint8_t data[8];
	uint8_t data_Length;
} App___IO_TX_Data_Typedef;

void IO___RX_Start_Task(void const * argument);
void IO___TX_Start_Task(void const * argument);

#endif /* IO_INC_IO_H_ */
