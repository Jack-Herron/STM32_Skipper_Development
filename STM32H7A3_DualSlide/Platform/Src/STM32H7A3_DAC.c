/*
 * STM32H7A3_DAC.c
 *
 * Independent DAC peripheral driver for STM32H7A3.
 */
#include <STDLIB.h>
#include "STM32H7A3_DAC.h"

static uint32_t DAC_GetChannelShift(
    const DAC_HandleTypeDef *Handle
);

static uint32_t DAC_GetTriggerSelection(
    DAC_TriggerTypeDef Trigger
);

static uint8_t DAC_IsHandleValid(
    const DAC_HandleTypeDef *Handle
);

static uint8_t DAC_IsAlignmentValid(
    DAC_AlignmentTypeDef Alignment
);

static uint8_t DAC_IsTriggerValid(
    DAC_TriggerTypeDef Trigger
);

static uint8_t DAC_IsValueValid(
    const DAC_HandleTypeDef *Handle,
    uint16_t Value
);

static volatile uint32_t *DAC_GetDataRegister(
    const DAC_HandleTypeDef *Handle
);

DAC_StatusTypeDef DAC_Init(DAC_HandleTypeDef *Handle)
{
    uint32_t channel_shift;
    uint32_t control_mask;
    uint32_t control_value;
    uint32_t mode_mask;
    uint32_t mode_value;

    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    /*
     * Enable the DAC12 peripheral clock.
     */
#if defined(RCC_APB1LENR_DAC12EN)

    RCC->APB1LENR |= RCC_APB1LENR_DAC12EN;
    (void)RCC->APB1LENR;

#elif defined(RCC_APB1LENR_DAC1EN)

    RCC->APB1LENR |= RCC_APB1LENR_DAC1EN;
    (void)RCC->APB1LENR;

#else

#error "DAC peripheral clock-enable definition not found."

#endif

    channel_shift = DAC_GetChannelShift(Handle);

    /*
     * Disable the selected DAC channel before modifying its configuration.
     */
    Handle->Instance->CR &= ~(DAC_CR_EN1 << channel_shift);

    /*
     * Configure trigger selection.
     *
     * DMA and DMA-underrun interrupt enable bits are cleared here.
     * They may be enabled separately after initialization.
     */
    control_mask =
        (DAC_CR_TEN1 |
         DAC_CR_TSEL1 |
         DAC_CR_DMAEN1 |
         DAC_CR_DMAUDRIE1) << channel_shift;

    control_value = 0U;

    if(Handle->Trigger != DAC_TRIGGER_NONE)
    {
        control_value |= DAC_CR_TEN1 << channel_shift;

        control_value |=
            DAC_GetTriggerSelection(Handle->Trigger)
            << (DAC_CR_TSEL1_Pos + channel_shift);
    }

    Handle->Instance->CR =
        (Handle->Instance->CR & ~control_mask) |
        (control_value & control_mask);

    /*
     * Configure the DAC operating mode.
     *
     * MODE = 000:
     *     Normal mode, output buffer enabled.
     *
     * MODE = 001:
     *     Sample-and-hold mode, output buffer enabled.
     *
     * MODE = 010:
     *     Normal mode, output buffer disabled.
     *
     * MODE = 011:
     *     Sample-and-hold mode, output buffer disabled.
     */
    mode_mask = DAC_MCR_MODE1 << channel_shift;
    mode_value = 0U;

    if(Handle->SampleAndHold == DAC_SAMPLE_AND_HOLD_ENABLED)
    {
        mode_value = 1UL << (DAC_MCR_MODE1_Pos + channel_shift);
    }

    if(Handle->OutputBuffer == DAC_OUTPUT_BUFFER_DISABLED)
    {
        mode_value |= 2UL << (DAC_MCR_MODE1_Pos + channel_shift);
    }

    Handle->Instance->MCR =
        (Handle->Instance->MCR & ~mode_mask) |
        (mode_value & mode_mask);

    return DAC_STATUS_OK;
}

DAC_StatusTypeDef DAC_Enable(DAC_HandleTypeDef *Handle)
{
    uint32_t channel_shift;

    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    channel_shift = DAC_GetChannelShift(Handle);

    Handle->Instance->CR |= DAC_CR_EN1 << channel_shift;

    return DAC_STATUS_OK;
}

DAC_StatusTypeDef DAC_Disable(DAC_HandleTypeDef *Handle)
{
    uint32_t channel_shift;

    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    channel_shift = DAC_GetChannelShift(Handle);

    Handle->Instance->CR &= ~(DAC_CR_EN1 << channel_shift);

    return DAC_STATUS_OK;
}

DAC_StatusTypeDef DAC_Write(
    DAC_HandleTypeDef *Handle,
    uint16_t Value)
{
    volatile uint32_t *data_register;

    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    if(DAC_IsValueValid(Handle, Value) == 0U)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    data_register = DAC_GetDataRegister(Handle);

    if(data_register == NULL)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    *data_register = Value;

    return DAC_STATUS_OK;
}

