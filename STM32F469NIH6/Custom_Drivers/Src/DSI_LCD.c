/*
 * DSI_LCD.c
 *
 *  Created on: Dec 22, 2025
 *      Author: jackh
 */

#include "stm32f469xx.h"
#include "DSI_LCD.h"
#include "Clock.h"
#include "nt35510.h"
#include "stdlib.h"

// TODO implement real delays using timer in clock file

uint32_t TE_Flag = 0;
uint8_t* pending_Buffer_Address = NULL;
void (*Swap_Callback)(void) = NULL;


void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams)
{
	if(NbrParams <= 1)
	{
		DSI_LCD___Short_Write(0, DSI_LCD___DATA_TYPE_SHORT_WRITE_1P, pParams[0], pParams[1]);
	}
	else
	{
		DSI_LCD___Long_Write(0, DSI_LCD___DATA_TYPE_LONG_WRITE, pParams, NbrParams+1);
	}
}

void NT35510_IO_Delay(uint32_t Delay)
{
	clock___delay_ms(Delay);		// crude delay
}

void DSI_LCD___Short_Write(uint32_t channel_ID, uint32_t data_Type, uint32_t data1, uint32_t data2)
{
	while (!(DSI->GPSR & DSI_GPSR_CMDFE)); 											// Wait until command FIFO is empty
	DSI->GHCR = (data_Type | (channel_ID << DSI_GHCR_VCID_Pos) | (data1 << DSI_GHCR_WCLSB_Pos) | (data2 << DSI_GHCR_WCMSB_Pos));
}

//DSI_LCD___Long_Write takes a list of parameters, with the last byte being the command byte [p1, p2, ..., pn, cmd]
void DSI_LCD___Long_Write(uint32_t channel_ID, uint32_t data_Type, uint8_t *pdata, uint32_t size)
{
	uint8_t command = pdata[size - 1];

	while (!(DSI->GPSR & DSI_GPSR_CMDFE)); 									// Wait until command FIFO is empty

	uint32_t first_Word = command;
	uint32_t first_Word_Params = (size <= 4) ? (size) : (4);

	for (uint32_t i = 1; i < first_Word_Params; i++) {
		first_Word |= pdata[i-1] << (8 * (i));
	}

	DSI->GPDR = first_Word;

	uint32_t remaining_Size = size - first_Word_Params;

	for (uint32_t i = 0; i < remaining_Size; i += 4) {
		uint32_t word = 0;
		uint32_t bytes_In_Word = (remaining_Size - i >= 4) ? 4 : (remaining_Size - i);

		for (uint32_t j = 0; j < bytes_In_Word; j++)
		{
			word |= pdata[first_Word_Params + i + j - 1] << (8 * j);
		}

		DSI->GPDR = word;
	}

	uint32_t wc_lsb = (size & 0xFF);
	uint32_t wc_msb = ((size >> 8) & 0xFF);

	DSI->GHCR = (data_Type)
	              | (channel_ID << DSI_GHCR_VCID_Pos)
	              | (wc_lsb << DSI_GHCR_WCLSB_Pos)
	              | (wc_msb << DSI_GHCR_WCMSB_Pos);
}

