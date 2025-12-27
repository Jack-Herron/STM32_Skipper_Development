/*
 * DSI_LCD.c
 *
 *  Created on: Dec 22, 2025
 *      Author: jackh
 */

#include "stm32f4xx.h"
#include "DSI_LCD.h"
#include "Clock.h"

void DSI_LCD_Init(void)
{
	// Enable DSI clock
	RCC->APB2ENR |= RCC_APB2ENR_DSIEN;
	while (!(RCC->APB2ENR & RCC_APB2ENR_DSIEN)); 								// Wait for DSI clock to be enabled

	DSI->WRPCR = 0; 															// Reset DSI Wrapper

	// DSI Regulator enable
	DSI->WRPCR |= DSI_WRPCR_REGEN; 												// Enable DSI Wrapper Regulator
	while(!(DSI->WISR & DSI_WISR_RRS)); 										// Wait for Regulator Ready Interrupt Flag

	// DSI PLL Configuration
	DSI->WRPCR |= 	(PLLDSINDIV << 	DSI_WRPCR_PLL_NDIV_Pos) |					// Set PLL N Divider
					(PLLDSIIDF 	<< 	DSI_WRPCR_PLL_IDF_Pos) 	|					// Set PLL IDF);
					(PLLDSIODF 	<< 	DSI_WRPCR_PLL_ODF_Pos);						// Set PLL ODF
    DSI->WRPCR |= DSI_WRPCR_PLLEN; 												// Enable DSI PLL
    while(!(DSI->WISR & DSI_WISR_PLLLS)); 										// Wait for PLL Lock Interrupt Flag

    // DSI Host Configuration
    DSI->WPCR[0] 	|= (DSI_LCD___LANE_PERIODX4 << DSI_WPCR0_UIX4_Pos); 			// Set HS to LP time
    DSI->PCONFR 	|=((DSI_LCD___LANE_COUNT-1) << DSI_PCONFR_NL_Pos);				// Set number of lanes to 2
    DSI->CLCR 		|= 	DSI_CLCR_DPCC; 												// Enable D-PHY Clock HS mode
    DSI->CLCR 		&= ~DSI_CLCR_ACR; 												// Disable Automatic Clock control
    DSI->CLTCR 		= 	DSI_LCD___CLOCK_HS2LP_TIME 	<< DSI_CLTCR_HS2LP_TIME_Pos |	// Set HS to LP time for clock lanes
    					DSI_LCD___CLOCK_LP2HS_TIME 	<< DSI_CLTCR_LP2HS_TIME_Pos; 	// Set LP to HS time for clock lanes
	DSI->DLTCR 		= 	DSI_LCD___DATA_HS2LP_TIME 	<< DSI_DLTCR_HS2LP_TIME_Pos |	// Set HS to LP time for data lanes
						DSI_LCD___DATA_LP2HS_TIME 	<< DSI_DLTCR_LP2HS_TIME_Pos; 	// Set LP to HS time for data lanes
	DSI->PCONFR 	|= 	DSI_LCD___STOP_WAIT_TIME 	<< DSI_PCONFR_SW_TIME_Pos;		// Set the minimum wait period to request a high-speed transmission after the Stop state.
	DSI->CCR        =   DSI_LCD___TX_TIMEOUT_CLOCK_DIV << DSI_CCR_TOCKDIV_Pos	|	// Set the timeout clock division factor
						DSI_LCD___TX_ESCAPE_CLOCK_DIV << DSI_CCR_TXECKDIV_Pos;		// Set the timeout clock division factor
	DSI->VMCR       =   DSI_VMCR_LPCE | 											// Enable Low-power command enable
						DSI_VMCR_VMT0;										        // Set Video mode Type to non burst mode with sync events
	DSI->VPCR = 		800;
	DSI->VCCR = 		1;
	DSI->VNPCR = 		0;
	DSI->VLCR =         1733;										//?????? Check value
	DSI->VVSACR  = 		(4   << DSI_VVSACR_VSA_Pos);
	DSI->VVBPCR = 		(14  << DSI_VVBPCR_VBP_Pos);
	DSI->VVFPCR = 		(14  << DSI_VVFPCR_VFP_Pos);
	DSI->VVACR = 		(480 << DSI_VVACR_VA_Pos);

	DSI->VMCR |=        DSI_VMCR_PGE; 												// Enable Pattern Generator

	DSI->PCTLR |=       DSI_PCTLR_DEN; 												// Enable DSI digital section
	DSI->PCTLR |=       DSI_PCTLR_CKE; 												// Enable DSI clocks

    DSI -> CR 		|= DSI_CR_EN; // Enable DSI
}
