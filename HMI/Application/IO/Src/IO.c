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

void IO___Send_Lighting_Packet()
{
	App___IO_TX_Data_Typedef packet = {0};
	packet.ID = 0x100;
	packet.data_Length = 0x07;

	osMutexWait(App___IO_Control_State_Mutex, osWaitForever);

	packet.data[0] = App___IO_Control_State.lighting.white;
	packet.data[1] = App___IO_Control_State.lighting.white;
	packet.data[2] = App___IO_Control_State.lighting.red;
	packet.data[3] = App___IO_Control_State.lighting.red;
	packet.data[4] = App___IO_Control_State.lighting.lime;
	packet.data[5] = App___IO_Control_State.lighting.purple;
	packet.data[6] = App___IO_Control_State.lighting.far_Red;

	osMutexRelease(App___IO_Control_State_Mutex);

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
		osDelay(100);
	}
}
