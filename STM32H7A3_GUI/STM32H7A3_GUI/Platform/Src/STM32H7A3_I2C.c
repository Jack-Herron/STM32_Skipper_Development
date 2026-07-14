/*
 * STM32H7A3_I2C.c
 *
 *  Created on: May 22, 2026
 *      Author: Jack Herron
 */
#include "STM32H7A3_I2C.h"
#include "STM32H7A3_RCC.h"
#include <STDLIB.h>
#include "STM32H7xx.h"

static uint32_t I2C_CalculateTiming(uint32_t KernelClockHz, uint32_t BusFreq)
{

    uint32_t Prescaler;
    uint32_t TicksPerPeriod;
    uint32_t SCLLow;
    uint32_t SCLHigh;
    uint32_t SCLDelay;
    uint32_t SDADelay;

    for(Prescaler = 0U; Prescaler <= 15U; Prescaler++)
    {
        TicksPerPeriod = KernelClockHz / (BusFreq * (Prescaler + 1U));

        if((TicksPerPeriod >= 4U) && (TicksPerPeriod <= 512U))
        {
            SCLLow = (TicksPerPeriod / 2U) - 1U;
            SCLHigh = (TicksPerPeriod - (SCLLow + 1U)) - 1U;

            if((SCLLow <= 255U) && (SCLHigh <= 255U))
            {
                SCLDelay = 4U;
                SDADelay = 2U;

                return ((Prescaler & 0x0FU) << I2C_TIMINGR_PRESC_Pos) |
                       ((SCLDelay  & 0x0FU) << I2C_TIMINGR_SCLDEL_Pos) |
                       ((SDADelay  & 0x0FU) << I2C_TIMINGR_SDADEL_Pos) |
                       ((SCLHigh   & 0xFFU) << I2C_TIMINGR_SCLH_Pos) |
                       ((SCLLow    & 0xFFU) << I2C_TIMINGR_SCLL_Pos);
            }
        }
    }

    return 0xFFFFFFFFU;
}

I2C_StatusTypeDef I2C_Init(I2C_BusHandleTypeDef *Handle)
{
	RCC_EnablePeripheralClock(Handle->Instance);

    uint32_t TimingRegister;

    if((Handle == NULL) || (Handle->Instance == NULL))
    {
        return I2C_STATUS_ERROR;
    }

    if((RCC_GetKernelFreq(Handle->Instance) == 0U) || (Handle->BusFreq == 0U))
    {
        return I2C_STATUS_INVALID_PARAMETER;
    }

    if(Handle->Instance == I2C4)
    {
        RCC->APB4ENR |= RCC_APB4ENR_I2C4EN;
        (void)RCC->APB4ENR;
    }
    else
    {
        return I2C_STATUS_INVALID_PARAMETER;
    }

    TimingRegister = I2C_CalculateTiming(RCC_GetKernelFreq(Handle->Instance), Handle->BusFreq);

    if(TimingRegister == 0xFFFFFFFFU)
    {
        return I2C_STATUS_INVALID_PARAMETER;
    }

    Handle->Instance->CR1 &= ~I2C_CR1_PE;

    Handle->Instance->CR1 = 0U;
    Handle->Instance->CR2 = 0U;
    Handle->Instance->ICR = 0xFFFFFFFFU;

    Handle->Instance->TIMINGR = TimingRegister;

    Handle->Instance->CR1 |= I2C_CR1_PE;

    return I2C_STATUS_OK;
}

