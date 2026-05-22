/*
 * STM32H7A3_TIM.c
 *
 *  Created on: May 21, 2026
 *      Author: Jack Herron
 */

#include "STM32H7A3_TIM.h"
#include "STM32H7A3_RCC.h"
#include "stm32h7xx.h"
#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* Private helpers                                                            */
/* -------------------------------------------------------------------------- */

static uint8_t TIM_IsAdvanced(TIM_TypeDef *tim)
{
    return (tim == TIM1) || (tim == TIM8);
}

static uint8_t TIM_Is32Bit(TIM_TypeDef *tim)
{
    return (tim == TIM2) || (tim == TIM5);
}

static uint8_t TIM_IsValidPwmChannel(uint8_t channel)
{
    return (channel >= 1u) && (channel <= 4u);
}

static volatile uint32_t *TIM_GetCCR(TIM_TypeDef *tim, uint8_t channel)
{
    switch (channel)
    {
        case 1u: return &tim->CCR1;
        case 2u: return &tim->CCR2;
        case 3u: return &tim->CCR3;
        case 4u: return &tim->CCR4;
        default: return 0;
    }
}

static volatile uint32_t *TIM_GetCCMR(TIM_TypeDef *tim, uint8_t channel)
{
    if ((channel == 1u) || (channel == 2u))
    {
        return &tim->CCMR1;
    }

    return &tim->CCMR2;
}

static uint8_t TIM_GetCCMRShift(uint8_t channel)
{
    if ((channel == 1u) || (channel == 3u))
    {
        return 0u;
    }

    return 8u;
}

static uint8_t TIM_GetCCERShift(uint8_t channel)
{
    return (uint8_t)((channel - 1u) * 4u);
}

static TIM_Status_t TIM_ComputePrescalerPeriod(TIM_TypeDef *tim,
                                                uint32_t timer_clock_hz,
                                                uint32_t target_frequency_hz,
                                                uint16_t *prescaler_out,
                                                uint32_t *period_out)
{
    uint32_t prescaler;
    uint32_t ticks;
    uint32_t max_period;

    if ((tim == 0) ||
        (timer_clock_hz == 0u) ||
        (target_frequency_hz == 0u) ||
        (prescaler_out == 0) ||
        (period_out == 0))
    {
        return TIM_STATUS_ERROR;
    }

    max_period = TIM_Is32Bit(tim) ? 0xFFFFFFFFu : 0xFFFFu;

    for (prescaler = 0u; prescaler <= 0xFFFFu; prescaler++)
    {
        ticks = timer_clock_hz / ((prescaler + 1u) * target_frequency_hz);

        if (ticks == 0u)
        {
            continue;
        }

        if ((ticks - 1u) <= max_period)
        {
            *prescaler_out = (uint16_t)prescaler;
            *period_out = ticks - 1u;
            return TIM_STATUS_OK;
        }
    }

    return TIM_STATUS_ERROR;
}

/* -------------------------------------------------------------------------- */
/* Clock control                                                              */
/* -------------------------------------------------------------------------- */

TIM_Status_t TIM_EnableClock(TIM_TypeDef *tim)
{
    if (tim == 0)
    {
        return TIM_STATUS_ERROR;
    }

    if (tim == TIM1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
        (void)RCC->APB2ENR;
        return TIM_STATUS_OK;
    }

    if (tim == TIM8)
    {
        RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
        (void)RCC->APB2ENR;
        return TIM_STATUS_OK;
    }

    if (tim == TIM2)
    {
        RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
        (void)RCC->APB1LENR;
        return TIM_STATUS_OK;
    }

    if (tim == TIM3)
    {
        RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;
        (void)RCC->APB1LENR;
        return TIM_STATUS_OK;
    }

    if (tim == TIM4)
    {
        RCC->APB1LENR |= RCC_APB1LENR_TIM4EN;
        (void)RCC->APB1LENR;
        return TIM_STATUS_OK;
    }

    if (tim == TIM5)
    {
        RCC->APB1LENR |= RCC_APB1LENR_TIM5EN;
        (void)RCC->APB1LENR;
        return TIM_STATUS_OK;
    }

    if (tim == TIM6)
    {
        RCC->APB1LENR |= RCC_APB1LENR_TIM6EN;
        (void)RCC->APB1LENR;
        return TIM_STATUS_OK;
    }

    if (tim == TIM7)
    {
        RCC->APB1LENR |= RCC_APB1LENR_TIM7EN;
        (void)RCC->APB1LENR;
        return TIM_STATUS_OK;
    }

    if (tim == TIM12)
    {
        RCC->APB1LENR |= RCC_APB1LENR_TIM12EN;
        (void)RCC->APB1LENR;
        return TIM_STATUS_OK;
    }

    if (tim == TIM13)
    {
        RCC->APB1LENR |= RCC_APB1LENR_TIM13EN;
        (void)RCC->APB1LENR;
        return TIM_STATUS_OK;
    }

    if (tim == TIM14)
    {
        RCC->APB1LENR |= RCC_APB1LENR_TIM14EN;
        (void)RCC->APB1LENR;
        return TIM_STATUS_OK;
    }

#if defined(RCC_APB2ENR_TIM15EN)
    if (tim == TIM15)
    {
        RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
        (void)RCC->APB2ENR;
        return TIM_STATUS_OK;
    }
#endif

#if defined(RCC_APB2ENR_TIM16EN)
    if (tim == TIM16)
    {
        RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
        (void)RCC->APB2ENR;
        return TIM_STATUS_OK;
    }
#endif

#if defined(RCC_APB2ENR_TIM17EN)
    if (tim == TIM17)
    {
        RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
        (void)RCC->APB2ENR;
        return TIM_STATUS_OK;
    }
#endif

    return TIM_STATUS_UNSUPPORTED;
}

