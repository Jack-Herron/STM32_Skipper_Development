/*
 * Skipper_Clock.h
 *
 *  Created on: Jun 23, 2024
 *      Author: Jack Herron
 */

#ifndef DRIVERS_SKIPPER_DRIVERS_INC_SKIPPER_CLOCK_H_
#define DRIVERS_SKIPPER_DRIVERS_INC_SKIPPER_CLOCK_H_

#define Skipper_Clock___HSE_FREQUENCY					16000000
#define Skipper_Clock___HSI_FREQUENCY					16000000

#define Skipper_Clock___PLL_USE_HSE						0
#define Skipper_Clock___PLL_USE_HSI						1

#define Skipper_Clock___MUX_USE_HSE						0
#define Skipper_Clock___MUX_USE_HSI						1
#define Skipper_Clock___MUX_USE_PLL						2

// PLL INPUT SELECTION 	-- UN-COMMMENT SELECTION

#define Skipper_Clock___PLL_SOURCE						Skipper_Clock___PLL_USE_HSE

// ------------------------------------------------------------------

// MUX INPUT SELECTION

#define Skipper_Clock___MUX_SOURCE						Skipper_Clock___MUX_USE_PLL

// ------------------------------------------------------------------

// PLL CONFIGURATION 	-- ENTER DESIRED VALUES

#define Skipper_Clock___PLL_M							8					// 2  - 63
#define Skipper_Clock___PLL_N							72					// 50 - 432
#define Skipper_Clock___PLL_P							2					// 2, 4, 6, 8
#define Skipper_Clock___PLL_Q							3					// 2  - 15

// ------------------------------------------------------------------

// PRESCALER CONFIGURATION 	-- ENTER DESIRED VALUES

#define Skipper_Clock___AHB_PRESCALER					1					// 1, 2, 4, 8, 16, 64, 128, 256, 512
#define Skipper_Clock___APB1_PRESCALER					2					// 1, 2, 4, 8, 16
#define Skipper_Clock___APB2_PRESCALER					1					// 1, 2, 4, 8, 16
#define Skipper_Clock___CORTEX_SYSTEM_TIMER_PRESCALER	1					// 1, 8

// ------------------------------------------------------------------

#if Skipper_Clock___PLL_SOURCE == Skipper_Clock___PLL_USE_HSE
	#define Skipper_Clock___PLL_SOURCE_FREQUENCY 	Skipper_Clock___HSE_FREQUENCY
#endif

#if Skipper_Clock___PLL_SOURCE == Skipper_Clock___PLL_USE_HSI
	#define Skipper_Clock___PLL_SOURCE_FREQUENCY 	Skipper_Clock___HSI_FREQUENCY
#endif

#if (!Skipper_Clock___PLL_SOURCE == Skipper_Clock___PLL_USE_HSE) && (!Skipper_Clock___PLL_SOURCE == Skipper_Clock___PLL_USE_HSI)
	#error "PLL source selection out of bounds"
#endif

#if ((Skipper_Clock___PLL_M <= 63) && (Skipper_Clock___PLL_M >= 2))
	#define Skipper_Clock___PLL_M_OUTPUT_FREQUENCY Skipper_Clock___PLL_SOURCE_FREQUENCY / Skipper_Clock___PLL_M
	#if ((Skipper_Clock___PLL_M_OUTPUT_FREQUENCY < 950000) || (Skipper_Clock___PLL_M_OUTPUT_FREQUENCY > 2100000))
		#error "Skipper_Clock___PLL_M_OUTPUT_FREQUENCY must be >= 0.95 Mhz and be <= 2.1 Mhz"
	#endif
#else
	#error "Skipper_Clock_PLL_M value out of bounds"
#endif

#if ((Skipper_Clock___PLL_N <= 432) && (Skipper_Clock___PLL_N >= 50))
	#define Skipper_Clock___PLL_N_OUTPUT_FREQUENCY Skipper_Clock___PLL_M_OUTPUT_FREQUENCY * Skipper_Clock___PLL_N
	#if ((Skipper_Clock___PLL_N_OUTPUT_FREQUENCY < 100000000) || (Skipper_Clock___PLL_N_OUTPUT_FREQUENCY > 432000000))
		#error "Skipper_Clock___PLL_N_OUTPUT_FREQUENCY must be >= 100 Mhz and be <= 432 Mhz"
	#endif
#else
	#error "Skipper_Clock_PLL_N value out of bounds"
#endif

#if ((Skipper_Clock___PLL_P == 2) || (Skipper_Clock___PLL_P == 4) || (Skipper_Clock___PLL_P == 6) || (Skipper_Clock___PLL_P == 8))
	#define Skipper_Clock___PLL_P_OUTPUT_FREQUENCY Skipper_Clock___PLL_N_OUTPUT_FREQUENCY / Skipper_Clock___PLL_P
	#if ((Skipper_Clock___PLL_P_OUTPUT_FREQUENCY < 24000000) || (Skipper_Clock___PLL_P_OUTPUT_FREQUENCY > 168000000))
		#error "Skipper_Clock___PLL_P_OUTPUT_FREQUENCY must be >= 24 Mhz and be <= 168 Mhz"
	#endif
