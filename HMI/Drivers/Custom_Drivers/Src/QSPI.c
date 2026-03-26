/*
 * QSPI.c
 *
 *  Created on: Jan 3, 2026
 *      Author: jackh
 */

#include "stm32f4xx.h"
#include "Clock.h"
#include "QSPI.h"

void QSPI___Pin_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t function)
{
	GPIOx->MODER &= ~(3 << (GPIO_Pin * 2));								// clear mode bits
	GPIOx->AFR[GPIO_Pin/8] &= ~(0xF << ((GPIO_Pin & 0x07) * 4));		// clear alternate function bits
	GPIOx->OSPEEDR &= ~(3 << (GPIO_Pin * 2));							// clear speed bits
	GPIOx->PUPDR &= ~(3 << (GPIO_Pin * 2));								// clear pull-up/pull-down bits

	GPIOx->AFR[GPIO_Pin/8] |= (function << ((GPIO_Pin & 0x07) * 4));	// set alternate function #
	GPIOx->MODER |= (2 << (GPIO_Pin * 2));								// set mode to alternate function
	GPIOx->OSPEEDR |= (3 << (GPIO_Pin * 2));							// set speed to high
}

void QSPI___GPIO_Init()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN; // Enable GPIOF clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable GPIOB clock

	QSPI___Pin_Init(GPIOF, 6, 9);		// PF6 -> AF9
	QSPI___Pin_Init(GPIOF, 7, 9);		// PF7 -> AF9
	QSPI___Pin_Init(GPIOF, 8, 10);		// PF8 -> AF10
	QSPI___Pin_Init(GPIOF, 9, 10);		// PF9 -> AF10
	QSPI___Pin_Init(GPIOF, 10, 9);		// PF10 -> AF9
	QSPI___Pin_Init(GPIOB, 6, 10);		// PB6 -> AF10
}

static void QSPI___Wait_While_Busy(void)
{
    while (QUADSPI->SR & QUADSPI_SR_BUSY);
}

static void QSPI___Clear_Flags(void)
{
    QUADSPI->FCR = QUADSPI_FCR_CTCF
                 | QUADSPI_FCR_CSMF
                 | QUADSPI_FCR_CTOF
                 | QUADSPI_FCR_CTEF;
}

static void QSPI___Send_Command_1Line(uint8_t cmd)
{
    QSPI___Wait_While_Busy();
    QSPI___Clear_Flags();

    QUADSPI->DLR = 0;   // no data

    QUADSPI->CCR =
          (1U << QUADSPI_CCR_IMODE_Pos)                 			// instruction on 1 line
        | (0U << QUADSPI_CCR_FMODE_Pos)                 			// indirect write
        | ((uint32_t)cmd << QUADSPI_CCR_INSTRUCTION_Pos);

    while ((QUADSPI->SR & QUADSPI_SR_TCF) == 0);
    QUADSPI->FCR = QUADSPI_FCR_CTCF;
}

static uint8_t QSPI___Read_Status(void)
{
    uint8_t status;

    QSPI___Wait_While_Busy();
    QSPI___Clear_Flags();

    QUADSPI->DLR = 0;   // 1 byte

    QUADSPI->CCR =
          (1U << QUADSPI_CCR_IMODE_Pos)                 				// instruction on 1 line
        | (1U << QUADSPI_CCR_DMODE_Pos)                 				// data on 1 line
        | (1U << QUADSPI_CCR_FMODE_Pos)                 				// indirect read
        | ((uint32_t)0x05 << QUADSPI_CCR_INSTRUCTION_Pos);

    while ((((QUADSPI->SR >> QUADSPI_SR_FLEVEL_Pos) & 0x1FU) == 0));
    status = *(volatile uint8_t *)&QUADSPI->DR;

    while ((QUADSPI->SR & QUADSPI_SR_TCF) == 0);
    QUADSPI->FCR = QUADSPI_FCR_CTCF;

    return status;
}

static void QSPI___Write_Enable(void)
{
    QSPI___Send_Command_1Line(0x06);

    while ((QSPI___Read_Status() & 0x02U) == 0);	// Wait for WEL = 1
}