I2C_StatusTypeDef I2C_DeviceRead(I2C_DeviceHandleTypeDef *Device, uint8_t *Data, uint16_t Length)
{
    I2C_TypeDef *Instance;
    uint32_t Timeout;

    if((Device == NULL) || (Device->Bus == NULL) || (Device->Bus->Instance == NULL) || (Data == NULL))
    {
        return I2C_STATUS_ERROR;
    }

    if(Length == 0U)
    {
        return I2C_STATUS_INVALID_PARAMETER;
    }

    Instance = Device->Bus->Instance;

    Timeout = Device->Bus->Timeout;
    while((Instance->ISR & I2C_ISR_BUSY) != 0U)
    {
        if(Timeout-- == 0U)
        {
            return I2C_STATUS_BUSY;
        }
    }

    Instance->ICR = I2C_ICR_ADDRCF  |
                    I2C_ICR_NACKCF  |
                    I2C_ICR_STOPCF  |
                    I2C_ICR_BERRCF  |
                    I2C_ICR_ARLOCF  |
                    I2C_ICR_OVRCF   |
                    I2C_ICR_PECCF   |
                    I2C_ICR_TIMOUTCF |
                    I2C_ICR_ALERTCF;

    Instance->CR2 =
        ((uint32_t)(Device->Address << 1U) & I2C_CR2_SADD) |
        ((uint32_t)Length << I2C_CR2_NBYTES_Pos) |
        I2C_CR2_RD_WRN |
        I2C_CR2_AUTOEND |
        I2C_CR2_START;

    while(Length > 0U)
    {
        Timeout = Device->Bus->Timeout;

        while((Instance->ISR & I2C_ISR_RXNE) == 0U)
        {
            if((Instance->ISR & I2C_ISR_NACKF) != 0U)
            {
                Instance->ICR = I2C_ICR_NACKCF;
                return I2C_STATUS_NACK;
            }

            if((Instance->ISR & I2C_ISR_ARLO) != 0U)
            {
                Instance->ICR = I2C_ICR_ARLOCF;
                return I2C_STATUS_ARBITRATION_LOST;
            }

            if((Instance->ISR & I2C_ISR_STOPF) != 0U)
            {
                Instance->ICR = I2C_ICR_STOPCF;
                return I2C_STATUS_ERROR;
            }

            if(Timeout-- == 0U)
            {
                return I2C_STATUS_TIMEOUT;
            }
        }

        *Data = (uint8_t)Instance->RXDR;

        Data++;
        Length--;
    }

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_STOPF) == 0U)
    {
        if(Timeout-- == 0U)
        {
            return I2C_STATUS_TIMEOUT;
        }
    }

    Instance->ICR = I2C_ICR_STOPCF;

    return I2C_STATUS_OK;
}

I2C_StatusTypeDef I2C_DeviceWrite(I2C_DeviceHandleTypeDef *Device, const uint8_t *Data, uint16_t Length)
{
    I2C_TypeDef *Instance;
    uint32_t Timeout;

    if((Device == NULL) || (Device->Bus == NULL) || (Device->Bus->Instance == NULL) || (Data == NULL))
    {
        return I2C_STATUS_ERROR;
    }

    if(Length == 0U)
    {
        return I2C_STATUS_INVALID_PARAMETER;
    }

    Instance = Device->Bus->Instance;

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_BUSY) != 0U)
    {
        if(Timeout-- == 0U)
        {
            return I2C_STATUS_BUSY;
        }
    }

    Instance->ICR =
        I2C_ICR_ADDRCF   |
        I2C_ICR_NACKCF   |
        I2C_ICR_STOPCF   |
        I2C_ICR_BERRCF   |
        I2C_ICR_ARLOCF   |
        I2C_ICR_OVRCF    |
        I2C_ICR_PECCF    |
        I2C_ICR_TIMOUTCF |
        I2C_ICR_ALERTCF;

    Instance->CR2 =
        ((uint32_t)(Device->Address << 1U) & I2C_CR2_SADD) |
        ((uint32_t)Length << I2C_CR2_NBYTES_Pos) |
        I2C_CR2_AUTOEND |
        I2C_CR2_START;

    while(Length > 0U)
    {
        Timeout = Device->Bus->Timeout;

        while((Instance->ISR & I2C_ISR_TXIS) == 0U)
        {
            if((Instance->ISR & I2C_ISR_NACKF) != 0U)
            {
                Instance->ICR = I2C_ICR_NACKCF;
                return I2C_STATUS_NACK;
            }

            if((Instance->ISR & I2C_ISR_ARLO) != 0U)
            {
                Instance->ICR = I2C_ICR_ARLOCF;
                return I2C_STATUS_ARBITRATION_LOST;
            }

            if((Instance->ISR & I2C_ISR_STOPF) != 0U)
            {
                Instance->ICR = I2C_ICR_STOPCF;
                return I2C_STATUS_ERROR;
            }

            if(Timeout-- == 0U)
            {
                return I2C_STATUS_TIMEOUT;
            }
        }

        Instance->TXDR = *Data;

        Data++;
        Length--;
    }

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_STOPF) == 0U)
    {
        if(Timeout-- == 0U)
        {
            return I2C_STATUS_TIMEOUT;
        }
    }

    Instance->ICR = I2C_ICR_STOPCF;

    return I2C_STATUS_OK;
}

