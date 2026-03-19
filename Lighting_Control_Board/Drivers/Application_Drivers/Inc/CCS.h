/*
 * CCS.h
 *
 *  Created on: Mar 15, 2026
 *      Author: jackh
 */

#ifndef APPLICATION_DRIVERS_INC_CCS_H_
#define APPLICATION_DRIVERS_INC_CCS_H_

#define CCS___MAX_CURRENT 	0.250f

void CCS___Init();
void CCS___Write_Channel(uint8_t channel, float C_Set);


#endif /* APPLICATION_DRIVERS_INC_CCS_H_ */
