/*
 * ADC.h
 *
 *  Created on: Mar 19, 2026
 *      Author: Jack Herron
 */

#ifndef APPLICATION_DRIVERS_INC_ADC_H_
#define APPLICATION_DRIVERS_INC_ADC_H_

#define ADC___WHITE_RIGHT		WHITE_RIGHT
#define ADC___WHITE_LEFT		WHITE_LEFT
#define ADC___RED_RIGHT			RED_RIGHT
#define ADC___RED_LEFT			RED_LEFT
#define ADC___LIME				LIME
#define ADC___PURPLE			PURPLE
#define ADC___FAR_RED			FAR_RED
#define ADC___BOOST				8

void ADC___Init(void);
float ADC___Get_Voltage(uint8_t channel);

#endif /* APPLICATION_DRIVERS_INC_ADC_H_ */