void DSI_LCD___DSI_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN; 												// Enable GPIOJ clock
	GPIOJ->MODER |= GPIO_MODER_MODER2_1; 												// Set PJ2 to Alternate Function mode
	GPIOJ->AFR[0] |= (13 << GPIO_AFRL_AFSEL2_Pos); 										// Set PJ2 to DSI Alternate Function (AF13)

	// Enable DSI clock
	RCC->APB2ENR |= RCC_APB2ENR_DSIEN;
	while (!(RCC->APB2ENR & RCC_APB2ENR_DSIEN)); 										// Wait for DSI clock to be enabled

	DSI->WRPCR = 0; 																	// Reset DSI Wrapper

	// DSI Regulator enable
	DSI->WRPCR |= DSI_WRPCR_REGEN; 														// Enable DSI Wrapper Regulator
	while(!(DSI->WISR & DSI_WISR_RRS)); 												// Wait for Regulator Ready Interrupt Flag

	// DSI PLL Configuration
	DSI->WRPCR |= 	(PLLDSINDIV << 	DSI_WRPCR_PLL_NDIV_Pos) |							// Set PLL N Divider
					(PLLDSIIDF 	<< 	DSI_WRPCR_PLL_IDF_Pos) 	|							// Set PLL IDF);
					(PLLDSIODF 	<< 	DSI_WRPCR_PLL_ODF_Pos);								// Set PLL ODF
	DSI->WRPCR |= DSI_WRPCR_PLLEN; 														// Enable DSI PLL
	while(!(DSI->WISR & DSI_WISR_PLLLS)); 												// Wait for PLL Lock Interrupt Flag

	// DSI Host Configuration
	DSI -> WPCR[0] 	=  	(DSI_LCD___LANE_PERIODX4 		<< DSI_WPCR0_UIX4_Pos); 		// Set HS to LP time
	DSI -> PCONFR 	=	((DSI_LCD___LANE_COUNT-1) 		<< DSI_PCONFR_NL_Pos) 		|	// Set number of lanes to 2
						DSI_LCD___STOP_WAIT_TIME 		<< DSI_PCONFR_SW_TIME_Pos;		// Set the minimum wait period to request a high-speed transmission after the Stop state.
	DSI -> CLCR 	= 	DSI_CLCR_DPCC; 													// Enable D-PHY Clock HS mode
	DSI -> CLTCR 	= 	DSI_LCD___CLOCK_HS2LP_TIME 		<< DSI_CLTCR_HS2LP_TIME_Pos |	// Set HS to LP time for clock lanes
						DSI_LCD___CLOCK_LP2HS_TIME 		<< DSI_CLTCR_LP2HS_TIME_Pos; 	// Set LP to HS time for clock lanes
	DSI -> DLTCR 	= 	DSI_LCD___DATA_HS2LP_TIME 		<< DSI_DLTCR_HS2LP_TIME_Pos |	// Set HS to LP time for data lanes
						DSI_LCD___DATA_LP2HS_TIME 		<< DSI_DLTCR_LP2HS_TIME_Pos; 	// Set LP to HS time for data lanes

	DSI -> CCR      =   DSI_LCD___TX_TIMEOUT_CLOCK_DIV 	<< DSI_CCR_TOCKDIV_Pos	|		// Set the timeout clock division factor
						DSI_LCD___TX_ESCAPE_CLOCK_DIV 	<< DSI_CCR_TXECKDIV_Pos;		// Set the timeout clock division factor

	DSI -> PCTLR 	=   DSI_PCTLR_DEN	| 												// Enable DSI digital section
						DSI_PCTLR_CKE; 													// Enable DSI clocks													// Enable DSI

	DSI -> WPCR[0]  =   DSI_LCD___LANE_PERIODX4;	 									// Set the clock to transmit at selected frequency
	DSI -> VMCR     =   DSI_LCD___VMT_MODE_BURST | 										// Set Video mode to Burst
						DSI_VMCR_LPCE 			 |										// Enable Low Power Command Enable
						DSI_VMCR_LPVSAE          |
						DSI_VMCR_LPVBPE          |
						DSI_VMCR_LPVFPE          |
						DSI_VMCR_LPVAE           |
						DSI_VMCR_LPHBPE          |
						DSI_VMCR_LPHFPE          |
						DSI_VMCR_LPCE;

	DSI -> LPMCR    =   0x10 << DSI_LPMCR_LPSIZE_Pos; 									// Set Low Power Size to 16 bytes
	DSI -> PCR      =   DSI_PCR_BTAE; 													// Enable Bus Turn Around

	DSI -> VPCR		=   DSI_LCD___HACT; 												// Set packet size to contain 1 line of pixel data
	DSI -> VCCR		= 	0; 																// Set to zero because 1 line per packet
	DSI -> VNPCR	=   0xfff; 															// Set null packet size (burst mode so max)
	DSI -> VLCR	    =   DSI_LCD___HLINE; 												// Set the total number of pixel clock periods per line
	DSI -> VHSACR	=   DSI_LCD___HSA_FOR_DSI; 											// Set Horizontal Synchronism Active duration
	DSI -> VHBPCR	=   DSI_LCD___HBP_FOR_DSI; 											// Set Horizontal Back Porch duration

	DSI -> VVSACR   =   DSI_LCD___VSA; 													// Set Vertical Synchronism Active duration in pixels
	DSI -> VVACR    =   DSI_LCD___VACT; 												// Set Vertical Active duration in pixels
	DSI -> VVFPCR   =   DSI_LCD___VFP; 													// Set Vertical Front Porch duration in pixels
	DSI -> VVBPCR   =   DSI_LCD___VBP; 													// Set Vertical Back Porch duration in pixels

	DSI -> LCOLCR   =   DSI_LCD___LCOLC_RGB888; 										// Set color coding format to RGB888
	DSI -> WCFGR    =   DSI_LCD___COLMUX_RGB888;

	DSI -> WIER 	= 	DSI_WIER_TEIE; 													// Enable Tearing Effect Interrupt

	NVIC_EnableIRQ(DSI_IRQn); 															// Enable DSI Interrupt in NVIC
	NVIC_SetPriority(DSI_IRQn, 12); 														// Set DSI Interrupt priority

	DSI -> WCFGR    |= 	DSI_WCFGR_TESRC; 												// Set Tearing Effect Source to External Pin
	DSI -> WCR 		= 	DSI_WCR_DSIEN;										 			// Enable DSI Wrapper
	DSI -> CR 		= 	DSI_CR_EN; 														// Enable DSI

	NT35510_Init(NT35510_FORMAT_RGB888, NT35510_ORIENTATION_LANDSCAPE);

	NT35510_IO_Delay(10);
}