static void QSPI___Wait_Flash_Ready(void)
{
    while (QSPI___Read_Status() & 0x01U);
}

uint8_t QSPI___Erase_Sector(uint32_t address)
{
    /* Align to 4 KB sector boundary */
    address &= ~0xFFFU;

    QSPI___Disable_MemoryMapped_Mode();
    QSPI___Wait_Flash_Ready();
    QSPI___Write_Enable();

    QSPI___Wait_While_Busy();
    QSPI___Clear_Flags();

    QUADSPI->DLR = 0;   // no data phase

    QUADSPI->CCR =
          (1U << QUADSPI_CCR_IMODE_Pos)                  // instruction on 1 line
        | (1U << QUADSPI_CCR_ADMODE_Pos)                 // address on 1 line
        | (2U << QUADSPI_CCR_ADSIZE_Pos)                 // 24-bit address
        | (0U << QUADSPI_CCR_FMODE_Pos)                  // indirect write
        | (0x20U << QUADSPI_CCR_INSTRUCTION_Pos);        // 4 KB sector erase

    QUADSPI->AR = address;

    while ((QUADSPI->SR & (QUADSPI_SR_TCF | QUADSPI_SR_TEF)) == 0);

    if (QUADSPI->SR & QUADSPI_SR_TEF)
    {
        QUADSPI->FCR = QUADSPI_FCR_CTEF;
        return 0;
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF;

    QSPI___Wait_Flash_Ready();
    return 1;
}

uint8_t QSPI___Read_Bytes(uint32_t address, uint8_t *data, uint32_t length)
{
    if ((data == 0) || (length == 0))
    {
        return 0;
    }

    QSPI___Wait_While_Busy();
    QSPI___Clear_Flags();

    QUADSPI->DLR = length - 1U;

    QUADSPI->CCR =
          (1U << QUADSPI_CCR_IMODE_Pos)           // instruction on 1 line
        | (1U << QUADSPI_CCR_ADMODE_Pos)          // address on 1 line
        | (2U << QUADSPI_CCR_ADSIZE_Pos)          // 24-bit address
        | (1U << QUADSPI_CCR_DMODE_Pos)           // data on 1 line
        | (8U << QUADSPI_CCR_DCYC_Pos)            // 8 dummy cycles
        | (1U << QUADSPI_CCR_FMODE_Pos)           // indirect read
        | (0x0BU << QUADSPI_CCR_INSTRUCTION_Pos); // FAST READ

    QUADSPI->AR = address;

    for (uint32_t i = 0; i < length; i++)
    {
        while (1)
        {
            uint32_t sr = QUADSPI->SR;
            uint32_t flevel = (sr >> QUADSPI_SR_FLEVEL_Pos) & 0x3FU;

            if (sr & QUADSPI_SR_TEF)
            {
                QUADSPI->FCR = QUADSPI_FCR_CTEF;
                return 0;
            }

            if (flevel != 0)
            {
                break;
            }

            if (sr & QUADSPI_SR_TCF)
            {
                return 0;
            }
        }

        data[i] = *(volatile uint8_t *)&QUADSPI->DR;
    }

    while ((QUADSPI->SR & (QUADSPI_SR_TCF | QUADSPI_SR_TEF)) == 0);

    if (QUADSPI->SR & QUADSPI_SR_TEF)
    {
        QUADSPI->FCR = QUADSPI_FCR_CTEF;
        return 0;
    }

    QUADSPI->FCR = QUADSPI_FCR_CTCF;
    return 1;
}

uint8_t QSPI___Program_Page(uint32_t address, const uint8_t *data, uint32_t length)
{
    uint32_t page_offset;

    if ((data == 0) || (length == 0) || (length > 256U))
    {
        return 0;
    }

    page_offset = address & 0xFFU;	// page cross check
    if ((page_offset + length) > 256U)
    {
        return 0;
    }

    QSPI___Wait_Flash_Ready();
    QSPI___Write_Enable();

    QSPI___Wait_While_Busy();
    QSPI___Clear_Flags();

    QUADSPI->DLR = length - 1U;

    QUADSPI->CCR =
          (1U << QUADSPI_CCR_IMODE_Pos)                  // instruction on 1 line
        | (1U << QUADSPI_CCR_ADMODE_Pos)                 // address on 1 line
        | (2U << QUADSPI_CCR_ADSIZE_Pos)                 // 24-bit address
        | (1U << QUADSPI_CCR_DMODE_Pos)                  // data on 1 line
        | (0U << QUADSPI_CCR_FMODE_Pos)                  // indirect write
        | (0x02U << QUADSPI_CCR_INSTRUCTION_Pos);        // Page Program

    QUADSPI->AR = address;

    for (uint32_t i = 0; i < length; i++)
    {
        while ((QUADSPI->SR & QUADSPI_SR_FTF) == 0);
        *(volatile uint8_t *)&QUADSPI->DR = data[i];
    }

    while ((QUADSPI->SR & QUADSPI_SR_TCF) == 0);
    QUADSPI->FCR = QUADSPI_FCR_CTCF;

    QSPI___Wait_Flash_Ready();

    return 1;
}

uint32_t QSPI___Read_ID(void)
{
    uint32_t id = 0;

    while (QUADSPI->SR & QUADSPI_SR_BUSY);

    QUADSPI->FCR = QUADSPI_FCR_CTCF
                 | QUADSPI_FCR_CSMF
                 | QUADSPI_FCR_CTOF
                 | QUADSPI_FCR_CTEF;

    /* Data length = 3 bytes -> DLR = N - 1 */
    QUADSPI->DLR = 2;

    /*
     * CCR setup:
     * IMODE 		= 01 : instruction on 1 line
     * ADMODE 		= 00 : no address
     * ABMODE 		= 00 : no alternate bytes
     * DMODE 		= 01 : data on 1 line
     * FMODE 		= 01 : indirect read
     * INSTRUCTION 	= 0x9F
     */

    QUADSPI->CCR =
          (1U << QUADSPI_CCR_IMODE_Pos)
        | (1U << QUADSPI_CCR_DMODE_Pos)
        | (1U << QUADSPI_CCR_FMODE_Pos)
        | (0x9FU << QUADSPI_CCR_INSTRUCTION_Pos);

    for (uint32_t i = 0; i < 3; i++)	// read 3 bytes
    {
        while ((QUADSPI->SR & QUADSPI_SR_FTF) == 0);

        id <<= 8;
        id |= *(volatile uint8_t *)&QUADSPI->DR;
    }

    while ((QUADSPI->SR & QUADSPI_SR_TCF) == 0);

    QUADSPI->FCR = QUADSPI_FCR_CTCF;	// transfer complete flag clear

    return id;
}

void QSPI___Disable_MemoryMapped_Mode(void)
{
    QSPI___Wait_While_Busy();

    QUADSPI->CR |= QUADSPI_CR_ABORT;

    while (QUADSPI->CR & QUADSPI_CR_ABORT);

    QSPI___Clear_Flags();   // ADD THIS
}

static void QSPI___Write_SR1_SR2(uint8_t sr1, uint8_t sr2)
{
    QSPI___Wait_Flash_Ready();
    QSPI___Write_Enable();

    QSPI___Wait_While_Busy();
    QSPI___Clear_Flags();

    QUADSPI->DLR = 1;   // 2 bytes total

    QUADSPI->CCR =
          (1U << QUADSPI_CCR_IMODE_Pos)                 // instruction on 1 line
        | (1U << QUADSPI_CCR_DMODE_Pos)                 // data on 1 line
        | (0U << QUADSPI_CCR_FMODE_Pos)                 // indirect write
        | (0x01U << QUADSPI_CCR_INSTRUCTION_Pos);       // Write Status Registers

    while ((QUADSPI->SR & QUADSPI_SR_FTF) == 0);
    *(volatile uint8_t *)&QUADSPI->DR = sr1;

    while ((QUADSPI->SR & QUADSPI_SR_FTF) == 0);
    *(volatile uint8_t *)&QUADSPI->DR = sr2;

    while ((QUADSPI->SR & QUADSPI_SR_TCF) == 0);
    QUADSPI->FCR = QUADSPI_FCR_CTCF;

    QSPI___Wait_Flash_Ready();
}

uint8_t QSPI___Enable_QE(void)
{
    uint8_t sr = QSPI___Read_Status();

    if (sr & 0x40U)   // QE = bit 6
    {
        return 1;
    }

    sr |= 0x40U;

    QSPI___Wait_Flash_Ready();
    QSPI___Write_Enable();

    QSPI___Wait_While_Busy();
    QSPI___Clear_Flags();

    QUADSPI->DLR = 0;   // 1 byte total

    QUADSPI->CCR =
          (1U << QUADSPI_CCR_IMODE_Pos)                  // instruction on 1 line
        | (1U << QUADSPI_CCR_DMODE_Pos)                  // data on 1 line
        | (0U << QUADSPI_CCR_FMODE_Pos)                  // indirect write
        | (0x01U << QUADSPI_CCR_INSTRUCTION_Pos);        // WRSR

    while ((QUADSPI->SR & QUADSPI_SR_FTF) == 0);
    *(volatile uint8_t *)&QUADSPI->DR = sr;

    while ((QUADSPI->SR & QUADSPI_SR_TCF) == 0);
    QUADSPI->FCR = QUADSPI_FCR_CTCF;

    QSPI___Wait_Flash_Ready();

    return ((QSPI___Read_Status() & 0x40U) != 0U);
}

void QSPI___Enable_MemoryMapped_Mode(void)
{
    QUADSPI->CR |= QUADSPI_CR_ABORT;	// abort ongoing reads
    while (QUADSPI->CR & QUADSPI_CR_ABORT);

    if ((QSPI___Read_Status() & 0x40U) == 0)
    {
        QSPI___Enable_QE();
    }

    QSPI___Wait_While_Busy();
    QSPI___Clear_Flags();

    uint32_t ccr = 0;

    ccr |= (1U << QUADSPI_CCR_IMODE_Pos);          // 1-line instruction
    ccr |= (1U << QUADSPI_CCR_ADMODE_Pos);         // 1-line address
    ccr |= (2U << QUADSPI_CCR_ADSIZE_Pos);         // 24-bit address
    ccr |= (3U << QUADSPI_CCR_DMODE_Pos);          // 4-line data
    ccr |= (8U << QUADSPI_CCR_DCYC_Pos);           // tune if needed
    ccr |= (0x6BU << QUADSPI_CCR_INSTRUCTION_Pos); // Fast Read Quad Output
    ccr |= (3U << QUADSPI_CCR_FMODE_Pos);          // memory-mapped mode
    ccr |= QUADSPI_CCR_SIOO;                       // send instruction once

    QUADSPI->CCR = ccr;
}

void QSPI___Init(void)
{
    QSPI___GPIO_Init();

    RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN;
    (void)RCC->AHB3ENR;

    QUADSPI->CR = 0;
    while (QUADSPI->SR & QUADSPI_SR_BUSY);

    QUADSPI->CR  = 0x00000000;
    QUADSPI->DCR = 0x00000004;   // documented reset value

    QUADSPI->CR |= (QSPI___PRESCALER_VALUE << QUADSPI_CR_PRESCALER_Pos);

    QUADSPI->DCR |= (QSPI___FSIZE << QUADSPI_DCR_FSIZE_Pos);
    QUADSPI->DCR |= (QSPI___CSHT  << QUADSPI_DCR_CSHT_Pos);

    QUADSPI->CR |= (1U << QUADSPI_CR_FTHRES_Pos); // 1 byte FIFO threshold

    QUADSPI->FCR = QUADSPI_FCR_CTCF		// clear flags
                 | QUADSPI_FCR_CSMF
                 | QUADSPI_FCR_CTOF
                 | QUADSPI_FCR_CTEF;

    QUADSPI->CR |= QUADSPI_CR_SSHIFT;

    QUADSPI->CR |= QUADSPI_CR_EN;

    QSPI___Enable_QE();
    volatile uint8_t qe_after_init = QSPI___Read_Status();
}
