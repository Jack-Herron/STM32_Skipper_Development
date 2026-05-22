/*
 * STM32H7A3_TIM.h
 *
 *  Created on: May 21, 2026
 *      Author: Jack Herron
 */

#ifndef INC_STM32H7A3_TIM_H_
#define INC_STM32H7A3_TIM_H_

#include "stm32h7xx.h"
#include <stdint.h>

typedef enum
{
    TIM_STATUS_OK = 0,
    TIM_STATUS_ERROR,
    TIM_STATUS_UNSUPPORTED

} TIM_Status_t;

typedef enum
{
    TIM_MODE_PERIODIC = 0,
    TIM_MODE_PWM1,
    TIM_MODE_PWM2

} TIM_Mode_t;

typedef struct
{
    TIM_TypeDef *instance;

    TIM_Mode_t   mode;

    uint32_t     frequency;
    uint8_t      enable_update_interrupt;

    uint8_t      channel;
    uint16_t     duty_permille;
    uint8_t      active_low;

    uint8_t      initialized;
    uint8_t      running;

} TIM_Handle_t;

TIM_Status_t TIM_EnableClock(TIM_TypeDef *tim);

TIM_Status_t TIM_Init(TIM_Handle_t *handle);

void TIM_Start(TIM_Handle_t *handle);
void TIM_Stop(TIM_Handle_t *handle);

uint8_t TIM_IsUpdateFlagSet(TIM_Handle_t *handle);
void TIM_ClearUpdateFlag(TIM_Handle_t *handle);

void TIM_SetPWMDutyPermille(TIM_Handle_t *handle,
                            uint16_t duty_permille);

void TIM_SetPWMDutyPercent(TIM_Handle_t *handle,
                           uint8_t duty_percent);

void TIM_SetPWMCompare(TIM_Handle_t *handle,
                       uint32_t compare);

#endif /* INC_STM32H7A3_TIM_H_ */
