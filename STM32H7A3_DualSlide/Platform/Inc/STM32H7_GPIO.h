/*
 * STM32H7_GPIO.h
 *
 *  Created on: May 16, 2026
 *      Author: Jack Herron
 */

#ifndef INC_STM32H7_GPIO_H_
#define INC_STM32H7_GPIO_H_

#include <stdint.h>

typedef enum
{
    GPIO_MODE_INPUT  = 0x0,
    GPIO_MODE_OUTPUT = 0x1,
    GPIO_MODE_AF     = 0x2,
    GPIO_MODE_ANALOG = 0x3

} GPIO_Mode_t;

typedef enum
{
    GPIO_OTYPE_PP = 0x0,
    GPIO_OTYPE_OD = 0x1

} GPIO_OutputType_t;

typedef enum
{
    GPIO_SPEED_LOW       = 0x0,
    GPIO_SPEED_MEDIUM    = 0x1,
    GPIO_SPEED_HIGH      = 0x2,
    GPIO_SPEED_VERY_HIGH = 0x3

} GPIO_Speed_t;

typedef enum
{
    GPIO_PULL_NONE = 0x0,
    GPIO_PULL_UP   = 0x1,
    GPIO_PULL_DOWN = 0x2

} GPIO_Pull_t;

typedef struct
{
    GPIO_Mode_t         mode;
    GPIO_OutputType_t   output_type;
    GPIO_Speed_t        speed;
    GPIO_Pull_t         pull;

    uint8_t             alternate_function;
    uint8_t             initial_output_state;

} GPIO_Config_t;

void GPIO_Init(GPIO_TypeDef *port, uint8_t pin, const GPIO_Config_t *config);
void GPIO_Write(GPIO_TypeDef *port, uint8_t pin, uint8_t state);
uint8_t GPIO_Read(GPIO_TypeDef *port, uint8_t pin);
void GPIO_Toggle(GPIO_TypeDef *port, uint8_t pin);


#endif /* INC_STM32H7_GPIO_H_ */
