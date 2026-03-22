/*
 * IO.h
 *
 *  Created on: Jan 5, 2026
 *      Author: Jack Herron
 */

#ifndef IO_INC_IO_H_
#define IO_INC_IO_H_

void IO___Sense_Start_Task(void const * argument);
void IO___Control_Start_Task(void const * argument);
void IO___Store_Packet(App___IO_RX_Data_Typedef* packet);

#endif /* IO_INC_IO_H_ */
