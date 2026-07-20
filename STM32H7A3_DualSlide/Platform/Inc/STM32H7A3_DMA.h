/*
 * STM32H7A3_DMA.h
 *
 * Independent DMA1/DMA2 peripheral driver for STM32H7A3.
 */

#ifndef INC_STM32H7A3_DMA_H_
#define INC_STM32H7A3_DMA_H_

#include <stdint.h>
#include <stddef.h>
#include "STM32H7xx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    /*
     * No peripheral request.
     */
    DMA_REQUEST_MEM2MEM          = 0U,

    /*
     * ADC
     */
    DMA_REQUEST_ADC1             = 9U,
    DMA_REQUEST_ADC2             = 10U,
    DMA_REQUEST_ADC3             = 115U,

    /*
     * DAC
     */
    DMA_REQUEST_DAC1_CH1         = 67U,
    DMA_REQUEST_DAC1_CH2         = 68U,

    /*
     * SPI
     */
    DMA_REQUEST_SPI1_RX          = 37U,
    DMA_REQUEST_SPI1_TX          = 38U,
    DMA_REQUEST_SPI2_RX          = 39U,
    DMA_REQUEST_SPI2_TX          = 40U,
    DMA_REQUEST_SPI3_RX          = 61U,
    DMA_REQUEST_SPI3_TX          = 62U,
    DMA_REQUEST_SPI4_RX          = 83U,
    DMA_REQUEST_SPI4_TX          = 84U,
    DMA_REQUEST_SPI5_RX          = 85U,
    DMA_REQUEST_SPI5_TX          = 86U,

    /*
     * SPI6 is connected to BDMA/DMAMUX2, not DMA1/DMA2.
     */

    /*
     * I2C
     */
    DMA_REQUEST_I2C1_RX          = 33U,
    DMA_REQUEST_I2C1_TX          = 34U,
    DMA_REQUEST_I2C2_RX          = 35U,
    DMA_REQUEST_I2C2_TX          = 36U,
    DMA_REQUEST_I2C3_RX          = 73U,
    DMA_REQUEST_I2C3_TX          = 74U,

    /*
     * I2C4 is connected to BDMA/DMAMUX2, not DMA1/DMA2.
     */

    /*
     * UART / USART
     */
    DMA_REQUEST_USART1_RX        = 41U,
    DMA_REQUEST_USART1_TX        = 42U,
    DMA_REQUEST_USART2_RX        = 43U,
    DMA_REQUEST_USART2_TX        = 44U,
    DMA_REQUEST_USART3_RX        = 45U,
    DMA_REQUEST_USART3_TX        = 46U,

    DMA_REQUEST_UART4_RX         = 63U,
    DMA_REQUEST_UART4_TX         = 64U,
    DMA_REQUEST_UART5_RX         = 65U,
    DMA_REQUEST_UART5_TX         = 66U,

    DMA_REQUEST_USART6_RX        = 71U,
    DMA_REQUEST_USART6_TX        = 72U,

    DMA_REQUEST_UART7_RX         = 79U,
    DMA_REQUEST_UART7_TX         = 80U,
    DMA_REQUEST_UART8_RX         = 81U,
    DMA_REQUEST_UART8_TX         = 82U,

    /*
     * TIM1
     */
    DMA_REQUEST_TIM1_CH1         = 11U,
    DMA_REQUEST_TIM1_CH2         = 12U,
    DMA_REQUEST_TIM1_CH3         = 13U,
    DMA_REQUEST_TIM1_CH4         = 14U,
    DMA_REQUEST_TIM1_UP          = 15U,
    DMA_REQUEST_TIM1_TRIG        = 16U,
    DMA_REQUEST_TIM1_COM         = 17U,

    /*
     * TIM2
     */
    DMA_REQUEST_TIM2_CH1         = 18U,
    DMA_REQUEST_TIM2_CH2         = 19U,
    DMA_REQUEST_TIM2_CH3         = 20U,
    DMA_REQUEST_TIM2_CH4         = 21U,
    DMA_REQUEST_TIM2_UP          = 22U,

    /*
     * TIM3
     */
    DMA_REQUEST_TIM3_CH1         = 23U,
    DMA_REQUEST_TIM3_CH2         = 24U,
    DMA_REQUEST_TIM3_CH3         = 25U,
    DMA_REQUEST_TIM3_CH4         = 26U,
    DMA_REQUEST_TIM3_UP          = 27U,

    /*
     * TIM4
     *
     * STM32H7A3 DMAMUX1 does not provide a TIM4_CH4 request.
     */
    DMA_REQUEST_TIM4_CH1         = 29U,
    DMA_REQUEST_TIM4_CH2         = 30U,
    DMA_REQUEST_TIM4_CH3         = 31U,
    DMA_REQUEST_TIM4_UP          = 32U,

    /*
     * TIM5
     */
    DMA_REQUEST_TIM5_CH1         = 55U,
    DMA_REQUEST_TIM5_CH2         = 56U,
    DMA_REQUEST_TIM5_CH3         = 57U,
    DMA_REQUEST_TIM5_CH4         = 58U,
    DMA_REQUEST_TIM5_UP          = 59U,

    /*
     * Basic timers
     */
    DMA_REQUEST_TIM6_UP          = 69U,
    DMA_REQUEST_TIM7_UP          = 70U,

    /*
     * TIM8
     */
    DMA_REQUEST_TIM8_CH1         = 47U,
    DMA_REQUEST_TIM8_CH2         = 48U,
    DMA_REQUEST_TIM8_CH3         = 49U,
    DMA_REQUEST_TIM8_CH4         = 50U,
    DMA_REQUEST_TIM8_UP          = 51U,
    DMA_REQUEST_TIM8_TRIG        = 52U,
    DMA_REQUEST_TIM8_COM         = 53U,

    /*
     * SAI
     */
    DMA_REQUEST_SAI1_A           = 87U,
    DMA_REQUEST_SAI1_B           = 88U,
    DMA_REQUEST_SAI2_A           = 89U,
    DMA_REQUEST_SAI2_B           = 90U,

    /*
     * Camera, cryptography and hash
     */
    DMA_REQUEST_DCMI             = 75U,
    DMA_REQUEST_AES_IN           = 76U,  /* Called CRYP_IN by ST. */
    DMA_REQUEST_AES_OUT          = 77U,  /* Called CRYP_OUT by ST. */
    DMA_REQUEST_HASH_IN          = 78U,

    /*
     * DFSDM
     */
    DMA_REQUEST_DFSDM1_FLT0      = 101U,
    DMA_REQUEST_DFSDM1_FLT1      = 102U,
    DMA_REQUEST_DFSDM1_FLT2      = 103U,
    DMA_REQUEST_DFSDM1_FLT3      = 104U,

    /*
     * This is one greater than the largest DMAMUX1 request ID represented
     * by this enum. It is not the number of entries in the enum.
     */
    DMA_REQUEST_ID_LIMIT         = 116U

} DMA_RequestTypeDef;

