/*
 * IO.c
 *
 *  Created on: Jan 5, 2026
 *      Author: Jack Herron
 */

#include <stdlib.h>
#include <stdint.h>
#include "App.h"
#include "IO.h"
#include "cmsis_os.h"

int IO___Push_Packet(App___IO_RX_Data_Typedef* packet)
{
	int ret;

	uint8_t next = (App___IO_Sense_State.RX_Buffer_Head + 1) % APP___IO_RX_FIFO_DEPTH;

	if (next == App___IO_Sense_State.RX_Buffer_Tail)
	{
		ret = 0;	// full
	}
	else
	{
		ret = 1;
		App___IO_Sense_State.RX_Buffer[App___IO_Sense_State.RX_Buffer_Head] = *packet;
		App___IO_Sense_State.RX_Buffer_Head = next;
	}

	return(ret);
}

App___IO_RX_Data_Typedef IO___Pop_Packet(void)
{
	App___IO_RX_Data_Typedef packet = {0};

	if (App___IO_Sense_State.RX_Buffer_Head != App___IO_Sense_State.RX_Buffer_Tail)
	{
		packet = App___IO_Sense_State.RX_Buffer[App___IO_Sense_State.RX_Buffer_Tail];
		App___IO_Sense_State.RX_Buffer_Tail = (App___IO_Sense_State.RX_Buffer_Tail + 1) % APP___IO_RX_FIFO_DEPTH;
	}

	return(packet);
}

void IO___Store_Packet(App___IO_RX_Data_Typedef* packet)
{
	IO___Push_Packet(packet);
}

void IO___Read_LED_Status_Packet(uint8_t* status)
{
	osMutexWait(App___IO_Sense_State_Mutex, osWaitForever);

	App___IO_Sense_State.lighting_Status.white 		= (status[0] + status[1])/2;
	App___IO_Sense_State.lighting_Status.red 		= (status[2] + status[3])/2;
	App___IO_Sense_State.lighting_Status.lime 		= status[4];
	App___IO_Sense_State.lighting_Status.purple 	= status[5];
	App___IO_Sense_State.lighting_Status.far_Red 	= status[6];

	osMutexRelease(App___IO_Sense_State_Mutex);
}

void IO___Read_Packets()
{
	App___IO_RX_Data_Typedef packet;

	do
	{
		packet = IO___Pop_Packet();

		switch(packet.ID)
		{
		case 0x200:
			IO___Read_LED_Status_Packet(packet.data);
			break;
		}

	}
	while(packet.ID != 0);
}

void IO___Sense_Start_Task(void const * argument)
{
	osSignalWait(APP___IO_SENSE_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		IO___Read_Packets();
		osDelay(5);
	}
}

void IO___Send_Lighting_Packet(void)
{
	App___IO_TX_Data_Typedef packet = {0};

	uint16_t white;
	uint16_t red;
	uint16_t lime;
	uint16_t purple;
	uint16_t far_red;

	/* Take one atomic snapshot of lighting state */
	osMutexWait(App___IO_Control_State_Mutex, osWaitForever);

	white   = App___IO_Control_State.lighting.white;
	red     = App___IO_Control_State.lighting.red;
	lime    = App___IO_Control_State.lighting.lime;
	purple  = App___IO_Control_State.lighting.purple;
	far_red = App___IO_Control_State.lighting.far_Red;

	osMutexRelease(App___IO_Control_State_Mutex);

	/* Transmit White */
	packet.ID = 0x100;
	packet.data_Length = 0x02;
	packet.data[0] = (uint8_t)(white & 0xFF);
	packet.data[1] = (uint8_t)(white >> 8);
	App___Transmit(packet);

	/* Transmit Red */
	packet.ID = 0x101;
	packet.data_Length = 0x02;
	packet.data[0] = (uint8_t)(red & 0xFF);
	packet.data[1] = (uint8_t)(red >> 8);
	App___Transmit(packet);

	/* Transmit Lime */
	packet.ID = 0x102;
	packet.data_Length = 0x02;
	packet.data[0] = (uint8_t)(lime & 0xFF);
	packet.data[1] = (uint8_t)(lime >> 8);
	App___Transmit(packet);

	/* Transmit Purple */
	packet.ID = 0x103;
	packet.data_Length = 0x02;
	packet.data[0] = (uint8_t)(purple & 0xFF);
	packet.data[1] = (uint8_t)(purple >> 8);
	App___Transmit(packet);

	/* Transmit Far Red */
	packet.ID = 0x104;
	packet.data_Length = 0x02;
	packet.data[0] = (uint8_t)(far_red & 0xFF);
	packet.data[1] = (uint8_t)(far_red >> 8);
	App___Transmit(packet);
}

void IO___Send_Packets()
{
	IO___Send_Lighting_Packet();
}

void IO___Control_Start_Task(void const * argument)
{
	osSignalWait(APP___IO_CONTROL_TASK_START_FLAG, osWaitForever);

	for(;;)
	{
		IO___Send_Packets();
		osDelay(5);
	}
}
