/*
 * ST7701S.h
 *
 *  Created on: Jul 11, 2026
 *      Author: Jack Herron
 */

#ifndef INC_ST7701S_H_
#define INC_ST7701S_H_

#include <stdint.h>
#include "STM32H7xx.h"
#include "STM32H7A3_SPI.h"

typedef enum
{
    ST7701S_STATUS_OK = 0U,
    ST7701S_STATUS_ERROR,
    ST7701S_STATUS_TIMEOUT,
    ST7701S_STATUS_INVALID_PARAMETER

} ST7701S_StatusTypeDef;

typedef struct
{
    SPI_DeviceHandleTypeDef *SPI;

    GPIO_TypeDef *ResetPort;
    uint16_t ResetPin;
    uint8_t ResetActiveLow;
    void (*Delay_ms)(uint32_t Delay);
} ST7701S_HandleTypeDef;

ST7701S_StatusTypeDef ST7701S_WriteCommand(
    ST7701S_HandleTypeDef *Handle,
    uint8_t Command);

ST7701S_StatusTypeDef ST7701S_WriteData(
    ST7701S_HandleTypeDef *Handle,
    uint8_t Data);

ST7701S_StatusTypeDef ST7701S_WriteDataBuffer(
    ST7701S_HandleTypeDef *Handle,
    const uint8_t *Data,
    uint16_t Length);

ST7701S_StatusTypeDef ST7701S_WriteRegister(
    ST7701S_HandleTypeDef *Handle,
    uint8_t Command,
    const uint8_t *Data,
    uint16_t Length);

ST7701S_StatusTypeDef ST7701S_HardwareReset(
    ST7701S_HandleTypeDef *Handle);

ST7701S_StatusTypeDef ST7701S_ExitSleep(
    ST7701S_HandleTypeDef *Handle);

ST7701S_StatusTypeDef ST7701S_DisplayOn(
    ST7701S_HandleTypeDef *Handle);

ST7701S_StatusTypeDef ST7701S_DisplayOff(
    ST7701S_HandleTypeDef *Handle);

ST7701S_StatusTypeDef ST7701S_EnterSleep(
    ST7701S_HandleTypeDef *Handle);

#endif /* INC_ST7701S_H_ */
