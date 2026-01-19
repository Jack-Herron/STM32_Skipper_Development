#include <stdint.h>
#include "stm32f1xx.h"

typedef struct
{
	uint16_t ID;
	uint8_t data[8];
	uint8_t data_Length;

}CAN_Tansmit_TypeDef;

void Clock_Init(){	// initiate clock ()
	RCC -> CR |= RCC_CR_HSEON;																						// enable HSE Oscillator
	while(!(RCC -> CR & RCC_CR_HSERDY));																			// wait for HSE To Stabilize
	FLASH -> ACR |= FLASH_ACR_LATENCY_2;																			// set flash wait to 2, because 48 MHz < SYSCLK  72 MHz
	RCC -> CFGR &= ~(RCC_CFGR_USBPRE);																				// set USB Pre-scaler to /1.5
	RCC -> CFGR |=
					(RCC_CFGR_PLLMULL9		) |																		// set PLL Multiplication Factor to *9
					(RCC_CFGR_PLLSRC			) |																	// select HSE as the PLL Source
					(RCC_CFGR_ADCPRE_DIV6	) |																		// set ADC Pre-scaler to /6
					(RCC_CFGR_PPRE1_DIV2	);																		// set APB1 Pre-Scaler to /2
	RCC -> CR |= (RCC_CR_PLLON);																					// enable PLL
	while(!(RCC -> CR & RCC_CR_PLLRDY));																			// wait for PLL To Stabilize
	RCC -> CFGR |= (RCC_CFGR_SW_PLL);																				// switch PLL on
	while(!(RCC -> CFGR & RCC_CFGR_SWS_PLL));																		// Wait for source to switch to PLL
	RCC -> CR &= ~(1<<0);																							// Turn off HSI
}

void GPIO_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;					// enable alt function clock

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;					// Enable GPIOC clock
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;					// Enable GPIOB clock

	GPIOC->CRH &= ~GPIO_CRH_MODE13_Msk;					// Clear PC13 mode
	GPIOC->CRH &= ~GPIO_CRH_CNF13_Msk;					// clear PC13 configuration

	GPIOC->CRH |= GPIO_CRH_MODE13_0;					// Set PC13 to output (slow) for on board LED

	GPIOB->CRH &= ~GPIO_CRH_MODE8_Msk;					// Clear PB8 mode
	GPIOB->CRH &= ~GPIO_CRH_CNF8_Msk;					// clear PB8 configuration

	GPIOB->CRH &= ~GPIO_CRH_MODE9_Msk;					// Clear PB9 mode
	GPIOB->CRH &= ~GPIO_CRH_CNF9_Msk;					// clear PB9 configuration

	GPIOB->CRH |= GPIO_CRH_CNF8_0;						// set PB8 to floating input (CANRX)
	GPIOB->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1; 	// set PB9 to fast output mode (CANTX)
	GPIOB->CRH |= GPIO_CRH_CNF9_1;						// set PB9 to alt function push pull (CANTX)

	AFIO->MAPR &= ~AFIO_MAPR_CAN_REMAP_Msk;				// clear CAN remap
	AFIO->MAPR |= AFIO_MAPR_CAN_REMAP_1;				// remap CAN to PB8,PB9

}

void CAN_Transmit(CAN_Tansmit_TypeDef Payload)
{
	CAN1->sTxMailBox[0].TIR = (Payload.ID << CAN_TI1R_STID_Pos);				// Set message ID

	CAN1->sTxMailBox[0].TDTR = (Payload.data_Length << CAN_TDT1R_DLC_Pos);		// set data length

	CAN1->sTxMailBox[0].TDLR = ((uint32_t*)(Payload.data))[0];					// send first 4 bytes

	if(Payload.data_Length > 4)
	{
		CAN1->sTxMailBox[0].TDHR = ((uint32_t*)(Payload.data))[1];				// send next bytes if length is >4
	}

	CAN1->sTxMailBox[0].TIR |= CAN_TI1R_TXRQ;									// request to send
}


//															|		1 CAN bit			|
void CAN_Init()		// baud rate = 250kHz				 	[ 1 Sync | 13 TS1 | 2 TS2 	] (16 bit times total)
{
	RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;					// enable CAN clock

	CAN1->MCR |= CAN_MCR_INRQ;							// enter INIT mode
	while ((CAN1->MSR & CAN_MSR_INAK) == 0);			// wait for confirmation

	CAN1->BTR = 0;										// clear timing register

	CAN1->BTR |= 8 << CAN_BTR_BRP_Pos;					// set baud rate prescaler to 8. BRP = (Ttimequanta/Tpclk)-1. Ttimequanta = Tbaudrate/16 (because the can bit time will be divided into 16 for steps)

	CAN1->BTR |= (13-1) << CAN_BTR_TS1_Pos;				// set TS1 to 13 time quanta
	CAN1->BTR |= (2-1) << CAN_BTR_TS2_Pos;				// set TS2 to 2 time quanta
	CAN1->BTR |= (1-1) << CAN_BTR_SJW_Pos;				// set SYNC to 1 time quanta

	CAN1->MCR &= ~CAN_MCR_INRQ;							// exit INIT mode
    while ((CAN1->MSR & CAN_MSR_INAK) != 0);			// wait for confirmation

    CAN1->MCR &= ~CAN_MCR_SLEEP;						// exit sleep mode
}

int main(void)
{
	Clock_Init();
	GPIO_Init();
	CAN_Init();

	for(;;)
	{
		for(uint32_t i = 0; i < 0xfffff; i++);
		GPIOC->ODR |= (1<<13);
		for(uint32_t i = 0; i < 0xfffff; i++);
		GPIOC->ODR &= ~(1<<13);
		CAN_Tansmit_TypeDef Payload;
		Payload.ID = 0x123;
		Payload.data_Length = 8;
		Payload.data[0] = 0xDE;
		Payload.data[1] = 0xAD;
		Payload.data[2] = 0xBE;
		Payload.data[3] = 0xEF;
		Payload.data[4] = 0xDE;
		Payload.data[5] = 0xAD;
		Payload.data[6] = 0xBE;
		Payload.data[7] = 0xEF;
		CAN_Transmit(Payload);
	}
}
