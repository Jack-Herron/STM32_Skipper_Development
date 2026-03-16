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
	uint8_t data[8];
	uint8_t data_Length;

}CAN_Tansmit_TypeDef;

void CAN___Init();
void CAN___Accept_All_Messages();
void CAN___Transmit(CAN_Tansmit_TypeDef Payload);



#endif /* CUSTOM_DRIVERS_INC_CAN_H_ */
