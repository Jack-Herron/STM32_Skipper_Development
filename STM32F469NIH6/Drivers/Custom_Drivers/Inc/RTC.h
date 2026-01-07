/*
 * RTC.h
 *
 *  Created on: Jan 6, 2026
 *      Author: Jack Herron
 */

#ifndef CUSTOM_DRIVERS_INC_RTC_H_
#define CUSTOM_DRIVERS_INC_RTC_H_

typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t pm;
} RTC___Time_TypeDef;

typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
} RTC___Date_TypeDef;

void RTC___Init(void);
RTC___Time_TypeDef RTC___Get_Time(void);
void RTC___Set_Time_And_Date(RTC___Time_TypeDef time, RTC___Date_TypeDef date);

#endif /* CUSTOM_DRIVERS_INC_RTC_H_ */
