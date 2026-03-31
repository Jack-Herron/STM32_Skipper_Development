/*
 * CAN.h
 *
 *  Created on: Mar 15, 2026
 *      Author: jackh
 */

#ifndef APPLICATION_DRIVERS_INC_CAN_H_
#define APPLICATION_DRIVERS_INC_CAN_H_

#define CAN___ENABLE_STDIO	1
#define CAN___STDIO_CAN_ID	0x700

typedef struct
{
	uint16_t ID;
	uint8_t data[8];
	uint8_t data_Length;

}CAN_Tansmit_TypeDef;

typedef struct
{
	uint16_t ID;
	uint8_t* data;
	uint8_t data_Length;

}CAN___Receive_TypeDef;

void CAN___Init();
void CAN___Accept_All_Messages();
void CAN___Transmit(CAN_Tansmit_TypeDef Payload);
void CAN___Set_RX_Callback(void (*callback)(CAN___Receive_TypeDef));

#endif /* APPLICATION_DRIVERS_INC_CAN_H_ */