typedef enum
{
    DMA_STATUS_OK = 0U,
    DMA_STATUS_ERROR,
    DMA_STATUS_BUSY,
    DMA_STATUS_TIMEOUT,
    DMA_STATUS_INVALID_PARAMETER

} DMA_StatusTypeDef;

typedef enum
{
    DMA_DIRECTION_PERIPHERAL_TO_MEMORY = 0U,
    DMA_DIRECTION_MEMORY_TO_PERIPHERAL,
    DMA_DIRECTION_MEMORY_TO_MEMORY

} DMA_DirectionTypeDef;

typedef enum
{
    DMA_DATA_WIDTH_BYTE = 0U,
    DMA_DATA_WIDTH_HALFWORD,
    DMA_DATA_WIDTH_WORD

} DMA_DataWidthTypeDef;

typedef enum
{
    DMA_MODE_NORMAL = 0U,
    DMA_MODE_CIRCULAR

} DMA_ModeTypeDef;

typedef enum
{
    DMA_PRIORITY_LOW = 0U,
    DMA_PRIORITY_MEDIUM,
    DMA_PRIORITY_HIGH,
    DMA_PRIORITY_VERY_HIGH

} DMA_PriorityTypeDef;

typedef enum
{
    DMA_BURST_SINGLE = 0U,
    DMA_BURST_INCR4,
    DMA_BURST_INCR8,
    DMA_BURST_INCR16

} DMA_BurstTypeDef;

typedef enum
{
    DMA_FIFO_DISABLED = 0U,
    DMA_FIFO_ENABLED

} DMA_FIFOModeTypeDef;

