
#include "stm32h7xx.h"
#include "STM32H7_GPIO.h"

static void GPIO_Enable_Clock(GPIO_TypeDef *port)
{
    if(port == GPIOA)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    }
    else if(port == GPIOB)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    }
    else if(port == GPIOC)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
    }
    else if(port == GPIOD)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;
    }
    else if(port == GPIOE)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
    }
    else if(port == GPIOF)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;
    }
    else if(port == GPIOG)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;
    }
    else if(port == GPIOH)
    {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;
    }

    /*
     * Read-back delay.
     */

    (void)RCC->AHB4ENR;
}

/*
 * Initializes a GPIO pin using a GPIO_Config_t structure.
 */

void GPIO_Init(GPIO_TypeDef *port, uint8_t pin, const GPIO_Config_t *config)
{

	GPIO_Enable_Clock(port);

    uint32_t shift;
    uint32_t index;

    shift = pin * 2U;

    /*
     * Set initial output state before enabling output mode.
     */

    if(config->mode == GPIO_MODE_OUTPUT || config->mode == GPIO_MODE_AF)
    {
        if(config->initial_output_state)
        {
            port->BSRR = (1UL << pin);
        }
        else
        {
            port->BSRR = (1UL << (pin + 16U));
        }
    }

    /*
     * Configure mode.
     */

    port->MODER &= ~(0x3UL << shift);
    port->MODER |=  ((uint32_t)config->mode << shift);

    /*
     * Configure output type.
     */

    port->OTYPER &= ~(1UL << pin);
    port->OTYPER |=  ((uint32_t)config->output_type << pin);

    /*
     * Configure speed.
     */

    port->OSPEEDR &= ~(0x3UL << shift);
    port->OSPEEDR |=  ((uint32_t)config->speed << shift);

    /*
     * Configure pull-up / pull-down.
     */

    port->PUPDR &= ~(0x3UL << shift);
    port->PUPDR |=  ((uint32_t)config->pull << shift);

    /*
     * Configure alternate function.
     */

    if(config->mode == GPIO_MODE_AF)
    {
        index = pin >> 3U;
        shift = (pin & 0x7U) * 4U;

        port->AFR[index] &= ~(0xFUL << shift);
        port->AFR[index] |=
        		((uint32_t)config->alternate_function << shift);
    }
}

/*
 * Writes a logic level to a GPIO pin.
 */

void GPIO_Write(GPIO_TypeDef *port, uint8_t pin, uint8_t state)
{
    if(state)
    {
        port->BSRR = (1UL << pin);
    }
    else
    {
        port->BSRR = (1UL << (pin + 16U));
    }
}

/**
 * Reads the logic level from a GPIO pin.
 */

uint8_t GPIO_Read(GPIO_TypeDef *port, uint8_t pin)
{
    return (port->IDR & (1UL << pin)) ? 1U : 0U;
}

/*
 * Toggles the logic level of a GPIO pin.
 */

void GPIO_Toggle(GPIO_TypeDef *port, uint8_t pin)
{
    if(port->ODR & (1UL << pin))
    {
        port->BSRR = (1UL << (pin + 16U));
    }
    else
    {
        port->BSRR = (1UL << pin);
    }
}
