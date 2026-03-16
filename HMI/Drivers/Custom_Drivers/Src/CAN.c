/*
 * CAN.c
 *
 *  Created on: Mar 15, 2026
 *      Author: jackh
 */

#include "stm32f4xx.h"
#include "Clock.h"
#include "CAN.h"
#include <stdio.h>

void CAN___GPIO_Init()
{
    /* Enable GPIOB clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    /* ---------- PB12 : CAN2_RX ---------- */

    GPIOB->MODER &= ~(3 << (12 * 2));
    GPIOB->MODER |=  (2 << (12 * 2));        // Alternate function mode

    GPIOB->PUPDR &= ~(3 << (12 * 2));
    GPIOB->PUPDR |=  (1 << (12 * 2));        // Pull-up (recommended for CAN RX)

    GPIOB->OSPEEDR |= (3 << (12 * 2));       // High speed

    GPIOB->AFR[1] &= ~(0xF << ((12 - 8) * 4));
    GPIOB->AFR[1] |=  (9 << ((12 - 8) * 4)); // AF9 = CAN2


    /* ---------- PB13 : CAN2_TX ---------- */

    GPIOB->MODER &= ~(3 << (13 * 2));
    GPIOB->MODER |=  (2 << (13 * 2));        // Alternate function mode

    GPIOB->OTYPER &= ~(1 << 13);             // Push-pull

    GPIOB->OSPEEDR |= (3 << (13 * 2));       // High speed

    GPIOB->PUPDR &= ~(3 << (13 * 2));        // No pull

    GPIOB->AFR[1] &= ~(0xF << ((13 - 8) * 4));
    GPIOB->AFR[1] |=  (9 << ((13 - 8) * 4)); // AF9 = CAN2
}

// APB1CLK = 45Mhz
//														|		1 CAN bit			|
void CAN___Init()		// baud rate = 250kHz				[ 1 Sync | 15 TS1 | 2 TS2 	] (18 bit times total)
{
	CAN___GPIO_Init();

	RCC->APB1ENR |= RCC_APB1ENR_CAN2EN | RCC_APB1ENR_CAN1EN;					// enable CAN clock (2 depends on 1)

	CAN2->MCR |= CAN_MCR_INRQ;							// enter INIT mode
	while ((CAN2->MSR & CAN_MSR_INAK) == 0);			// wait for confirmation

	CAN2->BTR = 0;										// clear timing register

	CAN2->BTR |= 9 << CAN_BTR_BRP_Pos;					// set baud rate prescaler to 8. BRP = (Ttimequanta/Tpclk)-1. Ttimequanta = Tbaudrate/16 (because the can bit time will be divided into 18 for steps)
	CAN2->BTR |= (15-1) << CAN_BTR_TS1_Pos;				// set TS1 to 15 time quanta
	CAN2->BTR |= (2-1) << CAN_BTR_TS2_Pos;				// set TS2 to 2 time quanta
	CAN2->BTR |= (1-1) << CAN_BTR_SJW_Pos;				// set SYNC to 1 time quanta

	CAN2->MCR &= ~CAN_MCR_INRQ;							// exit INIT mode
    while ((CAN2->MSR & CAN_MSR_INAK) != 0);			// wait for confirmation

    CAN2->IER |= CAN_IER_FMPIE0;						// enable FIFO full interrupt

    NVIC_SetPriority (CAN2_RX0_IRQn, 1);				// Set Interrupt Priority
	NVIC_EnableIRQ (CAN2_RX0_IRQn);						// Enable Interrupt

    CAN2->MCR &= ~CAN_MCR_SLEEP;						// exit sleep mode
}

void CAN___Accept_All_Messages()
{
	CAN2->FMR 	|= CAN_FMR_FINIT;

	CAN2->FS1R 	|=  (1 << 0);   // bank 0 = 32-bit
	CAN2->FM1R 	&= ~(1 << 0);   // mask mode
	CAN2->FFA1R &= ~(1 << 0);  // FIFO 0

	CAN2->sFilterRegister[0].FR1 = 0;
	CAN2->sFilterRegister[0].FR2 = 0;

	CAN2->FA1R 	|= (1 << 0);    // enable bank 0

	CAN2->FMR 	&= ~CAN_FMR_FINIT;
}

void CAN___Transmit(CAN_Tansmit_TypeDef Payload)
{
	CAN2->sTxMailBox[0].TIR = (Payload.ID << CAN_TI1R_STID_Pos);				// Set message ID

	CAN2->sTxMailBox[0].TDTR = (Payload.data_Length << CAN_TDT1R_DLC_Pos);		// set data length

	CAN2->sTxMailBox[0].TDLR = ((uint32_t*)(Payload.data))[0];					// send first 4 bytes

	if(Payload.data_Length > 4)
	{
		CAN2->sTxMailBox[0].TDHR = ((uint32_t*)(Payload.data))[1];				// send next bytes if length is >4
	}

	CAN2->sTxMailBox[0].TIR |= CAN_TI1R_TXRQ;									// request to send
}

void CAN2_RX0_IRQHandler(void)
{
	if (CAN2->RF0R & CAN_RF0R_FMP0) {                 // if pending message

		uint32_t RIR  = CAN2->sFIFOMailBox[0].RIR;
		uint32_t RDLR = CAN2->sFIFOMailBox[0].RDLR;

		printf("CAN Message Received: %d\n", (int)RDLR);

		CAN2->RF0R |= CAN_RF0R_RFOM0;                 			// release FIFO0 output mailbox
	}
}