void DSI_LCD___Panel_Reset()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN; 												// Enable GPIOH clock
	GPIOH->MODER |= GPIO_MODER_MODER7_0; 												// Set PH7 to Output mode
	GPIOH->ODR &= ~GPIO_ODR_OD7; 														// Set PH7 Low (LCD Reset active)
	clock___delay_ms(20);					                                            // Delay 20ms
	GPIOH->ODR |= GPIO_ODR_OD7; 														// Set PH7 High (LCD Reset inactive)
}

void DSI_LCD___Generate_Pattern(uint8_t mode, uint8_t orientation)
{
	DSI -> VMCR &= ~DSI_VMCR_PGM_Msk;  					// reset pattern gen
	DSI -> VMCR |= (mode << DSI_VMCR_PGM_Pos);  		// set pattern gen mode
	DSI -> VMCR &= ~DSI_VMCR_PGO_Msk;  					// reset pattern gen
	DSI -> VMCR |= (orientation << DSI_VMCR_PGO_Pos);  	// set pattern gen orientation
	DSI -> VMCR |= DSI_VMCR_PGE;  						// enable pattern gen
}

void DSI_LCD___LTDC_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN;	                                                        	// Enable SAI1 clock
	while (!(RCC->APB2ENR & RCC_APB2ENR_SAI1EN)); 													// Wait for SAI1 clock to be enabled
	RCC->APB2ENR |= RCC_APB2ENR_LTDCEN; 															// Enable LTDC clock
	while (!(RCC->APB2ENR & RCC_APB2ENR_LTDCEN)); 													// Wait for LTDC clock to be enabled

	LTDC->SSCR = 	((DSI_LCD___HSA - 1) 	<< LTDC_SSCR_HSW_Pos) | 								// Set Horizontal Synchronization Width
					((DSI_LCD___VSA - 1) 	<< LTDC_SSCR_VSH_Pos);									// Set Vertical Synchronization Height

	LTDC->BPCR = 	((DSI_LCD___HBP + DSI_LCD___HSA - 1) 	<< LTDC_BPCR_AHBP_Pos) | 				// Set horizontal back porch width
					((DSI_LCD___VBP + DSI_LCD___VSA - 1) 	<< LTDC_BPCR_AVBP_Pos);    				// Set vertical back porch height

	LTDC->AWCR = 	((DSI_LCD___HACT + DSI_LCD___HBP + DSI_LCD___HSA - 1) << LTDC_AWCR_AAW_Pos) | 	// Set accumulated active width
					((DSI_LCD___VACT + DSI_LCD___VBP + DSI_LCD___VSA - 1) << LTDC_AWCR_AAH_Pos);   	// Set accumulated active height

	LTDC->TWCR = 	((DSI_LCD___HACT + DSI_LCD___HBP + DSI_LCD___HSA + DSI_LCD___HFP - 1) << LTDC_TWCR_TOTALW_Pos) | 	// Set total width
					((DSI_LCD___VACT + DSI_LCD___VBP + DSI_LCD___VSA + DSI_LCD___VFP - 1) << LTDC_TWCR_TOTALH_Pos);   	// Set total height

	LTDC->BCCR =    (0x00fff2);					                                                                        // Set background color to Teal

	LTDC->GCR  = 	LTDC_GCR_HSPOL | LTDC_GCR_VSPOL; 																	// Set horizontal and vertical sync polarity to active high

	LTDC_Layer1->WHPCR 	= 	((DSI_LCD___HBP + DSI_LCD___HSA)						<< LTDC_LxWHPCR_WHSTPOS_Pos) | 		// Set horizontal start position
							((DSI_LCD___HACT + DSI_LCD___HBP + DSI_LCD___HSA - 1) 	<< LTDC_LxWHPCR_WHSPPOS_Pos);		// Set horizontal stop position

	LTDC_Layer1->WVPCR 	= 	((DSI_LCD___VBP + DSI_LCD___VSA) 						<< LTDC_LxWVPCR_WVSTPOS_Pos) | 		// Set vertical start position
							((DSI_LCD___VACT + DSI_LCD___VBP + DSI_LCD___VSA - 1) 	<< LTDC_LxWVPCR_WVSPPOS_Pos);		// Set vertical stop position

	LTDC_Layer1->PFCR 	=	DSI_LCD___PF_ARGB8888; 																		// Set pixel format to RGB888

	LTDC_Layer1->DCCR   =   0xffAAfff2; 																				// Set default color to Teal with full opacity

	LTDC_Layer1->CFBLR 	= 	((DSI_LCD___ARGB_BYTES_PER_PIXEL * DSI_LCD___HACT) 			<< LTDC_LxCFBLR_CFBP_Pos) | 	// Set color frame buffer line length
							(((DSI_LCD___ARGB_BYTES_PER_PIXEL * DSI_LCD___HACT) + 3) 	<< LTDC_LxCFBLR_CFBLL_Pos);		// Set color frame buffer line pitch

	LTDC_Layer1->CFBLNR =  	DSI_LCD___VACT; 																			// Set color frame buffer line number

	LTDC_Layer1->CFBAR  =   DSI_LCD___FRAME_BUFFER_ADDRESS; 															// Set frame buffer address

	LTDC_Layer1 -> CR 	=	LTDC_LxCR_LEN; 																				// Enable

	LTDC->SRCR = LTDC_SRCR_IMR; 																						// Reload shadow registers

	LTDC->GCR |= LTDC_GCR_LTDCEN; 																						// Enable LTDC
}

