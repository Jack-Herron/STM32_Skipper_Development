/*
 * CCS.h
 *
 *  Created on: Mar 15, 2026
 *      Author: jackh
 */

#ifndef APPLICATION_DRIVERS_INC_CCS_H_
#define APPLICATION_DRIVERS_INC_CCS_H_

#include "main.h"

#define CCS___MAX_CURRENT 		0.250f
#define CCS___NUM_CHANNELS		8
#define CCS___WHITE_RIGHT		WHITE_RIGHT
#define CCS___WHITE_LEFT		WHITE_LEFT
#define CCS___RED_RIGHT			RED_RIGHT
#define CCS___RED_LEFT			RED_LEFT
#define CCS___LIME				LIME
#define CCS___PURPLE			PURPLE
#define CCS___FAR_RED			FAR_RED

#define CCS___OVERHEAD_VOLTAGE 	2.0f		// maintain 2V across CC FET

#define CCS___RESET_VALUE		0.0f

void 	CCS___Init();
void 	CCS___Write_Channel(uint8_t channel, float C_Set);
float 	CCS___Get_Channel_Current(uint8_t channel);


#endif /* APPLICATION_DRIVERS_INC_CCS_H_ */
