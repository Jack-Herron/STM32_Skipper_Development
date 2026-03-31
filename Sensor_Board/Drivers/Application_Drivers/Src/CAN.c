/*
 * CAN.c
 *
 *  Created on: Mar 15, 2026
 *      Author: jackh
 */

#include "stm32f1xx.h"
#include <stdlib.h>
#include "CAN.h"

void (*CAN___RX_Callback)(CAN___Receive_TypeDef) = NULL;

#if CAN___ENABLE_STDIO == 1
	int _write(int file __attribute__((unused)), char *data, int len)
	{
		CAN_Tansmit_TypeDef packet;
		packet.ID = 0x700;
		packet.data_Length = 1;
		CAN___Transmit(packet);

		uint32_t len_Remaining = len;
		uint32_t num_Packets = (len+7)/8;

		for(uint32_t i = 0; i < num_Packets; i++)
		{
			uint8_t length_To_Transmit = len_Remaining > 8 ? 8 : len_Remaining;
			packet.ID = 0x703;
			for(uint8_t j = 0; j < length_To_Transmit; j++)
			{
				packet.data[j] = data[i*8+j];
			}
			packet.data_Length = length_To_Transmit;

			CAN___Transmit(packet);
			len_Remaining -= length_To_Transmit;
		}
		return(len);
	}

	int _read(int file __attribute__((unused)), char *data, int len)
	{

		return len;
	}
#endif

void CAN___GPIO_Init(void)
{
    /* Enable GPIOA and AFIO clocks */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

    /* ---------- PA10 : CAN_RX ---------- */
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |=  GPIO_CRH_CNF10_1;          	// Input with pull-up / pull-down
    GPIOA->ODR |=  GPIO_ODR_ODR10;            	// Pull-up

    /* ---------- PA12 : CAN_TX ---------- */
    GPIOA->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
    GPIOA->CRH |=  GPIO_CRH_MODE12;           	// Output mode 10 MHz
    GPIOA->CRH |=  GPIO_CRH_CNF12_1;           	// Alternate function push-pull
}

//														|		1 CAN bit			|
void CAN___Init()		// baud rate = 250kHz				[ 1 Sync | 13 TS1 | 2 TS2 	] (16 bit times total)
{
	CAN___GPIO_Init();
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

    CAN1->IER |= CAN_IER_FMPIE0;						// enable FIFO full interrupt

    NVIC_SetPriority (CAN1_RX0_IRQn, 1);				// Set Interrupt Priority
	NVIC_EnableIRQ (CAN1_RX0_IRQn);						// Enable Interrupt

    CAN1->MCR &= ~CAN_MCR_SLEEP;						// exit sleep mode
}

void CAN___Accept_All_Messages()
{
	CAN1->FMR |= CAN_FMR_FINIT;

	CAN1->FS1R |=  (1 << 0);   // bank 0 = 32-bit
	CAN1->FM1R &= ~(1 << 0);   // mask mode
	CAN1->FFA1R &= ~(1 << 0);  // FIFO 0

	CAN1->sFilterRegister[0].FR1 = 0;
	CAN1->sFilterRegister[0].FR2 = 0;

	CAN1->FA1R |= (1 << 0);    // enable bank 0

	CAN1->FMR &= ~CAN_FMR_FINIT;
}

void CAN___Transmit(CAN_Tansmit_TypeDef Payload)
{
	while ((CAN1->TSR & CAN_TSR_TME0) == 0);
	CAN1->sTxMailBox[0].TIR = (Payload.ID << CAN_TI1R_STID_Pos);				// Set message ID

	CAN1->sTxMailBox[0].TDTR = (Payload.data_Length << CAN_TDT1R_DLC_Pos);		// set data length

	CAN1->sTxMailBox[0].TDLR = ((uint32_t*)(Payload.data))[0];					// send first 4 bytes

	if(Payload.data_Length > 4)
	{
		CAN1->sTxMailBox[0].TDHR = ((uint32_t*)(Payload.data))[1];				// send next bytes if length is >4
	}

	CAN1->sTxMailBox[0].TIR |= CAN_TI1R_TXRQ;									// request to send
}

void CAN___Set_RX_Callback(void (*callback)(CAN___Receive_TypeDef))
{
	CAN___RX_Callback = callback;
}

void USB_LP_CAN_RX0_IRQHandler(void)
{
	if (CAN1->RF0R & CAN_RF0R_FMP0) {                 			// if pending message

		uint32_t RIR  = CAN1->sFIFOMailBox[0].RIR;
		uint32_t data[2];
		data[0] = CAN1->sFIFOMailBox[0].RDLR;
		data[1] = CAN1->sFIFOMailBox[0].RDHR;

		uint32_t RDTR = CAN1->sFIFOMailBox[0].RDTR;

		uint16_t ID = ((RIR & CAN_RI0R_STID_Msk) >> CAN_RI0R_STID_Pos);
		uint16_t data_Length = ((RDTR & CAN_RDT0R_DLC_Msk) >> CAN_RDT0R_DLC_Pos);

		CAN___Receive_TypeDef RX_Payload;
		RX_Payload.ID = ID;
		RX_Payload.data = (uint8_t*)data;
		RX_Payload.data_Length = data_Length;

		if(CAN___RX_Callback != NULL)
		{
			CAN___RX_Callback(RX_Payload);
		}
		CAN1->RF0R |= CAN_RF0R_RFOM0;                 			// release FIFO0 output mailbox
	}
}