/* -------------------------------------------------------------------------- */
/* Init                                                                       */
/* -------------------------------------------------------------------------- */

static TIM_Status_t TIM_InitPeriodic(TIM_Handle_t *handle)
{
    TIM_TypeDef *tim;
    uint32_t timer_clock_hz;
    uint16_t prescaler;
    uint32_t period;
    TIM_Status_t status;

    tim = handle->instance;
    timer_clock_hz = RCC_GetKernelFreq(tim);

    status = TIM_ComputePrescalerPeriod(tim,
                                         timer_clock_hz,
                                         handle->frequency,
                                         &prescaler,
                                         &period);
    if (status != TIM_STATUS_OK)
    {
        return status;
    }

    tim->CR1 &= ~TIM_CR1_CEN;

    tim->PSC = prescaler;
    tim->ARR = period;
    tim->CNT = 0u;

    tim->CR1 = TIM_CR1_ARPE;
    tim->DIER = 0u;
    tim->SR = 0u;

    if (handle->enable_update_interrupt != 0u)
    {
        tim->DIER |= TIM_DIER_UIE;
    }

    tim->EGR = TIM_EGR_UG;

    return TIM_STATUS_OK;
}

static TIM_Status_t TIM_InitPWM(TIM_Handle_t *handle)
{
    TIM_TypeDef *tim;
    volatile uint32_t *ccmr;
    volatile uint32_t *ccr;
    uint32_t timer_clock_hz;
    uint16_t prescaler;
    uint32_t period;
    uint32_t compare;
    uint8_t ccmr_shift;
    uint8_t ccer_shift;
    uint32_t ccmr_mask;
    uint32_t ccmr_value;
    TIM_Status_t status;

    if (!TIM_IsValidPwmChannel(handle->channel))
    {
        return TIM_STATUS_ERROR;
    }

    if (handle->duty_permille > 1000u)
    {
        handle->duty_permille = 1000u;
    }

    tim = handle->instance;
    timer_clock_hz = RCC_GetKernelFreq(tim);

    status = TIM_ComputePrescalerPeriod(tim,
                                         timer_clock_hz,
                                         handle->frequency,
                                         &prescaler,
                                         &period);
    if (status != TIM_STATUS_OK)
    {
        return status;
    }

    ccr = TIM_GetCCR(tim, handle->channel);
    ccmr = TIM_GetCCMR(tim, handle->channel);
    ccmr_shift = TIM_GetCCMRShift(handle->channel);
    ccer_shift = TIM_GetCCERShift(handle->channel);

    if ((ccr == 0) || (ccmr == 0))
    {
        return TIM_STATUS_ERROR;
    }

    compare = ((period + 1u) * handle->duty_permille) / 1000u;

    tim->CR1 &= ~TIM_CR1_CEN;

    tim->PSC = prescaler;
    tim->ARR = period;
    tim->CNT = 0u;

    *ccr = compare;

    ccmr_mask = (TIM_CCMR1_CC1S |
                 TIM_CCMR1_OC1FE |
                 TIM_CCMR1_OC1PE |
                 TIM_CCMR1_OC1M) << ccmr_shift;

    ccmr_value = 0u;
    ccmr_value |= TIM_CCMR1_OC1PE << ccmr_shift;

    if (handle->mode == TIM_MODE_PWM2)
    {
        ccmr_value |= (7u << TIM_CCMR1_OC1M_Pos) << ccmr_shift;
    }
    else
    {
        ccmr_value |= (6u << TIM_CCMR1_OC1M_Pos) << ccmr_shift;
    }

    *ccmr &= ~ccmr_mask;
    *ccmr |= ccmr_value;

    tim->CCER &= ~((TIM_CCER_CC1E | TIM_CCER_CC1P) << ccer_shift);

    if (handle->active_low != 0u)
    {
        tim->CCER |= TIM_CCER_CC1P << ccer_shift;
    }

    tim->CCER |= TIM_CCER_CC1E << ccer_shift;

    tim->CR1 = TIM_CR1_ARPE;
    tim->EGR = TIM_EGR_UG;

    if (TIM_IsAdvanced(tim))
    {
        tim->BDTR |= TIM_BDTR_MOE;
    }

    return TIM_STATUS_OK;
}

