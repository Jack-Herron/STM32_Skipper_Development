/*
 * STM32H7A3_SPI.c
 *
 *  Created on: Jul 11, 2026
 *      Author: Jack Herron
 */

#include "STM32H7A3_SPI.h"
#include "STM32H7A3_RCC.h"

#include <stddef.h>
#include "STM32H7xx.h"

static uint32_t SPI_CalculateBaudRate(
    uint32_t KernelClockHz,
    uint32_t BusFreq)
{
    uint32_t Divider;
    uint32_t BaudRate;

    if((KernelClockHz == 0U) || (BusFreq == 0U))
    {
        return 0xFFFFFFFFU;
    }

    Divider = 2U;
    BaudRate = 0U;

    while(Divider <= 256U)
    {
        if((KernelClockHz / Divider) <= BusFreq)
        {
            return BaudRate;
        }

        Divider <<= 1U;
        BaudRate++;
    }

    return 0xFFFFFFFFU;
}

static uint16_t SPI_GetFrameMask(
    const SPI_BusHandleTypeDef *Bus)
{
    if(Bus->FrameSize == SPI_FRAME_SIZE_9_BIT)
    {
        return 0x01FFU;
    }

    return 0x00FFU;
}

static void SPI_DeviceSelect(
    SPI_DeviceHandleTypeDef *Device)
{
    if(Device->ChipSelectPolarity == SPI_CHIP_SELECT_ACTIVE_LOW)
    {
        Device->ChipSelectPort->BSRR =
            ((uint32_t)Device->ChipSelectPin << 16U);
    }
    else
    {
        Device->ChipSelectPort->BSRR =
            Device->ChipSelectPin;
    }
}

static void SPI_DeviceDeselect(
    SPI_DeviceHandleTypeDef *Device)
{
    if(Device->ChipSelectPolarity == SPI_CHIP_SELECT_ACTIVE_LOW)
    {
        Device->ChipSelectPort->BSRR =
            Device->ChipSelectPin;
    }
    else
    {
        Device->ChipSelectPort->BSRR =
            ((uint32_t)Device->ChipSelectPin << 16U);
    }
}

static SPI_StatusTypeDef SPI_GetError(
    SPI_TypeDef *Instance)
{
    if((Instance->SR & SPI_SR_OVR) != 0U)
    {
        Instance->IFCR = SPI_IFCR_OVRC;
        return SPI_STATUS_OVERRUN;
    }

    if((Instance->SR & SPI_SR_MODF) != 0U)
    {
        Instance->IFCR = SPI_IFCR_MODFC;
        return SPI_STATUS_MODE_FAULT;
    }

    return SPI_STATUS_OK;
}

static SPI_StatusTypeDef SPI_ValidateDevice(
    SPI_DeviceHandleTypeDef *Device,
    uint16_t Length)
{
    if((Device == NULL) ||
       (Device->Bus == NULL) ||
       (Device->Bus->Instance == NULL) ||
       (Device->ChipSelectPort == NULL))
    {
        return SPI_STATUS_ERROR;
    }

    if((Length == 0U) ||
       (Device->ChipSelectPin == 0U))
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    if((Device->Bus->FrameSize != SPI_FRAME_SIZE_8_BIT) &&
       (Device->Bus->FrameSize != SPI_FRAME_SIZE_9_BIT))
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    if(Device->Bus->Direction > SPI_DIRECTION_HALF_DUPLEX)
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    return SPI_STATUS_OK;
}

static void SPI_StopTransfer(
    SPI_DeviceHandleTypeDef *Device)
{
    SPI_TypeDef *Instance;

    Instance = Device->Bus->Instance;

    Instance->CR1 &= ~SPI_CR1_SPE;
    SPI_DeviceDeselect(Device);
}