I2C_StatusTypeDef I2C_DeviceMemWrite8(I2C_DeviceHandleTypeDef *Device, uint8_t Register, uint8_t Data)
{
    uint8_t Buffer[2];

    if(Device == NULL)
    {
        return I2C_STATUS_ERROR;
    }

    Buffer[0] = Register;
    Buffer[1] = Data;

    return I2C_DeviceWrite(Device, Buffer, 2U);
}

I2C_StatusTypeDef I2C_DeviceMemRead8(I2C_DeviceHandleTypeDef *Device, uint8_t Register, uint8_t *Data)
{
    I2C_TypeDef *Instance;
    uint32_t Timeout;

    if((Device == NULL) || (Device->Bus == NULL) || (Device->Bus->Instance == NULL) || (Data == NULL))
    {
        return I2C_STATUS_ERROR;
    }

    Instance = Device->Bus->Instance;

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_BUSY) != 0U)
    {
        if(Timeout-- == 0U)
        {
            return I2C_STATUS_BUSY;
        }
    }

    Instance->ICR = I2C_ICR_ADDRCF | I2C_ICR_NACKCF | I2C_ICR_STOPCF | I2C_ICR_BERRCF |
                    I2C_ICR_ARLOCF | I2C_ICR_OVRCF | I2C_ICR_PECCF | I2C_ICR_TIMOUTCF | I2C_ICR_ALERTCF;

    /*
     * Write register address, no STOP.
     */

    Instance->CR2 =
        ((uint32_t)(Device->Address << 1U) & I2C_CR2_SADD) |
        (1U << I2C_CR2_NBYTES_Pos) |
        I2C_CR2_START;

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_TXIS) == 0U)
    {
        if((Instance->ISR & I2C_ISR_NACKF) != 0U)
        {
            Instance->ICR = I2C_ICR_NACKCF;
            return I2C_STATUS_NACK;
        }

        if((Instance->ISR & I2C_ISR_ARLO) != 0U)
        {
            Instance->ICR = I2C_ICR_ARLOCF;
            return I2C_STATUS_ARBITRATION_LOST;
        }

        if(Timeout-- == 0U)
        {
            return I2C_STATUS_TIMEOUT;
        }
    }

    Instance->TXDR = Register;

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_TC) == 0U)
    {
        if((Instance->ISR & I2C_ISR_NACKF) != 0U)
        {
            Instance->ICR = I2C_ICR_NACKCF;
            return I2C_STATUS_NACK;
        }

        if((Instance->ISR & I2C_ISR_ARLO) != 0U)
        {
            Instance->ICR = I2C_ICR_ARLOCF;
            return I2C_STATUS_ARBITRATION_LOST;
        }

        if(Timeout-- == 0U)
        {
            return I2C_STATUS_TIMEOUT;
        }
    }

    /*
     * Repeated START, read one byte, auto STOP.
     */

    Instance->CR2 =
        ((uint32_t)(Device->Address << 1U) & I2C_CR2_SADD) |
        (1U << I2C_CR2_NBYTES_Pos) |
        I2C_CR2_RD_WRN |
        I2C_CR2_AUTOEND |
        I2C_CR2_START;

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_RXNE) == 0U)
    {
        if((Instance->ISR & I2C_ISR_NACKF) != 0U)
        {
            Instance->ICR = I2C_ICR_NACKCF;
            return I2C_STATUS_NACK;
        }

        if((Instance->ISR & I2C_ISR_ARLO) != 0U)
        {
            Instance->ICR = I2C_ICR_ARLOCF;
            return I2C_STATUS_ARBITRATION_LOST;
        }

        if(Timeout-- == 0U)
        {
            return I2C_STATUS_TIMEOUT;
        }
    }

    *Data = (uint8_t)Instance->RXDR;

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_STOPF) == 0U)
    {
        if(Timeout-- == 0U)
        {
            return I2C_STATUS_TIMEOUT;
        }
    }

    Instance->ICR = I2C_ICR_STOPCF;

    return I2C_STATUS_OK;
}

I2C_StatusTypeDef I2C_DeviceMemWrite16(I2C_DeviceHandleTypeDef *Device, uint16_t Register, uint8_t Data)
{
    uint8_t Buffer[3];

    if(Device == NULL)
    {
        return I2C_STATUS_ERROR;
    }

    Buffer[0] = (uint8_t)(Register >> 8U);
    Buffer[1] = (uint8_t)(Register & 0x00FFU);
    Buffer[2] = Data;

    return I2C_DeviceWrite(Device, Buffer, 3U);
}

