/*
 * USB_LL_Host.h
 *
 *  Created on: Jun 26, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_HOST_H_
#define INC_USB_LL_HOST_H_

#define USB_LL_Host___NON_PERIODIC_TX_FIFO						0x00
#define USB_LL_Host___PERIODIC_TX_FIFO							0x01
#define USB_LL_Host___HOST_RESET_DELAY							0x19
#define USB_LL_Host___HPRT_RC_W1_BITS							0x2e
#define USB_LL_Host___HPRT_LOW_SPEED_VALUE						0x02
#define USB_LL_Host___HPRT_FULL_SPEED_VALUE						0x01
#define USB_LL_Host___HPRT_HIGH_SPEED_VALUE						0x00
#define USB_LL_HOST___CLOCK_CYCLES_IN_ONE_MILLISECOND_FRAME		0xbb80

uint16_t 	USB_LL_Host___Host_Get_Frame_Number			(uint8_t port_Number);
uint32_t 	USB_LL_Host___Host_Get_FIFO_Space_Available	(uint8_t port_Number, uint8_t FIFO_Type);
void 		USB_LL_Host___Host_Set_FIFO_Size			(uint8_t port_Number, uint32_t RX_FIFO_Depth, uint32_t non_Periodic_TX_FIFO_Depth, uint32_t periodic_TX_FIFO_Depth);
uint32_t* 	USB_LL_Host___Channel_Get_Fifo_Pointer		(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Halt					(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Halt_And_Wait			(uint8_t port_Number, uint8_t channel_Number);
uint8_t 	USB_LL_Host___Channel_Get_Current_PID		(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Set_Interrupts		(uint8_t port_Number, uint8_t channel_Number);
void 		USB_LL_Host___Channel_Set_Characteristics	(uint8_t port_Number, uint8_t channel_Number, uint32_t channel_Characteristics);
void 		USB_LL_Host___Channel_Load_HCTSIZ			(uint8_t port_Number, uint8_t channel_Number, uint32_t transfer_Size_In_Bytes, uint32_t packet_Count, uint8_t packet_ID);
void 		USB_LL_Host___Reset_Port					(uint8_t port_Number);

#endif /* INC_USB_LL_HOST_H_ */