static SPI_StatusTypeDef SPI_WaitForFlag(
    SPI_DeviceHandleTypeDef *Device,
    uint32_t Flag)
{
    SPI_TypeDef *Instance;
    SPI_StatusTypeDef Status;
    uint32_t Timeout;

    Instance = Device->Bus->Instance;
    Timeout = Device->Bus->Timeout;

    while((Instance->SR & Flag) == 0U)
    {
        Status = SPI_GetError(Instance);

        if(Status != SPI_STATUS_OK)
        {
            return Status;
        }

        if(Timeout-- == 0U)
        {
            return SPI_STATUS_TIMEOUT;
        }
    }

    return SPI_STATUS_OK;
}

static void SPI_WriteFrame(
    SPI_DeviceHandleTypeDef *Device,
    uint16_t Frame)
{
    if(Device->Bus->FrameSize == SPI_FRAME_SIZE_9_BIT)
    {
        *((__IO uint16_t *)&Device->Bus->Instance->TXDR) =
            Frame;
    }
    else
    {
        *((__IO uint8_t *)&Device->Bus->Instance->TXDR) =
            (uint8_t)Frame;
    }
}

static uint16_t SPI_ReadFrame(
    SPI_DeviceHandleTypeDef *Device)
{
    if(Device->Bus->FrameSize == SPI_FRAME_SIZE_9_BIT)
    {
        return *((__IO uint16_t *)&Device->Bus->Instance->RXDR);
    }

    return *((__IO uint8_t *)&Device->Bus->Instance->RXDR);
}

SPI_StatusTypeDef SPI_Init(
    SPI_BusHandleTypeDef *Handle)
{
    uint32_t KernelClockHz;
    uint32_t BaudRate;
    uint32_t Config1;
    uint32_t Config2;

    if((Handle == NULL) ||
       (Handle->Instance == NULL))
    {
        return SPI_STATUS_ERROR;
    }

    KernelClockHz = RCC_GetKernelFreq(Handle->Instance);

    if((KernelClockHz == 0U) ||
       (Handle->BusFreq == 0U))
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    if((Handle->Mode > SPI_MODE_3) ||
       (Handle->BitOrder > SPI_BIT_ORDER_LSB_FIRST))
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    if((Handle->FrameSize != SPI_FRAME_SIZE_8_BIT) &&
       (Handle->FrameSize != SPI_FRAME_SIZE_9_BIT))
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    if(Handle->Direction > SPI_DIRECTION_HALF_DUPLEX)
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    BaudRate = SPI_CalculateBaudRate(
        KernelClockHz,
        Handle->BusFreq);

    if(BaudRate == 0xFFFFFFFFU)
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    RCC_EnablePeripheralClock(Handle->Instance);

    Handle->Instance->CR1 &= ~SPI_CR1_SPE;

    /*
     * SSI must be high when software slave management is enabled,
     * otherwise the master can enter mode-fault state.
     */
    Handle->Instance->CR1 = SPI_CR1_SSI;
    Handle->Instance->CR2 = 0U;
    Handle->Instance->CFG1 = 0U;
    Handle->Instance->CFG2 = 0U;
    Handle->Instance->IER = 0U;
    Handle->Instance->IFCR = 0xFFFFFFFFU;

    Config1 =
        (((uint32_t)Handle->FrameSize - 1U)
            << SPI_CFG1_DSIZE_Pos) |
        ((BaudRate & 0x07U)
            << SPI_CFG1_MBR_Pos);

    Handle->Instance->CFG1 = Config1;

    Config2 =
        SPI_CFG2_MASTER |
        SPI_CFG2_SSM |
        SPI_CFG2_AFCNTR;

    if((Handle->Mode == SPI_MODE_1) ||
       (Handle->Mode == SPI_MODE_3))
    {
        Config2 |= SPI_CFG2_CPHA;
    }

    if((Handle->Mode == SPI_MODE_2) ||
       (Handle->Mode == SPI_MODE_3))
    {
        Config2 |= SPI_CFG2_CPOL;
    }

    if(Handle->BitOrder == SPI_BIT_ORDER_LSB_FIRST)
    {
        Config2 |= SPI_CFG2_LSBFRST;
    }

    switch(Handle->Direction)
    {
        case SPI_DIRECTION_FULL_DUPLEX:
            break;

        case SPI_DIRECTION_TX_ONLY:
            Config2 |= SPI_CFG2_COMM_0;
            break;

        case SPI_DIRECTION_RX_ONLY:
            Config2 |= SPI_CFG2_COMM_1;
            break;

        case SPI_DIRECTION_HALF_DUPLEX:
            Config2 |= SPI_CFG2_COMM_0 |
                       SPI_CFG2_COMM_1;
            break;

        default:
            return SPI_STATUS_INVALID_PARAMETER;
    }

    Handle->Instance->CFG2 = Config2;

    return SPI_STATUS_OK;
}

