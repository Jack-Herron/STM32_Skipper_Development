/*
 * STM32H7A3_SPI.h
 *
 *  Created on: Jul 11, 2026
 *      Author: Jack Herron
 */

#ifndef INC_STM32H7A3_SPI_H_
#define INC_STM32H7A3_SPI_H_

#include <stdint.h>
#include "STM32H7xx.h"

typedef enum
{
    SPI_STATUS_OK = 0U,
    SPI_STATUS_ERROR,
    SPI_STATUS_TIMEOUT,
    SPI_STATUS_BUSY,
    SPI_STATUS_OVERRUN,
    SPI_STATUS_MODE_FAULT,
    SPI_STATUS_INVALID_PARAMETER

} SPI_StatusTypeDef;

typedef enum
{
    SPI_MODE_0 = 0U,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3

} SPI_ModeTypeDef;

typedef enum
{
    SPI_BIT_ORDER_MSB_FIRST = 0U,
    SPI_BIT_ORDER_LSB_FIRST

} SPI_BitOrderTypeDef;

typedef enum
{
    SPI_CHIP_SELECT_ACTIVE_LOW = 0U,
    SPI_CHIP_SELECT_ACTIVE_HIGH

} SPI_ChipSelectPolarityTypeDef;

typedef enum
{
    SPI_FRAME_SIZE_8_BIT = 8U,
    SPI_FRAME_SIZE_9_BIT = 9U

} SPI_FrameSizeTypeDef;

typedef enum
{
    SPI_DIRECTION_FULL_DUPLEX = 0U,
    SPI_DIRECTION_TX_ONLY,
    SPI_DIRECTION_RX_ONLY,
    SPI_DIRECTION_HALF_DUPLEX

} SPI_DirectionTypeDef;

typedef struct
{
    SPI_TypeDef *Instance;
    uint32_t Timeout;

} SPI_BusHandleTypeDef;

typedef struct
{
    SPI_BusHandleTypeDef *Bus;

    GPIO_TypeDef *ChipSelectPort;
    uint16_t ChipSelectPin;
    SPI_ChipSelectPolarityTypeDef ChipSelectPolarity;

    uint32_t MaxClockFrequency;
    SPI_ModeTypeDef Mode;
    SPI_BitOrderTypeDef BitOrder;
    SPI_FrameSizeTypeDef FrameSize;
    SPI_DirectionTypeDef Direction;

} SPI_DeviceHandleTypeDef;

SPI_StatusTypeDef SPI_Init(
    SPI_BusHandleTypeDef *Handle);

SPI_StatusTypeDef SPI_DeviceRead(
    SPI_DeviceHandleTypeDef *Device,
    uint8_t *Data,
    uint16_t Length);

SPI_StatusTypeDef SPI_DeviceWrite(
    SPI_DeviceHandleTypeDef *Device,
    const uint8_t *Data,
    uint16_t Length);

SPI_StatusTypeDef SPI_DeviceTransfer(
    SPI_DeviceHandleTypeDef *Device,
    const uint8_t *TxData,
    uint8_t *RxData,
    uint16_t Length);

SPI_StatusTypeDef SPI_DeviceReadFrames(
    SPI_DeviceHandleTypeDef *Device,
    uint16_t *Data,
    uint16_t Length);

SPI_StatusTypeDef SPI_DeviceWriteFrames(
    SPI_DeviceHandleTypeDef *Device,
    const uint16_t *Data,
    uint16_t Length);

SPI_StatusTypeDef SPI_DeviceTransferFrames(
    SPI_DeviceHandleTypeDef *Device,
    const uint16_t *TxData,
    uint16_t *RxData,
    uint16_t Length);

#endif /* INC_STM32H7A3_SPI_H_ */
