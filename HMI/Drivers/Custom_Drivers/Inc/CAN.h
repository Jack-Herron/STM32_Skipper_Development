/*
 * CAN.h
 *
 *  Created on: Mar 15, 2026
 *      Author: jackh
 */

#ifndef CUSTOM_DRIVERS_INC_CAN_H_
#define CUSTOM_DRIVERS_INC_CAN_H_

typedef struct
{
	uint16_t ID;
	uint8_t* data;
	uint8_t data_Length;

}CAN___Transmit_TypeDef;

typedef struct
{
	uint16_t ID;
	uint8_t* data;
	uint8_t data_Length;
}CAN___Receive_TypeDef;

void CAN___Init();
void CAN___Accept_All_Messages();
void CAN___Transmit(CAN___Transmit_TypeDef Payload);
void CAN___Set_RX_Callback(void (*callback)(CAN___Receive_TypeDef));


#endif /* CUSTOM_DRIVERS_INC_CAN_H_ */