SPI_StatusTypeDef SPI_DeviceTransferFrames(
    SPI_DeviceHandleTypeDef *Device,
    const uint16_t *TxData,
    uint16_t *RxData,
    uint16_t Length)
{
    SPI_TypeDef *Instance;
    SPI_StatusTypeDef Status;
    uint16_t FrameMask;
    uint16_t Index;
    uint16_t TransmitFrame;
    uint16_t ReceivedFrame;

    Status = SPI_ValidateDevice(Device, Length);

    if(Status != SPI_STATUS_OK)
    {
        return Status;
    }

    if(Device->Bus->Direction != SPI_DIRECTION_FULL_DUPLEX)
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    Instance = Device->Bus->Instance;
    FrameMask = SPI_GetFrameMask(Device->Bus);

    if((Instance->CR1 & SPI_CR1_SPE) != 0U)
    {
        return SPI_STATUS_BUSY;
    }

    Instance->IFCR = 0xFFFFFFFFU;
    Instance->CR2 =
        ((uint32_t)Length << SPI_CR2_TSIZE_Pos);

    SPI_DeviceSelect(Device);

    Instance->CR1 |= SPI_CR1_SPE;

    /*
     * Preload the first frame before starting the master transfer.
     */
    Status = SPI_WaitForFlag(Device, SPI_SR_TXP);

    if(Status != SPI_STATUS_OK)
    {
        SPI_StopTransfer(Device);
        return Status;
    }

    if(TxData != NULL)
    {
        TransmitFrame = TxData[0U] & FrameMask;
    }
    else
    {
        TransmitFrame = FrameMask;
    }

    SPI_WriteFrame(Device, TransmitFrame);
    Instance->CR1 |= SPI_CR1_CSTART;

    for(Index = 0U; Index < Length; Index++)
    {
        if(Index > 0U)
        {
            Status = SPI_WaitForFlag(Device, SPI_SR_TXP);

            if(Status != SPI_STATUS_OK)
            {
                SPI_StopTransfer(Device);
                return Status;
            }

            if(TxData != NULL)
            {
                TransmitFrame = TxData[Index] & FrameMask;
            }
            else
            {
                TransmitFrame = FrameMask;
            }

            SPI_WriteFrame(Device, TransmitFrame);
        }

        Status = SPI_WaitForFlag(Device, SPI_SR_RXP);

        if(Status != SPI_STATUS_OK)
        {
            SPI_StopTransfer(Device);
            return Status;
        }

        ReceivedFrame = SPI_ReadFrame(Device) & FrameMask;

        if(RxData != NULL)
        {
            RxData[Index] = ReceivedFrame;
        }
    }

    Status = SPI_WaitForFlag(Device, SPI_SR_EOT);

    if(Status != SPI_STATUS_OK)
    {
        SPI_StopTransfer(Device);
        return Status;
    }

    Instance->IFCR =
        SPI_IFCR_EOTC |
        SPI_IFCR_TXTFC;

    Instance->CR1 &= ~SPI_CR1_SPE;
    SPI_DeviceDeselect(Device);

    return SPI_STATUS_OK;
}