TIM_Status_t TIM_Init(TIM_Handle_t *handle)
{
    TIM_Status_t status;

    if ((handle == 0) || (handle->instance == 0) || (handle->frequency == 0u))
    {
        return TIM_STATUS_ERROR;
    }

    handle->initialized = 0u;
    handle->running = 0u;

    status = TIM_EnableClock(handle->instance);
    if (status != TIM_STATUS_OK)
    {
        return status;
    }

    switch (handle->mode)
    {
        case TIM_MODE_PERIODIC:
            status = TIM_InitPeriodic(handle);
            break;

        case TIM_MODE_PWM1:
        case TIM_MODE_PWM2:
            status = TIM_InitPWM(handle);
            break;

        default:
            status = TIM_STATUS_UNSUPPORTED;
            break;
    }

    if (status == TIM_STATUS_OK)
    {
        handle->initialized = 1u;
    }

    return status;
}

/* -------------------------------------------------------------------------- */
/* Control                                                                    */
/* -------------------------------------------------------------------------- */

void TIM_Start(TIM_Handle_t *handle)
{
    if ((handle == 0) ||
        (handle->instance == 0) ||
        (handle->initialized == 0u))
    {
        return;
    }

    handle->instance->CR1 |= TIM_CR1_CEN;
    handle->running = 1u;
}

void TIM_Stop(TIM_Handle_t *handle)
{
    if ((handle == 0) || (handle->instance == 0))
    {
        return;
    }

    handle->instance->CR1 &= ~TIM_CR1_CEN;
    handle->running = 0u;
}

/* -------------------------------------------------------------------------- */
/* Periodic helpers                                                           */
/* -------------------------------------------------------------------------- */

uint8_t TIM_IsUpdateFlagSet(TIM_Handle_t *handle)
{
    if ((handle == 0) || (handle->instance == 0))
    {
        return 0u;
    }

    return (handle->instance->SR & TIM_SR_UIF) ? 1u : 0u;
}

void TIM_ClearUpdateFlag(TIM_Handle_t *handle)
{
    if ((handle == 0) || (handle->instance == 0))
    {
        return;
    }

    handle->instance->SR &= ~TIM_SR_UIF;
}

/* -------------------------------------------------------------------------- */
/* PWM helpers                                                                */
/* -------------------------------------------------------------------------- */

void TIM_SetPWMDutyPermille(TIM_Handle_t *handle, uint16_t duty_permille)
{
    volatile uint32_t *ccr;
    uint32_t compare;

    if ((handle == 0) ||
        (handle->instance == 0) ||
        (handle->initialized == 0u) ||
        !TIM_IsValidPwmChannel(handle->channel))
    {
        return;
    }

    if (duty_permille > 1000u)
    {
        duty_permille = 1000u;
    }

    ccr = TIM_GetCCR(handle->instance, handle->channel);

    if (ccr == 0)
    {
        return;
    }

    compare = ((handle->instance->ARR + 1u) * duty_permille) / 1000u;

    *ccr = compare;
    handle->duty_permille = duty_permille;
}

void TIM_SetPWMDutyPercent(TIM_Handle_t *handle, uint8_t duty_percent)
{
    if (duty_percent > 100u)
    {
        duty_percent = 100u;
    }

    TIM_SetPWMDutyPermille(handle, (uint16_t)duty_percent * 10u);
}

void TIM_SetPWMCompare(TIM_Handle_t *handle, uint32_t compare)
{
    volatile uint32_t *ccr;

    if ((handle == 0) ||
        (handle->instance == 0) ||
        (handle->initialized == 0u) ||
        !TIM_IsValidPwmChannel(handle->channel))
    {
        return;
    }

    ccr = TIM_GetCCR(handle->instance, handle->channel);

    if (ccr == 0)
    {
        return;
    }

    *ccr = compare;
}