void DSI_LCD___Init(void)
{
	DSI_LCD___Panel_Reset();
	DSI_LCD___LTDC_Init();
	DSI_LCD___DSI_Init();
	DSI -> MCR &= ~DSI_MCR_CMDM;

	GPIOD->MODER 	|= GPIO_MODER_MODER4_0; 												// Set PD4 to Output mode
	GPIOD->ODR 		|= GPIO_ODR_OD4; 														// Set PD4 High (LED on)

}

void DSI_LCD___Set_Buffer(uint8_t* buf)
{
	LTDC_Layer1->CFBAR = (uint32_t) buf; 			// Set frame buffer address
	LTDC->SRCR = LTDC_SRCR_IMR; 					// Reload shadow registers
}

void DSI_LCD___Set_Pending_Buffer(uint8_t* buf)
{
	pending_Buffer_Address = buf;
}

void DSI_LCD___Reset_TE_Flag(void)
{
	TE_Flag = 0;
}

uint32_t DSI_LCD___Get_TE_Flag(void)
{
	return(TE_Flag);
}

void DSI_LCD___Set_Swap_Callback(void (*callback)(void))
{
	Swap_Callback = callback;
}

void DSI_IRQHandler(void)
{
	uint32_t pending_ier = DSI->WISR & DSI->WIER;

	if (pending_ier & DSI_WISR_TEIF) 				// Tearing Effect Interrupt
	{
		TE_Flag = 1;

		if (GPIOD->ODR & GPIO_ODR_OD4)
		{
			GPIOD->ODR &= ~GPIO_ODR_OD4;
		}
		else
		{
			GPIOD->ODR |= GPIO_ODR_OD4;
		}

		if (pending_Buffer_Address != NULL)
		{
			DSI_LCD___Set_Buffer((uint8_t*) pending_Buffer_Address);
			if (Swap_Callback != NULL)
			{
				Swap_Callback();
			}
			pending_Buffer_Address = NULL;
		}
		DSI->WIFCR = DSI_WIFCR_CTEIF; 	// Clear Tearing Effect Interrupt Flag
	}
}
