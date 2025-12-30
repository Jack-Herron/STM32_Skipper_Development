/*
 * DSI_LCD.c
 *
 *  Created on: Dec 22, 2025
 *      Author: jackh
 */

#include "stm32f4xx.h"
#include "DSI_LCD.h"
#include "Clock.h"
#include "otm8009a.h"

void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams)
{
	switch(NbrParams)
	{
	case 0:
		DSI_LCD___Short_Write(0, DSI_LCD___DATA_TYPE_SHORT_WRITE_1P, pParams[0], pParams[1]);
		break;
	/*case 1:
		DSI_LCD___Short_Write(0, DSI_LCD___DATA_TYPE_SHORT_WRITE_1P, pParams[0], pParams[1]);
		break;*/
	default:
		DSI_LCD___Long_Write(0, DSI_LCD___DATA_TYPE_LONG_WRITE, pParams, NbrParams+1);
	}
}

void OTM8009A_IO_Delay(uint32_t Delay)
{
	for(uint32_t i = 0; i < (SystemCoreClock / 1000) * Delay; i++);		// crude delay
}

void DSI_LCD___Short_Write(uint32_t channel_ID, uint32_t data_Type, uint32_t data1, uint32_t data2)
{
	while (!(DSI->GPSR & DSI_GPSR_CMDFE)); 											// Wait until command FIFO is empty
	DSI->GHCR = (data_Type | (channel_ID << DSI_GHCR_VCID_Pos) | (data1 << DSI_GHCR_WCLSB_Pos) | (data2 << DSI_GHCR_WCMSB_Pos));
}

void DSI_LCD___Long_Write(uint32_t channel_ID, uint32_t data_Type, uint8_t *pdata, uint32_t size)
{
	uint8_t cmd = pdata[0];
	uint32_t payload_len = size; // include command byte

	while (!(DSI->GPSR & DSI_GPSR_CMDFE)); 									// Wait until command FIFO is empty



	for (uint32_t i = 0; i < payload_len; i += 4)
	{
		uint32_t b0 = (i + 0 < payload_len) ? ((i + 0 == 0) ? cmd : pdata[(i + 0)]) : 0;
		uint32_t b1 = (i + 1 < payload_len) ? ((i + 1 == 0) ? cmd : pdata[(i + 1)]) : 0;
		uint32_t b2 = (i + 2 < payload_len) ? ((i + 2 == 0) ? cmd : pdata[(i + 2)]) : 0;
		uint32_t b3 = (i + 3 < payload_len) ? ((i + 3 == 0) ? cmd : pdata[(i + 3)]) : 0;

		DSI->GPDR = (b0 << DSI_GPDR_DATA1_Pos)
				  | (b1 << DSI_GPDR_DATA2_Pos)
				  | (b2 << DSI_GPDR_DATA3_Pos)
				  | (b3 << DSI_GPDR_DATA4_Pos);
	}

	uint32_t wc_lsb = (payload_len & 0xFF);
	uint32_t wc_msb = ((payload_len >> 8) & 0xFF);

	DSI->GHCR = (data_Type)
	              | (channel_ID << DSI_GHCR_VCID_Pos)
	              | (wc_lsb << DSI_GHCR_WCLSB_Pos)
	              | (wc_msb << DSI_GHCR_WCMSB_Pos);
}

void DSI_LCD___Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN; 											// Enable GPIOH clock
	GPIOH->MODER |= GPIO_MODER_MODER7_0; 											// Set PH7 to Output mode
	GPIOH->ODR &= ~GPIO_ODR_OD7; 													// Set PH7 Low (LCD Reset active)
	for(uint32_t i = 0; i < (SystemCoreClock / 10); i++); 							// Delay 100ms
	GPIOH->ODR |= GPIO_ODR_OD7; 													// Set PH7 High (LCD Reset inactive)

	// Enable DSI clock
	RCC->APB2ENR |= RCC_APB2ENR_DSIEN;
	while (!(RCC->APB2ENR & RCC_APB2ENR_DSIEN)); 									// Wait for DSI clock to be enabled
	DSI->MCR &= ~DSI_MCR_CMDM; // enable video mode
	DSI->WRPCR = 0; 																// Reset DSI Wrapper

	// DSI Regulator enable
	DSI->WRPCR |= DSI_WRPCR_REGEN; 													// Enable DSI Wrapper Regulator
	while(!(DSI->WISR & DSI_WISR_RRS)); 											// Wait for Regulator Ready Interrupt Flag

	// DSI PLL Configuration
	DSI->WRPCR |= 	(PLLDSINDIV << 	DSI_WRPCR_PLL_NDIV_Pos) |						// Set PLL N Divider
					(PLLDSIIDF 	<< 	DSI_WRPCR_PLL_IDF_Pos) 	|						// Set PLL IDF);
					(PLLDSIODF 	<< 	DSI_WRPCR_PLL_ODF_Pos);							// Set PLL ODF
    DSI->WRPCR |= DSI_WRPCR_PLLEN; 													// Enable DSI PLL
    while(!(DSI->WISR & DSI_WISR_PLLLS)); 											// Wait for PLL Lock Interrupt Flag

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
						0;										        // Set Video mode Type to  sync pulse mode

	DSI->VPCR = 		800;								// set horizontal pixal count
	DSI->VCCR = 		1	<<	DSI_VCCR_NUMC_Pos;			// set number of packets per line
	DSI->VNPCR = 		0	 << DSI_VNPCR_NPSIZE_Pos;		// disables null packets
	DSI->VLCR =         1575 << DSI_VLCR_HLINE_Pos;														//?????? Check value
	DSI->VVSACR  = 		(4   << DSI_VVSACR_VSA_Pos);
	DSI->VVBPCR = 		(14  << DSI_VVBPCR_VBP_Pos);
	DSI->VVFPCR = 		(14  << DSI_VVFPCR_VFP_Pos);
	DSI->VVACR = 		(480 << DSI_VVACR_VA_Pos);


	//DSI->LCCCR |=       DSI_LCCCR_COLC0 | DSI_LCCCR_COLC2; 						// Enable Color Mode RGB888
	DSI->LCOLCR |=      DSI_LCOLCR_COLC0 | 4; 										// Enable Color Mode RGB888



	DSI->PCTLR |=       DSI_PCTLR_DEN; 												// Enable DSI digital section
	DSI->PCTLR |=       DSI_PCTLR_CKE; 												// Enable DSI clocks
	//DSI->MCR &= ~DSI_MCR_CMDM; // enable video mode
	DSI->WCR 		|= DSI_WCR_DSIEN;												// Enable DSI Wrapper
	DSI -> CR 		|= DSI_CR_EN;													// Enable DSI

	DSI->MCR |= DSI_MCR_CMDM; // enable command mode
	OTM8009A_Init(OTM8009A_FORMAT_RGB888, OTM8009A_ORIENTATION_LANDSCAPE);
	DSI->MCR &= ~DSI_MCR_CMDM; // enable video mode
	DSI->VMCR |=        DSI_VMCR_PGM;
	DSI->VMCR |=        DSI_VMCR_PGE; 												// Enable Pattern Generator
}
