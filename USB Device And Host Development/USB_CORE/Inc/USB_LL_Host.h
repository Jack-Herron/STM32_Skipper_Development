/*
 * USB_LL_Host.h
 *
 *  Created on: Jun 26, 2024
 *      Author: Jack Herron
 */

#ifndef INC_USB_LL_HOST_H_
#define INC_USB_LL_HOST_H_

uint16_t USB_LL_Hardware___Host_Get_Frame_Number(uint8_t port_Number);
uint32_t USB_LL_Hardware___Host_Get_FIFO_Space_Available(uint8_t port_Number, uint8_t FIFO_Type);
void USB_LL_Hardware___Host_Set_FIFO_Size(uint8_t port_Number, uint32_t RX_FIFO_Depth, uint32_t non_Periodic_TX_FIFO_Depth, uint32_t periodic_TX_FIFO_Depth);
uint32_t* USB_LL_Hardware___Channel_Get_Fifo_Pointer(uint8_t port_Number, uint8_t channel_Number);
void USB_LLHardware___Channel_Halt(uint8_t port_Number, uint8_t channel_Number);
void USB_LL_Hardware___Channel_Halt_And_Wait(uint8_t port_Number, uint8_t channel_Number);
uint8_t USB_LL_Hardware___Channel_Get_Current_PID(uint8_t port_Number, uint8_t channel_Number);
void USB_LL_Hardware___Channel_Set_Interrupts(uint8_t port_Number,uint8_t channel_Number);
void USB_LL_Hardware___Channel_Set_Characteristics(uint8_t port_Number, uint8_t channel_Number, uint32_t channel_Characteristics);
void USB_LL_Hardware___Channel_Load_HCTSIZ(uint8_t port_Number, uint8_t channel_Number, uint32_t transfer_Size_In_Bytes, uint32_t packet_Count, uint8_t packet_ID);


#endif /* INC_USB_LL_HOST_H_ */
