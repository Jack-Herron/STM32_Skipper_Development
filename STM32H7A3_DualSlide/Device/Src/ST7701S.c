/*
 * ST7701S.c
 *
 *  Created on: Jul 11, 2026
 *      Author: Jack Herron
 */

#include "ST7701S.h"

#include <stddef.h>

#define ST7701S_COMMAND_FRAME(Value) \
    ((uint16_t)(Value) & 0x00FFU)

#define ST7701S_DATA_FRAME(Value) \
    (0x0100U | ((uint16_t)(Value) & 0x00FFU))

static void ST7701S_SetReset(
    ST7701S_HandleTypeDef *Handle,
    uint8_t Asserted)
{
    uint8_t OutputLow;

    OutputLow = Asserted;

    if(Handle->ResetActiveLow == 0U)
    {
        OutputLow = (uint8_t)!Asserted;
    }

    if(OutputLow != 0U)
    {
        Handle->ResetPort->BSRR =
            ((uint32_t)Handle->ResetPin << 16U);
    }
    else
    {
        Handle->ResetPort->BSRR =
            Handle->ResetPin;
    }
}

static ST7701S_StatusTypeDef ST7701S_ConvertSPIStatus(
    SPI_StatusTypeDef Status)
{
    switch(Status)
    {
        case SPI_STATUS_OK:
            return ST7701S_STATUS_OK;

        case SPI_STATUS_TIMEOUT:
            return ST7701S_STATUS_TIMEOUT;

        default:
           return ST7701S_STATUS_ERROR;
    }
}

static ST7701S_StatusTypeDef ST7701S_ValidateHandle(
    const ST7701S_HandleTypeDef *Handle)
{
    if((Handle == NULL) ||
       (Handle->SPI == NULL) ||
       (Handle->SPI->Bus == NULL) ||
       (Handle->ResetPort == NULL) ||
       (Handle->ResetPin == 0U))
    {
        return ST7701S_STATUS_INVALID_PARAMETER;
    }

    if(Handle->SPI->FrameSize != SPI_FRAME_SIZE_9_BIT)
    {
        return ST7701S_STATUS_INVALID_PARAMETER;
    }

    return ST7701S_STATUS_OK;
}

ST7701S_StatusTypeDef ST7701S_WriteCommand(
    ST7701S_HandleTypeDef *Handle,
    uint8_t Command)
{
    uint16_t Frame;
    SPI_StatusTypeDef SPIStatus;

    if(ST7701S_ValidateHandle(Handle) != ST7701S_STATUS_OK)
    {
        return ST7701S_STATUS_INVALID_PARAMETER;
    }

    Frame = ST7701S_COMMAND_FRAME(Command);

    SPIStatus = SPI_DeviceWriteFrames(
        Handle->SPI,
        &Frame,
        1U);

    return ST7701S_ConvertSPIStatus(SPIStatus);
}

ST7701S_StatusTypeDef ST7701S_WriteData(
    ST7701S_HandleTypeDef *Handle,
    uint8_t Data)
{
    uint16_t Frame;
    SPI_StatusTypeDef SPIStatus;

    if(ST7701S_ValidateHandle(Handle) != ST7701S_STATUS_OK)
    {
        return ST7701S_STATUS_INVALID_PARAMETER;
    }

    Frame = ST7701S_DATA_FRAME(Data);

    SPIStatus = SPI_DeviceWriteFrames(
        Handle->SPI,
        &Frame,
        1U);

    return ST7701S_ConvertSPIStatus(SPIStatus);
}

ST7701S_StatusTypeDef ST7701S_WriteDataBuffer(
    ST7701S_HandleTypeDef *Handle,
    const uint8_t *Data,
    uint16_t Length)
{
    ST7701S_StatusTypeDef Status;
    uint16_t Index;

    if((Data == NULL) || (Length == 0U))
    {
        return ST7701S_STATUS_INVALID_PARAMETER;
    }

    for(Index = 0U; Index < Length; Index++)
    {
        Status = ST7701S_WriteData(Handle, Data[Index]);

        if(Status != ST7701S_STATUS_OK)
        {
            return Status;
        }
    }

    return ST7701S_STATUS_OK;
}

ST7701S_StatusTypeDef ST7701S_WriteRegister(
    ST7701S_HandleTypeDef *Handle,
    uint8_t Command,
    const uint8_t *Data,
    uint16_t Length)
{
    ST7701S_StatusTypeDef Status;

    Status = ST7701S_WriteCommand(Handle, Command);

    if(Status != ST7701S_STATUS_OK)
    {
        return Status;
    }

    if(Length == 0U)
    {
        return ST7701S_STATUS_OK;
    }

    if(Data == NULL)
    {
        return ST7701S_STATUS_INVALID_PARAMETER;
    }

    return ST7701S_WriteDataBuffer(
        Handle,
        Data,
        Length);
}

ST7701S_StatusTypeDef ST7701S_HardwareReset(
    ST7701S_HandleTypeDef *Handle)
{
    ST7701S_StatusTypeDef Status;

    Status = ST7701S_ValidateHandle(Handle);

    if(Status != ST7701S_STATUS_OK)
    {
        return Status;
    }

    ST7701S_SetReset(Handle, 0U);
    Handle->Delay_ms(10U);

    ST7701S_SetReset(Handle, 1U);
    Handle->Delay_ms(20U);

    ST7701S_SetReset(Handle, 0U);
    Handle->Delay_ms(120U);

    return ST7701S_STATUS_OK;
}

ST7701S_StatusTypeDef ST7701S_ExitSleep(
    ST7701S_HandleTypeDef *Handle)
{
    ST7701S_StatusTypeDef Status;

    Status = ST7701S_WriteCommand(Handle, 0x11U);

    if(Status != ST7701S_STATUS_OK)
    {
        return Status;
    }

    Handle->Delay_ms(120U);

    return ST7701S_STATUS_OK;
}

ST7701S_StatusTypeDef ST7701S_DisplayOn(
    ST7701S_HandleTypeDef *Handle)
{
    return ST7701S_WriteCommand(Handle, 0x29U);
}

ST7701S_StatusTypeDef ST7701S_DisplayOff(
    ST7701S_HandleTypeDef *Handle)
{
    return ST7701S_WriteCommand(Handle, 0x28U);
}

ST7701S_StatusTypeDef ST7701S_EnterSleep(
    ST7701S_HandleTypeDef *Handle)
{
    ST7701S_StatusTypeDef Status;

    Status = ST7701S_WriteCommand(Handle, 0x10U);

    if(Status != ST7701S_STATUS_OK)
    {
        return Status;
    }

    Handle->Delay_ms(120U);

    return ST7701S_STATUS_OK;
}