SPI_StatusTypeDef SPI_DeviceReadFrames(
    SPI_DeviceHandleTypeDef *Device,
    uint16_t *Data,
    uint16_t Length)
{
    if(Data == NULL)
    {
        return SPI_STATUS_ERROR;
    }

    if((Device == NULL) ||
       (Device->Bus == NULL) ||
       (Device->Bus->Direction != SPI_DIRECTION_FULL_DUPLEX))
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    return SPI_DeviceTransferFrames(
        Device,
        NULL,
        Data,
        Length);
}

SPI_StatusTypeDef SPI_DeviceWriteFrames(
    SPI_DeviceHandleTypeDef *Device,
    const uint16_t *Data,
    uint16_t Length)
{
    SPI_TypeDef *Instance;
    SPI_StatusTypeDef Status;
    uint16_t FrameMask;
    uint16_t Index;
    uint16_t ReceivedFrame;

    Status = SPI_ValidateDevice(Device, Length);

    if(Status != SPI_STATUS_OK)
    {
        return Status;
    }

    if(Data == NULL)
    {
        return SPI_STATUS_ERROR;
    }

    if((Device->Bus->Direction != SPI_DIRECTION_TX_ONLY) &&
       (Device->Bus->Direction != SPI_DIRECTION_FULL_DUPLEX))
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    Instance = Device->Bus->Instance;
    FrameMask = SPI_GetFrameMask(Device->Bus);

    if((Instance->CR1 & SPI_CR1_SPE) != 0U)
    {
        return SPI_STATUS_BUSY;
    }

    Instance->IFCR = 0xFFFFFFFFU;
    Instance->CR2 =
        ((uint32_t)Length << SPI_CR2_TSIZE_Pos);

    SPI_DeviceSelect(Device);

    Instance->CR1 |= SPI_CR1_SPE;

    /*
     * Preload the first frame before CSTART. This is important for
     * the STM32H7 master transfer state machine.
     */
    Status = SPI_WaitForFlag(Device, SPI_SR_TXP);

    if(Status != SPI_STATUS_OK)
    {
        SPI_StopTransfer(Device);
        return Status;
    }

    SPI_WriteFrame(
        Device,
        Data[0U] & FrameMask);

    Instance->CR1 |= SPI_CR1_CSTART;

    for(Index = 1U; Index < Length; Index++)
    {
        Status = SPI_WaitForFlag(Device, SPI_SR_TXP);

        if(Status != SPI_STATUS_OK)
        {
            SPI_StopTransfer(Device);
            return Status;
        }

        SPI_WriteFrame(
            Device,
            Data[Index] & FrameMask);

        /*
         * In full-duplex mode, drain RXDR so an unused received
         * frame cannot cause an overrun.
         */
        if((Device->Bus->Direction == SPI_DIRECTION_FULL_DUPLEX) &&
           ((Instance->SR & SPI_SR_RXP) != 0U))
        {
            ReceivedFrame = SPI_ReadFrame(Device);
            (void)ReceivedFrame;
        }
    }

    /*
     * Drain any final receive data when writing in full-duplex mode.
     */
    if(Device->Bus->Direction == SPI_DIRECTION_FULL_DUPLEX)
    {
        while((Instance->SR & SPI_SR_EOT) == 0U)
        {
            Status = SPI_GetError(Instance);

            if(Status != SPI_STATUS_OK)
            {
                SPI_StopTransfer(Device);
                return Status;
            }

            if((Instance->SR & SPI_SR_RXP) != 0U)
            {
                ReceivedFrame = SPI_ReadFrame(Device);
                (void)ReceivedFrame;
            }

            if(Device->Bus->Timeout == 0U)
            {
                SPI_StopTransfer(Device);
                return SPI_STATUS_TIMEOUT;
            }

            {
                uint32_t Timeout = Device->Bus->Timeout;

                while(((Instance->SR &
                       (SPI_SR_RXP |
                        SPI_SR_EOT |
                        SPI_SR_OVR |
                        SPI_SR_MODF)) == 0U))
                {
                    if(Timeout-- == 0U)
                    {
                        SPI_StopTransfer(Device);
                        return SPI_STATUS_TIMEOUT;
                    }
                }
            }
        }
    }
    else
    {
        Status = SPI_WaitForFlag(Device, SPI_SR_EOT);

        if(Status != SPI_STATUS_OK)
        {
            SPI_StopTransfer(Device);
            return Status;
        }
    }

    Instance->IFCR =
        SPI_IFCR_EOTC |
        SPI_IFCR_TXTFC;

    Instance->CR1 &= ~SPI_CR1_SPE;
    SPI_DeviceDeselect(Device);

    return SPI_STATUS_OK;
}

