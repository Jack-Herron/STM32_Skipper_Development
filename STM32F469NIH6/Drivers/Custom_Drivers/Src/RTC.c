/*
 * RTC.c
 *
 *  Created on: Jan 6, 2026
 *      Author: Jack Herron
 */


#include "STM32F4xx.h"
#include "Clock.h"
#include "RTC.h"

void RTC___Init(void)
{
	Clock___RTC_Init(1);
}

RTC___Time_TypeDef RTC___Get_Time(void)
{
	RTC___Time_TypeDef ret = {0};
	uint32_t TR = (uint32_t)RTC->TR;

	ret.hour = ((TR & RTC_TR_HT_Msk) >> RTC_TR_HT_Pos) * 10 + ((TR & RTC_TR_HU_Msk) >> RTC_TR_HU_Pos);
	ret.minute = ((TR & RTC_TR_MNT_Msk) >> RTC_TR_MNT_Pos) * 10 + ((TR & RTC_TR_MNU_Msk) >> RTC_TR_MNU_Pos);
	ret.second = ((TR & RTC_TR_ST_Msk) >> RTC_TR_ST_Pos) * 10 + ((TR & RTC_TR_SU_Msk) >> RTC_TR_SU_Pos);
	ret.pm = ((TR & RTC_TR_PM_Msk) >> RTC_TR_PM_Pos);

	return(ret);
}

//TODO make this have a loop timeout
void RTC___Set_Time_And_Date(RTC___Time_TypeDef time, RTC___Date_TypeDef date)
{
	PWR -> CR 		|= PWR_CR_DBP;							// Unblock backupdomain writes

	RTC->WPR = 0xCA;										// key to unlock RTC write (see ref manual)
	RTC->WPR = 0x53;

	RTC->ISR |= RTC_ISR_INIT;								// enter init mode
	while(!((RTC->ISR) & RTC_ISR_INITF));					// wait to enter init

	uint32_t TR = 0;

	TR = 	((time.hour 	/10) << RTC_TR_HT_Pos) 	|		// set time
			((time.hour 	%10) << RTC_TR_HU_Pos) 	|
			((time.minute 	/10) << RTC_TR_MNT_Pos) |
			((time.minute 	%10) << RTC_TR_MNU_Pos) |
			((time.second 	/10) << RTC_TR_ST_Pos)  |
			((time.second 	%10) << RTC_TR_SU_Pos)  |
			((time.pm)           << RTC_TR_PM_Pos)	;

	RTC->TR = TR;

	uint32_t DR = 0;

	DR = 	((date.year 	/10) << RTC_DR_YT_Pos) |		// set date
			((date.year 	%10) << RTC_DR_YU_Pos) |
			((date.month 	/10) << RTC_DR_MT_Pos) |
			((date.month 	%10) << RTC_DR_MU_Pos) |
			((date.day 		/10) << RTC_DR_DT_Pos) |
			((date.day 		%10) << RTC_DR_DU_Pos) ;

	RTC->DR = DR;

	RTC->ISR &= ~RTC_ISR_INIT;								// exit init mode

	RTC->WPR = 0x00;										// re-lock

	PWR -> CR 		|= PWR_CR_DBP;							// Reblock
}