typedef enum
{
    DMA_FIFO_THRESHOLD_1_QUARTER = 0U,
    DMA_FIFO_THRESHOLD_1_HALF,
    DMA_FIFO_THRESHOLD_3_QUARTERS,
    DMA_FIFO_THRESHOLD_FULL

} DMA_FIFOThresholdTypeDef;

typedef enum
{
    DMA_EVENT_NONE              = 0U,
    DMA_EVENT_FIFO_ERROR        = (1UL << 0U),
    DMA_EVENT_DIRECT_MODE_ERROR = (1UL << 1U),
    DMA_EVENT_TRANSFER_ERROR    = (1UL << 2U),
    DMA_EVENT_HALF_TRANSFER     = (1UL << 3U),
    DMA_EVENT_TRANSFER_COMPLETE = (1UL << 4U)

} DMA_EventTypeDef;

typedef struct
{
    uint32_t Request;

    DMA_DirectionTypeDef Direction;

    DMA_DataWidthTypeDef PeripheralDataWidth;
    DMA_DataWidthTypeDef MemoryDataWidth;

    uint8_t PeripheralIncrement;
    uint8_t MemoryIncrement;

    DMA_ModeTypeDef Mode;
    DMA_PriorityTypeDef Priority;

    DMA_BurstTypeDef PeripheralBurst;
    DMA_BurstTypeDef MemoryBurst;

    DMA_FIFOModeTypeDef FIFOMode;
    DMA_FIFOThresholdTypeDef FIFOThreshold;

    uint8_t EnableTransferCompleteInterrupt;
    uint8_t EnableHalfTransferInterrupt;
    uint8_t EnableTransferErrorInterrupt;
    uint8_t EnableDirectModeErrorInterrupt;
    uint8_t EnableFIFOErrorInterrupt;

} DMA_ConfigTypeDef;

typedef struct DMA_HandleTypeDef DMA_HandleTypeDef;

typedef void (*DMA_CallbackTypeDef)(
    DMA_HandleTypeDef *Handle,
    DMA_EventTypeDef Event
);

struct DMA_HandleTypeDef
{
    DMA_Stream_TypeDef *Instance;

    DMA_ConfigTypeDef Config;

    /*
     * Unshifted CMSIS interrupt priority.
     *
     * Valid range:
     *     0 to ((1 << __NVIC_PRIO_BITS) - 1)
     *
     * Lower numerical values have higher interrupt priority.
     */
    uint32_t InterruptPriority;

    DMA_CallbackTypeDef Callback;

    volatile uint8_t Busy;
    volatile DMA_EventTypeDef LastEvent;
};

DMA_StatusTypeDef DMA_Init(
    DMA_HandleTypeDef *Handle
);

DMA_StatusTypeDef DMA_Start(
    DMA_HandleTypeDef *Handle,
    uintptr_t PeripheralAddress,
    uintptr_t MemoryAddress,
    uint32_t TransferCount
);

DMA_StatusTypeDef DMA_Stop(
    DMA_HandleTypeDef *Handle
);

uint8_t DMA_IsBusy(
    const DMA_HandleTypeDef *Handle
);

uint32_t DMA_GetRemainingTransferCount(
    const DMA_HandleTypeDef *Handle
);

DMA_StatusTypeDef DMA_SetMemoryAddress(
    DMA_HandleTypeDef *Handle,
    uintptr_t MemoryAddress
);

DMA_StatusTypeDef DMA_SetPeripheralAddress(
    DMA_HandleTypeDef *Handle,
    uintptr_t PeripheralAddress
);

DMA_StatusTypeDef DMA_SetTransferCount(
    DMA_HandleTypeDef *Handle,
    uint32_t TransferCount
);

void DMA_IRQHandler(
    DMA_TypeDef *Controller,
    uint32_t StreamNumber
);

DMA_StatusTypeDef DMA_ClearFlags(
    DMA_HandleTypeDef *Handle
);

uint32_t DMA_GetStreamNumber(
    const DMA_HandleTypeDef *Handle
);

DMA_TypeDef *DMA_GetController(
    const DMA_HandleTypeDef *Handle
);

#ifdef __cplusplus
}
#endif

#endif /* INC_STM32H7A3_DMA_H_ */
