/*
 * USB_Host_Pipes.h
 *
 *  Created on: Jun 23, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_PIPES_H_
#define CORE_INC_USB_HOST_PIPES_H_

#include <USB_LL_Definitions.h>
#include <stdint.h>

#define USB_Host_Pipes___NUMBER_OF_PIPES        USB_LL_Definitions___MAX_NUMBER_OF_CHANNELS_PER_PORT

typedef struct {
	uint8_t 	is_Allocated;
	uint8_t 	ID;
	uint32_t	max_Packet_Size;
	uint8_t		device_Address;
	uint8_t		endpoint_Number;
	uint8_t		pipe_Type;				// 0 = Control, 1 = Bulk, 2 = Interrupt, 3 = Isochronous
	uint8_t		pipe_Direction;         // 0 = Out, 1 = In
	uint8_t 	multi_Count;
	uint8_t		is_Odd_Frame;
	uint8_t		is_Low_Speed;
	uint8_t* 	p_Buffer;
	uint32_t	transfer_Length;
	uint32_t	num_Packets;
	uint32_t	num_Packets_Remaining;
}USB_Host_Pipes___Pipe_TypeDef;

void USB_Host_Pipes___Create_Pipe(uint8_t port_Number, uint8_t device_Address, uint8_t pipe_ID, uint8_t pipe_Type, uint8_t pipe_Direction, uint8_t endpoint_Number, uint32_t max_Packet_Size, uint8_t* p_Buffer, uint32_t transfer_Length, uint8_t is_Odd_Frame, uint8_t is_Low_Speed, uint8_t multi_Count, uint8_t packet_ID);

#endif /* CORE_INC_USB_HOST_PIPES_H_ */
