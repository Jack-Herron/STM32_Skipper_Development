/*
 * STM32H7A3_I2C.h
 *
 *  Created on: May 22, 2026
 *      Author: Jack Herron
 */

#ifndef INC_STM32H7A3_I2C_H_
#define INC_STM32H7A3_I2C_H_

#include <stdint.h>
#include "STM32H7xx.h"

typedef enum
{
    I2C_STATUS_OK = 0U,
    I2C_STATUS_ERROR,
    I2C_STATUS_TIMEOUT,
    I2C_STATUS_BUSY,
    I2C_STATUS_NACK,
    I2C_STATUS_ARBITRATION_LOST,
    I2C_STATUS_INVALID_PARAMETER

} I2C_StatusTypeDef;

typedef enum
{
    I2C_ADDRESSING_MODE_7BIT = 0U,
    I2C_ADDRESSING_MODE_10BIT

} I2C_AddressingModeTypeDef;

typedef struct
{
    I2C_TypeDef *Instance;

    uint32_t BusFreq;

    uint32_t Timeout;

} I2C_BusHandleTypeDef;

typedef struct
{
    I2C_BusHandleTypeDef *Bus;
    uint16_t Address;

    I2C_AddressingModeTypeDef AddressingMode;

} I2C_DeviceHandleTypeDef;

I2C_StatusTypeDef I2C_Init(I2C_BusHandleTypeDef *Handle);
I2C_StatusTypeDef I2C_DeviceCheck(I2C_DeviceHandleTypeDef *Device);
I2C_StatusTypeDef I2C_DeviceRead(I2C_DeviceHandleTypeDef *Device, uint8_t *Data, uint16_t Length);
I2C_StatusTypeDef I2C_DeviceWrite(I2C_DeviceHandleTypeDef *Device, const uint8_t *Data, uint16_t Length);
I2C_StatusTypeDef I2C_DeviceMemWrite8(I2C_DeviceHandleTypeDef *Device, uint8_t Register, uint8_t Data);
I2C_StatusTypeDef I2C_DeviceMemRead8(I2C_DeviceHandleTypeDef *Device, uint8_t Register, uint8_t *Data);
I2C_StatusTypeDef I2C_DeviceMemWrite16(I2C_DeviceHandleTypeDef *Device, uint16_t Register, uint8_t Data);
I2C_StatusTypeDef I2C_DeviceMemRead16(I2C_DeviceHandleTypeDef *Device, uint16_t Register, uint8_t *Data);

#endif /* INC_STM32H7A3_I2C_H_ */