DAC_StatusTypeDef DAC_SoftwareTrigger(
    DAC_HandleTypeDef *Handle)
{
    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    if(Handle->Trigger != DAC_TRIGGER_SOFTWARE)
    {
        return DAC_STATUS_ERROR;
    }

    if(Handle->Channel == DAC_CHANNEL_1)
    {
        Handle->Instance->SWTRIGR = DAC_SWTRIGR_SWTRIG1;
    }
    else
    {
        Handle->Instance->SWTRIGR = DAC_SWTRIGR_SWTRIG2;
    }

    return DAC_STATUS_OK;
}

DAC_StatusTypeDef DAC_EnableDMARequest(
    DAC_HandleTypeDef *Handle)
{
    uint32_t channel_shift;

    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    channel_shift = DAC_GetChannelShift(Handle);

    Handle->Instance->CR |= DAC_CR_DMAEN1 << channel_shift;

    return DAC_STATUS_OK;
}

DAC_StatusTypeDef DAC_DisableDMARequest(
    DAC_HandleTypeDef *Handle)
{
    uint32_t channel_shift;

    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    channel_shift = DAC_GetChannelShift(Handle);

    Handle->Instance->CR &= ~(DAC_CR_DMAEN1 << channel_shift);

    return DAC_STATUS_OK;
}

DAC_StatusTypeDef DAC_EnableDMAUnderrunInterrupt(
    DAC_HandleTypeDef *Handle)
{
    uint32_t channel_shift;

    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    channel_shift = DAC_GetChannelShift(Handle);

    Handle->Instance->CR |= DAC_CR_DMAUDRIE1 << channel_shift;

    return DAC_STATUS_OK;
}

DAC_StatusTypeDef DAC_DisableDMAUnderrunInterrupt(
    DAC_HandleTypeDef *Handle)
{
    uint32_t channel_shift;

    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    channel_shift = DAC_GetChannelShift(Handle);

    Handle->Instance->CR &= ~(DAC_CR_DMAUDRIE1 << channel_shift);

    return DAC_STATUS_OK;
}

uintptr_t DAC_GetDataRegisterAddress(
    const DAC_HandleTypeDef *Handle)
{
    volatile uint32_t *data_register;

    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return (uintptr_t)0U;
    }

    data_register = DAC_GetDataRegister(Handle);

    return (uintptr_t)data_register;
}

uint32_t DAC_GetDataWidthBytes(
    const DAC_HandleTypeDef *Handle)
{
    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return 0U;
    }

    if(Handle->Alignment == DAC_ALIGNMENT_8BIT_RIGHT)
    {
        return 1U;
    }

    return 2U;
}

uint8_t DAC_IsDMAUnderrun(
    const DAC_HandleTypeDef *Handle)
{
    uint32_t underrun_flag;

    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return 0U;
    }

    if(Handle->Channel == DAC_CHANNEL_1)
    {
        underrun_flag = DAC_SR_DMAUDR1;
    }
    else
    {
        underrun_flag = DAC_SR_DMAUDR2;
    }

    return
        ((Handle->Instance->SR & underrun_flag) != 0U)
        ? 1U
        : 0U;
}

DAC_StatusTypeDef DAC_ClearDMAUnderrun(
    DAC_HandleTypeDef *Handle)
{
    uint32_t underrun_flag;

    if(DAC_IsHandleValid(Handle) == 0U)
    {
        return DAC_STATUS_INVALID_PARAMETER;
    }

    if(Handle->Channel == DAC_CHANNEL_1)
    {
        underrun_flag = DAC_SR_DMAUDR1;
    }
    else
    {
        underrun_flag = DAC_SR_DMAUDR2;
    }

    /*
     * DAC DMA underrun flags are cleared by writing one to the flag.
     */
    Handle->Instance->SR = underrun_flag;

    return DAC_STATUS_OK;
}

static uint32_t DAC_GetChannelShift(
    const DAC_HandleTypeDef *Handle)
{
    return
        (Handle->Channel == DAC_CHANNEL_1)
        ? 0U
        : 16U;
}

