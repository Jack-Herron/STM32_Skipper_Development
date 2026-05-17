
#include "stm32h7xx.h"

/*
 * Sets the alternate function of a GPIO pin.
 *
 * GPIOx: GPIO port
 * pin:   Pin number (0-15)
 * af:    Alternate function number (0-15)
 */

void STM32H7_GPIO_Set_AF(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t af)
{
    uint32_t index = pin >> 3;
    uint32_t shift = (pin & 0x07) * 4;

    GPIOx->AFR[index] &= ~(0xFUL << shift);
    GPIOx->AFR[index] |=  ((uint32_t)af << shift);
}
