/*
 * USB_LL_Host.h
 *
 *  Created on: Jun 26, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_HOST_H_
#define INC_USB_LL_HOST_H_

#define USB_LL_Host___QUICK_DIV_ROOF(number, diviser) 			((number + (diviser-1)) / diviser)
#define USB_LL_Host___GET_MAX(a,b)								(((a) > (b)) ? (a) : (b))
#define USB_LL_Host___GET_MIN(a,b)								(((a) > (b)) ? (b) : (a))

#define USB_LL_Host___MAX_NUMBER_OF_CHANNELS					0x0c
#define USB_LL_Host___NON_PERIODIC_TX_FIFO						0x00
#define USB_LL_Host___PERIODIC_TX_FIFO							0x01
#define USB_LL_Host___HOST_RESET_DELAY							0x19
#define USB_LL_Host___HPRT_RC_W1_BITS							0x2e
#define USB_LL_Host___HPRT_LOW_SPEED_VALUE						0x02
#define USB_LL_Host___HPRT_FULL_SPEED_VALUE						0x01
#define USB_LL_Host___HPRT_HIGH_SPEED_VALUE						0x00
#define USB_LL_HOST___CLOCK_CYCLES_IN_ONE_MILLISECOND_FRAME		0xbb80
#define USB_LL_Host___RX_PACKET_STATUS_DATA_PACKET_RECIEVED		0x02
#define USB_LL_Host___EXIT_SUCCESS								 (0x01)
#define USB_LL_Host___EXIT_FAILURE_BUFFER_OVERFLOW				-(0x02)
#define USB_LL_Host___PORT_INTERRUPTS_MASK						0x000a
#define USB_LL_Host___CHANNEL_INTERRUPTS_MASK					0x06bb
#define USB_LL_Host___CHANNEL_PACKET_ID_DATA_ZERO				0x00
#define USB_LL_Host___CHANNEL_PACKET_ID_DATA_ONE				0x02
#define USB_LL_Host___CHANNEL_PACKET_ID_DATA_TWO				0x01
#define USB_LL_Host___CHANNEL_PACKET_ID_SETUP					0x03
#define USB_LL_Host___TRANSFER_DIRECTION_OUT					0x00
#define USB_LL_Host___TRANSFER_DIRECTION_IN						0x01


typedef struct {
	uint32_t 									transfer_Size;
	uint32_t 									transfer_Progress;
	uint16_t									packet_Size;
	uint16_t									retries_Remaining;
	uint8_t  									retry_After_Halt;
	uint8_t 									transfer_Complete;
	uint8_t										transfer_Complete_Flag;
	uint8_t										exit_Status;
	uint8_t										transfer_Direction;
	uint8_t* 									p_Buffer;
	uint32_t 									buffer_Fill_Level;
} USB_LL_Host___Channel_Status_TypeDef;

typedef struct {
	uint8_t                                             is_Start_Of_Frame;
	uint8_t												is_Root_Device_Connection_Status_Change;
	uint8_t 											is_Root_Device_Connected;
	uint8_t 											is_Root_Device_Disconnected;
	uint8_t 											root_Device_Speed;
	uint8_t												all_Channels_Status_Change_Flag;
	USB_LL_Host___Channel_Status_TypeDef				channel_Status[USB_LL_Host___MAX_NUMBER_OF_CHANNELS];
} USB_LL_Host___Status_TypeDef;

uint16_t 	USB_LL_Host___Host_Get_Frame_Number				(uint8_t port_Number);
uint32_t 	USB_LL_Host___Host_Get_FIFO_Space_Available		(uint8_t port_Number, uint8_t FIFO_Type);
void 		USB_LL_Host___Host_Set_FIFO_Size				(uint8_t port_Number, uint32_t RX_FIFO_Depth, uint32_t non_Periodic_TX_FIFO_Depth, uint32_t periodic_TX_FIFO_Depth);
uint32_t* 	USB_LL_Host___Channel_Get_Fifo_Pointer			(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Halt						(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Halt_And_Wait				(uint8_t port_Number, uint8_t channel_Number);
uint8_t 	USB_LL_Host___Channel_Get_Current_PID			(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Set_Interrupts			(uint8_t port_Number, uint8_t channel_Number);
int8_t 		USB_LL_Host___Channel_RX_POP					(uint8_t port_Number, uint8_t channel_Number, uint32_t RX_Status);
void 		USB_LL_Host___Setup_Channel						(uint8_t port_Number, uint8_t channel_Number, uint16_t max_Packet_Size, uint8_t endpoint_Number, uint8_t endpoint_Direction, uint8_t low_Speed_Device, uint8_t endpoint_Type, uint8_t multi_Count, uint8_t device_Address, uint8_t odd_Frame);
void 		USB_LL_Host___Channel_Load_HCTSIZ				(uint8_t port_Number, uint8_t channel_Number, uint32_t transfer_Size_In_Bytes, uint32_t packet_Count, uint8_t packet_ID);
void 		USB_LL_Host___Channel_Enable					(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Reset_Port						(uint8_t port_Number);
uint32_t 	USB_LL_Host___Channel_Get_Transfer_Progress		(uint8_t port_Number, uint8_t channel_Number);
uint8_t* 	USB_LL_Host___Channel_Get_Buffer_Pointer		(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Begin_Transfer_Out		(uint8_t port_Number, uint8_t channel_Number);
uint16_t 	USB_LL_Host___Channel_Get_Retries_Remaining		(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Retry_Transfer_Out		(uint8_t port_Number, uint8_t channel_Number);
uint8_t 	USB_LL_Host___Channel_Get_Transfer_Direction	(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Out_Packet_Acknowledged	(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_In_Packet_Acknowledged	(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Setup_Buffer				(uint8_t port_Number, uint8_t channel_Number, uint8_t *buffer_Pointer, uint32_t transfer_Size);
void 		USB_LL_Host___Channel_Begin_Transfer_In			(uint8_t port_Number, uint8_t channel_Number);
uint8_t 	USB_LL_Host___Channel_Get_Current_Packet_ID		(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Set_Number_Of_Retries		(uint8_t port_Number, uint8_t channel_Number, uint16_t number_Of_Retries);
void 		USB_LL_Host___Channel_Retry_Transfer_In			(uint8_t port_Number, uint8_t channel_Number);
uint8_t 	USB_LL_Host___Channel_Get_Retry_After_Halt		(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Set_Retry_After_Halt		(uint8_t port_Number, uint8_t channel_Number, uint8_t value);
void 		USB_LL_Host___Set_FIFO_Size						(uint8_t port_Number, uint32_t RX_FIFO_Depth, uint32_t non_Periodic_TX_FIFO_Depth, uint32_t periodic_TX_FIFO_Depth);
void 		USB_LL_Host___Channel_Disable					(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Set_Retries_Remaining		(uint8_t port_Number, uint8_t channel_Number, uint16_t number_Of_Retries);




#include "../Inc/USB_LL_Host.h"

#ifdef USB_LL_Host___DEBUG_ON
	#define USB_LL_Host___DPRINTF 1
#else
	#define USB_LL_Host___DPRINTF 0
#endif


#define USB_LL_Host___Debug_Log(...) if (USB_LL_Host___DPRINTF) printf(__VA_ARGS__)

#define USB_LL_Host___LOW_SPEED_VALUE							0x00
#define USB_LL_Host___FULL_SPEED_VALUE							0x01
#define USB_LL_Host___HIGH_SPEED_VALUE							0x02
#define USB_LL_Host___CHANNEL_STATUS_TRANSFER_COMPLETE			0x00
#define USB_LL_Host___CHANNEL_STATUS_TRANSFER_FAILED_NAK			0x01
#define USB_LL_Host___CHANNEL_STATUS_TRANSFER_FAILED_STALL		0x02
#define USB_LL_Host___CHANNEL_STATUS_TRANSFER_FAILED_ERROR		0x03
#define USB_LL_Host___CHANNEL_STATUS_CHANNEL_HALTED				0x04



USB_LL_Host___Status_TypeDef* 	USB_LL_Host___Get_Host_Status							(uint8_t port_Number);
void 							USB_LL_Host___Channel_Interrupt_Handler					(uint8_t port_Number);
void 							USB_LL_Host___Port_Interrupt_Handler					(uint8_t port_Number);
void 							USB_LL_Host___Packet_Received							(uint8_t port_Number);
uint8_t 						USB_LL_Host___Is_Root_Device_Connection_Status_Change	(uint8_t port_Number);
uint8_t 						USB_LL_Host___Is_Root_Device_Connected					(uint8_t port_Number);
uint8_t 						USB_LL_Host___Is_Root_Device_Disconnected				(uint8_t port_Number);
uint8_t 						USB_LL_Host___Get_Root_Device_Speed						(uint8_t port_Number);
void 							USB_LL_Host___Clear_Connection_Status_Change			(uint8_t port_Number);
uint8_t 						USB_LL_Host___Get_All_Channels_Status_Change_Flag		(uint8_t port_Number);
uint8_t 						USB_LL_Host___Get_Transfer_Complete_Flag				(uint8_t port_Number, uint8_t channel_Number);
void	 						USB_LL_Host___Clear_Transfer_Complete_Flag				(uint8_t port_Number, uint8_t channel_Number);
uint8_t 						USB_LL_Host___Get_Channel_Status						(uint8_t port_Number, uint8_t channel_Number);
void 							USB_LL_Host___Clear_All_Channels_Status_Change_Flag		(uint8_t port_Number);
void 							USB_LL_Host___Clear_Channel_Status_Change_Flag			(uint8_t port_Number,  uint8_t channel_Number);
void 							USB_LL_Host___Start_Of_Frame_Interrupt_Received			(uint8_t port_Number);
uint8_t 						USB_LL_Host___Is_Start_Of_Frame							(uint8_t port_Number);
void 							USB_LL_Host___Clear_Start_Of_Frame						(uint8_t port_Number);
uint8_t 						USB_LL_Host___Channel_Is_Busy							(uint8_t port_Number, uint8_t channel_Number);

#endif /* INC_USB_LL_HOST_H_ */