SPI_StatusTypeDef SPI_DeviceTransfer(
    SPI_DeviceHandleTypeDef *Device,
    const uint8_t *TxData,
    uint8_t *RxData,
    uint16_t Length)
{
    SPI_TypeDef *Instance;
    SPI_StatusTypeDef Status;
    uint16_t Index;
    uint8_t TransmitData;
    uint8_t ReceivedData;

    Status = SPI_ValidateDevice(Device, Length);

    if(Status != SPI_STATUS_OK)
    {
        return Status;
    }

    if(Device->Bus->FrameSize != SPI_FRAME_SIZE_8_BIT)
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    if(Device->Bus->Direction != SPI_DIRECTION_FULL_DUPLEX)
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    Instance = Device->Bus->Instance;

    if((Instance->CR1 & SPI_CR1_SPE) != 0U)
    {
        return SPI_STATUS_BUSY;
    }

    Instance->IFCR = 0xFFFFFFFFU;
    Instance->CR2 =
        ((uint32_t)Length << SPI_CR2_TSIZE_Pos);

    SPI_DeviceSelect(Device);

    Instance->CR1 |= SPI_CR1_SPE;

    Status = SPI_WaitForFlag(Device, SPI_SR_TXP);

    if(Status != SPI_STATUS_OK)
    {
        SPI_StopTransfer(Device);
        return Status;
    }

    if(TxData != NULL)
    {
        TransmitData = TxData[0U];
    }
    else
    {
        TransmitData = 0xFFU;
    }

    *((__IO uint8_t *)&Instance->TXDR) = TransmitData;
    Instance->CR1 |= SPI_CR1_CSTART;

    for(Index = 0U; Index < Length; Index++)
    {
        if(Index > 0U)
        {
            Status = SPI_WaitForFlag(Device, SPI_SR_TXP);

            if(Status != SPI_STATUS_OK)
            {
                SPI_StopTransfer(Device);
                return Status;
            }

            if(TxData != NULL)
            {
                TransmitData = TxData[Index];
            }
            else
            {
                TransmitData = 0xFFU;
            }

            *((__IO uint8_t *)&Instance->TXDR) =
                TransmitData;
        }

        Status = SPI_WaitForFlag(Device, SPI_SR_RXP);

        if(Status != SPI_STATUS_OK)
        {
            SPI_StopTransfer(Device);
            return Status;
        }

        ReceivedData =
            *((__IO uint8_t *)&Instance->RXDR);

        if(RxData != NULL)
        {
            RxData[Index] = ReceivedData;
        }
    }

    Status = SPI_WaitForFlag(Device, SPI_SR_EOT);

    if(Status != SPI_STATUS_OK)
    {
        SPI_StopTransfer(Device);
        return Status;
    }

    Instance->IFCR =
        SPI_IFCR_EOTC |
        SPI_IFCR_TXTFC;

    Instance->CR1 &= ~SPI_CR1_SPE;
    SPI_DeviceDeselect(Device);

    return SPI_STATUS_OK;
}