#else
	#error "Skipper_Clock_PLL_P value out of bounds"
#endif

#if ((Skipper_Clock___PLL_Q <= 15) && (Skipper_Clock___PLL_Q >= 2))
	#define Skipper_Clock___PLL_Q_OUTPUT_FREQUENCY Skipper_Clock___PLL_N_OUTPUT_FREQUENCY / Skipper_Clock___PLL_Q
	#if (Skipper_Clock___PLL_Q_OUTPUT_FREQUENCY > 75000000)
		#error "Skipper_Clock___PLL_Q_OUTPUT_FREQUENCY must be <= 75 Mhz"
	#endif
#else
	#error "Skipper_Clock_PLL_Q value out of bounds"
#endif

#if Skipper_Clock___MUX_SOURCE == Skipper_Clock___MUX_USE_HSI
	#define Skipper_Clock___MUX_OUTPUT_FREQUENCY 			Skipper_Clock___HSI_FREQUENCY
#endif

#if Skipper_Clock___MUX_SOURCE == Skipper_Clock___MUX_USE_HSE
	#define Skipper_Clock___MUX_OUTPUT_FREQUENCY 			Skipper_Clock___HSE_FREQUENCY
#endif

#if Skipper_Clock___MUX_SOURCE == Skipper_Clock___MUX_USE_PLL
	#define Skipper_Clock___MUX_OUTPUT_FREQUENCY 			Skipper_Clock___PLL_P_OUTPUT_FREQUENCY
#endif

#if (!Skipper_Clock___MUX_SOURCE == Skipper_Clock___MUX_USE_PLL) && (!Skipper_Clock___MUX_SOURCE == Skipper_Clock___MUX_USE_HSI) && (!Skipper_Clock___MUX_SOURCE == Skipper_Clock___MUX_USE_HSE)
	#error "MUX selection out of bounds"
#endif

#if ((Skipper_Clock___AHB_PRESCALER == 1) || (Skipper_Clock___AHB_PRESCALER == 2) || (Skipper_Clock___AHB_PRESCALER == 4) || (Skipper_Clock___AHB_PRESCALER == 8) || (Skipper_Clock___AHB_PRESCALER == 16) || (Skipper_Clock___AHB_PRESCALER == 64) || (Skipper_Clock___AHB_PRESCALER == 128) || (Skipper_Clock___AHB_PRESCALER == 256) || (Skipper_Clock___AHB_PRESCALER == 512))
	#define Skipper_Clock___AHB_PRESCALER_OUTPUT_FREQUENCY Skipper_Clock___MUX_OUTPUT_FREQUENCY / Skipper_Clock___AHB_PRESCALER
	#if ((Skipper_Clock___AHB_PRESCALER_OUTPUT_FREQUENCY > 168000000) || (Skipper_Clock___AHB_PRESCALER_OUTPUT_FREQUENCY < 14200000))
		#error "Skipper_Clock___AHB_PRESCALER_OUTPUT_FREQUENCY must be <= 168 Mhz and >= 14.2 Mhz"
	#endif
#else
	#error "Skipper_Clock___AHB_PRESCALER value out of bounds"
#endif

#if ((Skipper_Clock___APB1_PRESCALER == 1) || (Skipper_Clock___APB1_PRESCALER == 2) || (Skipper_Clock___APB1_PRESCALER == 4) || (Skipper_Clock___APB1_PRESCALER == 8) || (Skipper_Clock___APB1_PRESCALER == 16))
	#define Skipper_Clock___APB1_PRESCALER_OUTPUT_FREQUENCY Skipper_Clock___AHB_PRESCALER_OUTPUT_FREQUENCY / Skipper_Clock___APB1_PRESCALER
	#if (Skipper_Clock___APB1_PRESCALER_OUTPUT_FREQUENCY > 42000000)
		#error "Skipper_Clock___APB1_PRESCALER_OUTPUT_FREQUENCY must be <= 42 Mhz"
	#endif
#else
	#error "Skipper_Clock___APB1_PRESCALER value out of bounds"
#endif

#if ((Skipper_Clock___APB2_PRESCALER == 1) || (Skipper_Clock___APB2_PRESCALER == 2) || (Skipper_Clock___APB2_PRESCALER == 4) || (Skipper_Clock___APB2_PRESCALER == 8) || (Skipper_Clock___APB2_PRESCALER == 16))
	#define Skipper_Clock___APB2_PRESCALER_OUTPUT_FREQUENCY Skipper_Clock___AHB_PRESCALER_OUTPUT_FREQUENCY / Skipper_Clock___APB2_PRESCALER
	#if (Skipper_Clock___APB2_PRESCALER_OUTPUT_FREQUENCY > 84000000)
		#error "Skipper_Clock___APB2_PRESCALER_OUTPUT_FREQUENCY must be <= 84 Mhz"
	#endif
#else
	#error "Skipper_Clock___APB2_PRESCALER value out of bounds"
#endif

