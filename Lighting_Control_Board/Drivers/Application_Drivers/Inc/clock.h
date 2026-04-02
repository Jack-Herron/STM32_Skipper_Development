/*
 * clock.h
 *
 *  Created on: Mar 14, 2026
 *      Author: jackh
 */

#ifndef APPLICATION_DRIVERS_INC_CLOCK_H_
#define APPLICATION_DRIVERS_INC_CLOCK_H_

void clock___Init();
void clock___Delay_ms(uint32_t delay);
uint32_t clock___Get_Millis();

#endif /* APPLICATION_DRIVERS_INC_CLOCK_H_ */
