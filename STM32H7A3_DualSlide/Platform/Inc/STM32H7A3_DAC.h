/*
 * STM32H7A3_DAC.h
 *
 * Independent DAC peripheral driver for STM32H7A3.
 *
 * This driver does not depend on the DMA or timer drivers.
 * Higher-level code may connect DMA to the DAC by using:
 *
 *     DAC_GetDataRegisterAddress()
 *     DAC_EnableDMARequest()
 *
 */

#ifndef INC_STM32H7A3_DAC_H_
#define INC_STM32H7A3_DAC_H_

#include <stdint.h>
#include "STM32H7xx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DAC_STATUS_OK = 0U,
    DAC_STATUS_ERROR,
    DAC_STATUS_INVALID_PARAMETER

} DAC_StatusTypeDef;

typedef enum
{
    DAC_CHANNEL_1 = 0U,
    DAC_CHANNEL_2

} DAC_ChannelTypeDef;

typedef enum
{
    DAC_ALIGNMENT_12BIT_RIGHT = 0U,
    DAC_ALIGNMENT_12BIT_LEFT,
    DAC_ALIGNMENT_8BIT_RIGHT

} DAC_AlignmentTypeDef;

typedef enum
{
    DAC_TRIGGER_NONE = 0U,
    DAC_TRIGGER_TIM6_TRGO,
    DAC_TRIGGER_TIM8_TRGO,
    DAC_TRIGGER_TIM7_TRGO,
    DAC_TRIGGER_TIM5_TRGO,
    DAC_TRIGGER_TIM2_TRGO,
    DAC_TRIGGER_TIM4_TRGO,
    DAC_TRIGGER_EXTI9,
    DAC_TRIGGER_SOFTWARE,
    DAC_TRIGGER_TIM1_TRGO,
    DAC_TRIGGER_TIM15_TRGO,
    DAC_TRIGGER_LPTIM1_OUT,
    DAC_TRIGGER_LPTIM2_OUT,
    DAC_TRIGGER_LPTIM3_OUT

} DAC_TriggerTypeDef;

typedef enum
{
    DAC_OUTPUT_BUFFER_ENABLED = 0U,
    DAC_OUTPUT_BUFFER_DISABLED

} DAC_OutputBufferTypeDef;

typedef enum
{
    DAC_SAMPLE_AND_HOLD_DISABLED = 0U,
    DAC_SAMPLE_AND_HOLD_ENABLED

} DAC_SampleAndHoldTypeDef;

typedef struct
{
    DAC_TypeDef *Instance;

    DAC_ChannelTypeDef Channel;
    DAC_AlignmentTypeDef Alignment;
    DAC_TriggerTypeDef Trigger;
    DAC_OutputBufferTypeDef OutputBuffer;
    DAC_SampleAndHoldTypeDef SampleAndHold;

} DAC_HandleTypeDef;

/*
 * Initializes the selected DAC channel.
 *
 * The GPIO pin must be configured separately:
 *
 *     DAC1 channel 1: PA4
 *     DAC1 channel 2: PA5
 *
 * Configure the pin as analog mode with no pull resistor.
 */
DAC_StatusTypeDef DAC_Init(DAC_HandleTypeDef *Handle);

/*
 * Enables or disables the selected DAC channel.
 */
DAC_StatusTypeDef DAC_Enable(DAC_HandleTypeDef *Handle);
DAC_StatusTypeDef DAC_Disable(DAC_HandleTypeDef *Handle);

/*
 * Writes one value into the selected DAC data holding register.
 *
 * Valid ranges:
 *
 *     DAC_ALIGNMENT_12BIT_RIGHT: 0 to 4095
 *     DAC_ALIGNMENT_12BIT_LEFT:  0 to 65520, lower four bits zero
 *     DAC_ALIGNMENT_8BIT_RIGHT:  0 to 255
 */
DAC_StatusTypeDef DAC_Write(
    DAC_HandleTypeDef *Handle,
    uint16_t Value
);

/*
 * Generates a software trigger for the selected channel.
 *
 * The handle must have been initialized with:
 *
 *     Trigger = DAC_TRIGGER_SOFTWARE
 */
DAC_StatusTypeDef DAC_SoftwareTrigger(
    DAC_HandleTypeDef *Handle
);

/*
 * Enables or disables DAC-generated DMA requests.
 *
 * This only changes the DAC peripheral's DMA request-enable bit.
 * DMA stream and DMAMUX configuration remain the responsibility of
 * the independent DMA driver or higher-level application code.
 */
DAC_StatusTypeDef DAC_EnableDMARequest(
    DAC_HandleTypeDef *Handle
);

DAC_StatusTypeDef DAC_DisableDMARequest(
    DAC_HandleTypeDef *Handle
);

/*
 * Enables or disables the DAC DMA underrun interrupt.
 */
DAC_StatusTypeDef DAC_EnableDMAUnderrunInterrupt(
    DAC_HandleTypeDef *Handle
);

DAC_StatusTypeDef DAC_DisableDMAUnderrunInterrupt(
    DAC_HandleTypeDef *Handle
);

/*
 * Returns the address of the selected DAC data holding register.
 *
 * This address can be supplied as the peripheral destination address
 * to an independent DMA driver.
 *
 * Returns 0U if the handle or alignment is invalid.
 */
uintptr_t DAC_GetDataRegisterAddress(
    const DAC_HandleTypeDef *Handle
);

/*
 * Returns the transfer width, in bytes, required by DMA for the selected
 * DAC alignment:
 *
 *     1 byte for 8-bit right-aligned mode
 *     2 bytes for 12-bit modes
 *
 * Returns 0U if the handle or alignment is invalid.
 */
uint32_t DAC_GetDataWidthBytes(
    const DAC_HandleTypeDef *Handle
);

/*
 * Returns nonzero if a DMA underrun flag is set for the selected channel.
 */
uint8_t DAC_IsDMAUnderrun(
    const DAC_HandleTypeDef *Handle
);

/*
 * Clears the selected channel's DMA underrun flag.
 */
DAC_StatusTypeDef DAC_ClearDMAUnderrun(
    DAC_HandleTypeDef *Handle
);

#ifdef __cplusplus
}
#endif

#endif /* INC_STM32H7A3_DAC_H_ */
