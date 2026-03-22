/*
 * boost.h
 *
 *  Created on: Mar 14, 2026
 *      Author: jackh
 */

#ifndef APPLICATION_DRIVERS_INC_BOOST_H_
#define APPLICATION_DRIVERS_INC_BOOST_H_

#define DAC_CODE_ZEROSCALE	100
#define DAC_CODE_FULLSCALE	2482
#define DAC_SPAN			DAC_CODE_FULLSCALE - DAC_CODE_ZEROSCALE

#define V_FULLSCALE 		59.079f
#define V_ZEROSCALE 		29.791f
#define V_SPAN				(V_FULLSCALE - V_ZEROSCALE)

void boost___Init();
void boost___Set_Voltage(float V_Set);
void boost___Enable();

#endif /* APPLICATION_DRIVERS_INC_BOOST_H_ */
