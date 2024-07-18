/*
 * USB_Host_Pipes.h
 *
 *  Created on: Jun 23, 2024
 *      Author: Jack Herron
 */

#ifndef CORE_INC_USB_HOST_PIPES_H_
#define CORE_INC_USB_HOST_PIPES_H_

#include <USB_LL_Definitions.h>
#include <USB_LL_Host.h>
#include <stdint.h>

#define USB_Host_Pipes___NUMBER_OF_PIPES        		USB_LL_Definitions___MAX_NUMBER_OF_CHANNELS_PER_PORT
#define USB_Host_Pipes___PID_SETUP						USB_LL_Host___CHANNEL_PACKET_ID_SETUP
#define USB_Host_Pipes___PID_DATA0						USB_LL_Host___CHANNEL_PACKET_ID_DATA_ZERO
#define USB_Host_Pipes___PID_DATA1						USB_LL_Host___CHANNEL_PACKET_ID_DATA_ONE
#define USB_Host_Pipes___PID_DATA2						USB_LL_Host___CHANNEL_PACKET_ID_DATA_TWO
#define USB_Host_Pipes___Callback_Parameters		 	uint8_t port_Number, uint8_t pipe_Number, void* context, uint8_t status, uint8_t* p_Buffer, uint32_t transfer_Size
typedef struct {
	uint8_t 	is_Allocated;
	void* 		context;
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
	void		(*callback)(USB_Host_Pipes___Callback_Parameters);
}USB_Host_Pipes___Pipe_TypeDef;

uint8_t USB_Host_Pipes___Create_Pipe	(uint8_t port_Number, uint8_t device_Address, void*	context, uint8_t pipe_Type, uint8_t pipe_Direction, uint8_t endpoint_Number, uint16_t max_Packet_Size, uint8_t* p_Buffer, uint32_t transfer_Length, uint8_t is_Odd_Frame, uint8_t is_Low_Speed, uint8_t multi_Count, uint8_t packet_ID, void callback(USB_Host_Pipes___Callback_Parameters));
void 	USB_Host_Pipes___Begin_Transfer	(uint8_t port_Number, uint8_t pipe_Number);
void 	USB_Host_Pipes___Push_Transfer	(uint8_t port_Number, uint8_t pipe_Number, uint8_t* p_Buffer, uint32_t transfer_Size);
void 	USB_Host_Pipes___Process_Pipes	(uint8_t port_Number);
uint8_t USB_Host_Pipes___Get_Current_Packet_ID(uint8_t port_Number, uint8_t pipe_Number);
uint8_t USB_Host_Pipes___Get_Pipe_Direction(uint8_t port_Number, uint8_t pipe_Number);

#endif /* CORE_INC_USB_HOST_PIPES_H_ */