static uint32_t DAC_GetTriggerSelection(
    DAC_TriggerTypeDef Trigger)
{
    switch(Trigger)
    {
        case DAC_TRIGGER_SOFTWARE:
            return 0U;

        case DAC_TRIGGER_TIM1_TRGO:
            return 1U;

        case DAC_TRIGGER_TIM2_TRGO:
            return 2U;

        case DAC_TRIGGER_TIM4_TRGO:
            return 3U;

        case DAC_TRIGGER_TIM5_TRGO:
            return 4U;

        case DAC_TRIGGER_TIM6_TRGO:
            return 5U;

        case DAC_TRIGGER_TIM7_TRGO:
            return 6U;

        case DAC_TRIGGER_TIM8_TRGO:
            return 7U;

        case DAC_TRIGGER_TIM15_TRGO:
            return 8U;

        case DAC_TRIGGER_LPTIM1_OUT:
            return 11U;

        case DAC_TRIGGER_LPTIM2_OUT:
            return 12U;

        case DAC_TRIGGER_EXTI9:
            return 13U;

        case DAC_TRIGGER_LPTIM3_OUT:
            return 14U;

        case DAC_TRIGGER_NONE:
        default:
            return 0U;
    }
}

static uint8_t DAC_IsHandleValid(
    const DAC_HandleTypeDef *Handle)
{
    if((Handle == NULL) || (Handle->Instance == NULL))
    {
        return 0U;
    }

    if((Handle->Channel != DAC_CHANNEL_1) &&
       (Handle->Channel != DAC_CHANNEL_2))
    {
        return 0U;
    }

    if(DAC_IsAlignmentValid(Handle->Alignment) == 0U)
    {
        return 0U;
    }

    if(DAC_IsTriggerValid(Handle->Trigger) == 0U)
    {
        return 0U;
    }

    if((Handle->OutputBuffer != DAC_OUTPUT_BUFFER_ENABLED) &&
       (Handle->OutputBuffer != DAC_OUTPUT_BUFFER_DISABLED))
    {
        return 0U;
    }

    if((Handle->SampleAndHold != DAC_SAMPLE_AND_HOLD_DISABLED) &&
       (Handle->SampleAndHold != DAC_SAMPLE_AND_HOLD_ENABLED))
    {
        return 0U;
    }

    return 1U;
}

static uint8_t DAC_IsAlignmentValid(
    DAC_AlignmentTypeDef Alignment)
{
    switch(Alignment)
    {
        case DAC_ALIGNMENT_12BIT_RIGHT:
        case DAC_ALIGNMENT_12BIT_LEFT:
        case DAC_ALIGNMENT_8BIT_RIGHT:
            return 1U;

        default:
            return 0U;
    }
}

static uint8_t DAC_IsTriggerValid(
    DAC_TriggerTypeDef Trigger)
{
    switch(Trigger)
    {
        case DAC_TRIGGER_NONE:
        case DAC_TRIGGER_TIM6_TRGO:
        case DAC_TRIGGER_TIM8_TRGO:
        case DAC_TRIGGER_TIM7_TRGO:
        case DAC_TRIGGER_TIM5_TRGO:
        case DAC_TRIGGER_TIM2_TRGO:
        case DAC_TRIGGER_TIM4_TRGO:
        case DAC_TRIGGER_EXTI9:
        case DAC_TRIGGER_SOFTWARE:
        case DAC_TRIGGER_TIM1_TRGO:
        case DAC_TRIGGER_TIM15_TRGO:
        case DAC_TRIGGER_LPTIM1_OUT:
        case DAC_TRIGGER_LPTIM2_OUT:
        case DAC_TRIGGER_LPTIM3_OUT:
            return 1U;

        default:
            return 0U;
    }
}

static uint8_t DAC_IsValueValid(
    const DAC_HandleTypeDef *Handle,
    uint16_t Value)
{
    switch(Handle->Alignment)
    {
        case DAC_ALIGNMENT_12BIT_RIGHT:
            return (Value <= 4095U) ? 1U : 0U;

        case DAC_ALIGNMENT_12BIT_LEFT:
            return
                ((Value <= 65520U) && ((Value & 0x000FU) == 0U))
                ? 1U
                : 0U;

        case DAC_ALIGNMENT_8BIT_RIGHT:
            return (Value <= 255U) ? 1U : 0U;

        default:
            return 0U;
    }
}

static volatile uint32_t *DAC_GetDataRegister(
    const DAC_HandleTypeDef *Handle)
{
    if(Handle->Channel == DAC_CHANNEL_1)
    {
        switch(Handle->Alignment)
        {
            case DAC_ALIGNMENT_12BIT_RIGHT:
                return &Handle->Instance->DHR12R1;

            case DAC_ALIGNMENT_12BIT_LEFT:
                return &Handle->Instance->DHR12L1;

            case DAC_ALIGNMENT_8BIT_RIGHT:
                return &Handle->Instance->DHR8R1;

            default:
                return NULL;
        }
    }

    switch(Handle->Alignment)
    {
        case DAC_ALIGNMENT_12BIT_RIGHT:
            return &Handle->Instance->DHR12R2;

        case DAC_ALIGNMENT_12BIT_LEFT:
            return &Handle->Instance->DHR12L2;

        case DAC_ALIGNMENT_8BIT_RIGHT:
            return &Handle->Instance->DHR8R2;

        default:
            return NULL;
    }
}