#if (Skipper_Clock___APB1_PRESCALER == 1)
	#define Skipper_Clock___APB1_TIMER_MULTIPLIER_OUTPUT_FREQUENCY Skipper_Clock___APB1_PRESCALER_OUTPUT_FREQUENCY
#else
	#define Skipper_Clock___APB1_TIMER_MULTIPLIER_OUTPUT_FREQUENCY (Skipper_Clock___APB1_PRESCALER_OUTPUT_FREQUENCY * 2)
#endif

#if (Skipper_Clock___APB2_PRESCALER == 1)
	#define Skipper_Clock___APB2_TIMER_MULTIPLIER_OUTPUT_FREQUENCY Skipper_Clock___APB2_PRESCALER_OUTPUT_FREQUENCY
#else
	#define Skipper_Clock___APB2_TIMER_MULTIPLIER_OUTPUT_FREQUENCY (Skipper_Clock___APB2_PRESCALER_OUTPUT_FREQUENCY * 2)
#endif

#if ((Skipper_Clock___CORTEX_SYSTEM_TIMER_PRESCALER == 1) || (Skipper_Clock___CORTEX_SYSTEM_TIMER_PRESCALER == 8))
	#define Skipper_Clock___CORTEX_SYSTEM_TIMER_PRESCALER_OUTPUT_FREQUENCY Skipper_Clock___AHB_PRESCALER_OUTPUT_FREQUENCY / Skipper_Clock___CORTEX_SYSTEM_TIMER_PRESCALER
#else
	#error "Skipper_Clock___CORTEX_SYSTEM_TIMER_PRESCALER value out of bounds"
#endif

#define Skipper_Clock___USB_FREQUENCY					Skipper_Clock___PLL_Q_OUTPUT_FREQUENCY
#define Skipper_Clock___APB2_TIMER_FREQUENCY			Skipper_Clock___APB2_TIMER_MULTIPLIER_OUTPUT_FREQUENCY
#define Skipper_Clock___APB2_PERIPHERAL_FREQUENCY		Skipper_Clock___APB2_PRESCALER_OUTPUT_FREQUENCY
#define Skipper_Clock___APB1_TIMER_FREQUENCY			Skipper_Clock___APB1_TIMER_MULTIPLIER_OUTPUT_FREQUENCY
#define Skipper_Clock___APB1_PERIPHERAL_FREQUENCY		Skipper_Clock___APB1_PRESCALER_OUTPUT_FREQUENCY
#define Skipper_Clock___FCLK_FREQUENCY					Skipper_Clock___AHB_PRESCALER_OUTPUT_FREQUENCY
#define Skipper_Clock___CORTEX_SYSTEM_FREQUENCY			Skipper_Clock___CORTEX_SYSTEM_TIMER_PRESCALER_OUTPUT_FREQUENCY
#define Skipper_Clock___AHB_FREQUENCY					Skipper_Clock___AHB_PRESCALER_OUTPUT_FREQUENCY
#define Skipper_Clock___ETHERNET_PTP_FREQUENCY			Skipper_Clock___AHB_PRESCALER_OUTPUT_FREQUENCY


#define Skipper_Clock___APB_DIV_1				1
#define Skipper_Clock___APB_DIV_2				2
#define Skipper_Clock___APB_DIV_4				4
#define Skipper_Clock___APB_DIV_8				8
#define Skipper_Clock___APB_DIV_16				16

#define Skipper_Clock___RCC_CFGR_APB_DIV_1		0
#define Skipper_Clock___RCC_CFGR_APB_DIV_2		4
#define Skipper_Clock___RCC_CFGR_APB_DIV_4		5
#define Skipper_Clock___RCC_CFGR_APB_DIV_8		6
#define Skipper_Clock___RCC_CFGR_APB_DIV_16		7

#define Skipper_Clock___AHB_DIV_1				1
#define Skipper_Clock___AHB_DIV_2				2
#define Skipper_Clock___AHB_DIV_4				4
#define Skipper_Clock___AHB_DIV_8				8
#define Skipper_Clock___AHB_DIV_16				16
#define Skipper_Clock___AHB_DIV_64				64
#define Skipper_Clock___AHB_DIV_128				128
#define Skipper_Clock___AHB_DIV_256				256
#define Skipper_Clock___AHB_DIV_512				512

#define Skipper_Clock___RCC_CFGR_AHB_DIV_1		0
#define Skipper_Clock___RCC_CFGR_AHB_DIV_2		8
#define Skipper_Clock___RCC_CFGR_AHB_DIV_4		9
#define Skipper_Clock___RCC_CFGR_AHB_DIV_8		10
#define Skipper_Clock___RCC_CFGR_AHB_DIV_16		11
#define Skipper_Clock___RCC_CFGR_AHB_DIV_64		12
#define Skipper_Clock___RCC_CFGR_AHB_DIV_128	13
#define Skipper_Clock___RCC_CFGR_AHB_DIV_256	14
#define Skipper_Clock___RCC_CFGR_AHB_DIV_512	15

void Skipper_Clock___Init();

#endif /* DRIVERS_SKIPPER_DRIVERS_INC_SKIPPER_CLOCK_H_ */