I2C_StatusTypeDef I2C_DeviceMemRead16(I2C_DeviceHandleTypeDef *Device, uint16_t Register, uint8_t *Data)
{
    I2C_TypeDef *Instance;
    uint32_t Timeout;

    if((Device == NULL) || (Device->Bus == NULL) || (Device->Bus->Instance == NULL) || (Data == NULL))
    {
        return I2C_STATUS_ERROR;
    }

    Instance = Device->Bus->Instance;

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_BUSY) != 0U)
    {
        if(Timeout-- == 0U)
        {
            return I2C_STATUS_BUSY;
        }
    }

    Instance->ICR = I2C_ICR_ADDRCF | I2C_ICR_NACKCF | I2C_ICR_STOPCF | I2C_ICR_BERRCF |
                    I2C_ICR_ARLOCF | I2C_ICR_OVRCF | I2C_ICR_PECCF | I2C_ICR_TIMOUTCF | I2C_ICR_ALERTCF;

    Instance->CR2 =
        ((uint32_t)(Device->Address << 1U) & I2C_CR2_SADD) |
        (2U << I2C_CR2_NBYTES_Pos) |
        I2C_CR2_START;

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_TXIS) == 0U)
    {
        if((Instance->ISR & I2C_ISR_NACKF) != 0U)
        {
            Instance->ICR = I2C_ICR_NACKCF;
            return I2C_STATUS_NACK;
        }

        if((Instance->ISR & I2C_ISR_ARLO) != 0U)
        {
            Instance->ICR = I2C_ICR_ARLOCF;
            return I2C_STATUS_ARBITRATION_LOST;
        }

        if(Timeout-- == 0U)
        {
            return I2C_STATUS_TIMEOUT;
        }
    }

    Instance->TXDR = (uint8_t)(Register >> 8U);

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_TXIS) == 0U)
    {
        if((Instance->ISR & I2C_ISR_NACKF) != 0U)
        {
            Instance->ICR = I2C_ICR_NACKCF;
            return I2C_STATUS_NACK;
        }

        if((Instance->ISR & I2C_ISR_ARLO) != 0U)
        {
            Instance->ICR = I2C_ICR_ARLOCF;
            return I2C_STATUS_ARBITRATION_LOST;
        }

        if(Timeout-- == 0U)
        {
            return I2C_STATUS_TIMEOUT;
        }
    }

    Instance->TXDR = (uint8_t)(Register & 0x00FFU);

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_TC) == 0U)
    {
        if((Instance->ISR & I2C_ISR_NACKF) != 0U)
        {
            Instance->ICR = I2C_ICR_NACKCF;
            return I2C_STATUS_NACK;
        }

        if((Instance->ISR & I2C_ISR_ARLO) != 0U)
        {
            Instance->ICR = I2C_ICR_ARLOCF;
            return I2C_STATUS_ARBITRATION_LOST;
        }

        if(Timeout-- == 0U)
        {
            return I2C_STATUS_TIMEOUT;
        }
    }

    Instance->CR2 =
        ((uint32_t)(Device->Address << 1U) & I2C_CR2_SADD) |
        (1U << I2C_CR2_NBYTES_Pos) |
        I2C_CR2_RD_WRN |
        I2C_CR2_AUTOEND |
        I2C_CR2_START;

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_RXNE) == 0U)
    {
        if((Instance->ISR & I2C_ISR_NACKF) != 0U)
        {
            Instance->ICR = I2C_ICR_NACKCF;
            return I2C_STATUS_NACK;
        }

        if((Instance->ISR & I2C_ISR_ARLO) != 0U)
        {
            Instance->ICR = I2C_ICR_ARLOCF;
            return I2C_STATUS_ARBITRATION_LOST;
        }

        if(Timeout-- == 0U)
        {
            return I2C_STATUS_TIMEOUT;
        }
    }

    *Data = (uint8_t)Instance->RXDR;

    Timeout = Device->Bus->Timeout;

    while((Instance->ISR & I2C_ISR_STOPF) == 0U)
    {
        if(Timeout-- == 0U)
        {
            return I2C_STATUS_TIMEOUT;
        }
    }

    Instance->ICR = I2C_ICR_STOPCF;

    return I2C_STATUS_OK;
}