SPI_StatusTypeDef SPI_DeviceRead(
    SPI_DeviceHandleTypeDef *Device,
    uint8_t *Data,
    uint16_t Length)
{
    if(Data == NULL)
    {
        return SPI_STATUS_ERROR;
    }

    if((Device == NULL) ||
       (Device->Bus == NULL) ||
       (Device->Bus->Direction != SPI_DIRECTION_FULL_DUPLEX))
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    return SPI_DeviceTransfer(
        Device,
        NULL,
        Data,
        Length);
}

SPI_StatusTypeDef SPI_DeviceWrite(
    SPI_DeviceHandleTypeDef *Device,
    const uint8_t *Data,
    uint16_t Length)
{
    SPI_TypeDef *Instance;
    SPI_StatusTypeDef Status;
    uint16_t Index;
    uint8_t ReceivedData;

    Status = SPI_ValidateDevice(Device, Length);

    if(Status != SPI_STATUS_OK)
    {
        return Status;
    }

    if(Data == NULL)
    {
        return SPI_STATUS_ERROR;
    }

    if(Device->Bus->FrameSize != SPI_FRAME_SIZE_8_BIT)
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    if((Device->Bus->Direction != SPI_DIRECTION_TX_ONLY) &&
       (Device->Bus->Direction != SPI_DIRECTION_FULL_DUPLEX))
    {
        return SPI_STATUS_INVALID_PARAMETER;
    }

    Instance = Device->Bus->Instance;

    if((Instance->CR1 & SPI_CR1_SPE) != 0U)
    {
        return SPI_STATUS_BUSY;
    }

    Instance->IFCR = 0xFFFFFFFFU;
    Instance->CR2 =
        ((uint32_t)Length << SPI_CR2_TSIZE_Pos);

    SPI_DeviceSelect(Device);

    Instance->CR1 |= SPI_CR1_SPE;

    Status = SPI_WaitForFlag(Device, SPI_SR_TXP);

    if(Status != SPI_STATUS_OK)
    {
        SPI_StopTransfer(Device);
        return Status;
    }

    *((__IO uint8_t *)&Instance->TXDR) = Data[0U];
    Instance->CR1 |= SPI_CR1_CSTART;

    for(Index = 1U; Index < Length; Index++)
    {
        Status = SPI_WaitForFlag(Device, SPI_SR_TXP);

        if(Status != SPI_STATUS_OK)
        {
            SPI_StopTransfer(Device);
            return Status;
        }

        *((__IO uint8_t *)&Instance->TXDR) =
            Data[Index];

        if((Device->Bus->Direction == SPI_DIRECTION_FULL_DUPLEX) &&
           ((Instance->SR & SPI_SR_RXP) != 0U))
        {
            ReceivedData =
                *((__IO uint8_t *)&Instance->RXDR);

            (void)ReceivedData;
        }
    }

    if(Device->Bus->Direction == SPI_DIRECTION_FULL_DUPLEX)
    {
        uint32_t Timeout = Device->Bus->Timeout;

        while((Instance->SR & SPI_SR_EOT) == 0U)
        {
            Status = SPI_GetError(Instance);

            if(Status != SPI_STATUS_OK)
            {
                SPI_StopTransfer(Device);
                return Status;
            }

            if((Instance->SR & SPI_SR_RXP) != 0U)
            {
                ReceivedData =
                    *((__IO uint8_t *)&Instance->RXDR);

                (void)ReceivedData;
            }

            if(Timeout-- == 0U)
            {
                SPI_StopTransfer(Device);
                return SPI_STATUS_TIMEOUT;
            }
        }
    }
    else
    {
        Status = SPI_WaitForFlag(Device, SPI_SR_EOT);

        if(Status != SPI_STATUS_OK)
        {
            SPI_StopTransfer(Device);
            return Status;
        }
    }

    Instance->IFCR =
        SPI_IFCR_EOTC |
        SPI_IFCR_TXTFC;

    Instance->CR1 &= ~SPI_CR1_SPE;
    SPI_DeviceDeselect(Device);

    return SPI_STATUS_OK;
}
