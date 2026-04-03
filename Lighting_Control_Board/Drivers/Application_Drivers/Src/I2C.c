/*
 * I2C.c
 *
 *  Created on: Apr 2, 2026
 *      Author: jackh
 *
 *  STM32F103 version
 *  Supports only: I2C1 on PB6 (SCL), PB7 (SDA)
 */

#include "I2C.h"
#include "stm32f1xx.h"
#include "Clock.h"
#include <stdlib.h>

#define I2C___LOOP_TIMEOUT    20

static I2C_Inst_TypeDef I2C_Inst;

static void I2C___GPIO_Init(void)
{
    /* Enable GPIOB and AFIO clocks */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

    /*
     * PB6 = I2C1_SCL
     * PB7 = I2C1_SDA
     *
     * Alternate function open-drain, output mode, max speed 50 MHz
     *
     * MODEy = 11
     * CNFy  = 11
     */

    GPIOB->CRL &= ~(
          GPIO_CRL_MODE6 | GPIO_CRL_CNF6
        | GPIO_CRL_MODE7 | GPIO_CRL_CNF7
    );

    GPIOB->CRL |= (
          GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1
        | GPIO_CRL_CNF6_0  | GPIO_CRL_CNF6_1
        | GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1
        | GPIO_CRL_CNF7_0  | GPIO_CRL_CNF7_1
    );

    GPIOB->BSRR = GPIO_BSRR_BS6 | GPIO_BSRR_BS7;

    /* Make sure remap is disabled so I2C1 stays on PB6/PB7 */
    AFIO->MAPR &= ~AFIO_MAPR_I2C1_REMAP;
}

int I2C___Init(I2C___Conf_TypeDef* I2C_Conf)
{
    if (I2C_Conf == NULL)
    {
        return 1;
    }

    I2C___GPIO_Init();

    /* Enable I2C1 clock */
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    I2C_Inst.I2Cx = I2C1;
    I2C_Inst.Conf = *I2C_Conf;
    I2C_Inst.is_Initialized = 1;

    /* Optional peripheral reset */
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    I2C1->CR1 = 0;
    I2C1->CR2 = (((uint32_t)I2C_FREQ / 1000000U) << I2C_CR2_FREQ_Pos);

    uint32_t APB_Cycles_Per_CCR;
    uint8_t fast_Mode;

    if (I2C_Inst.Conf.Freq <= 100000U)
    {
        APB_Cycles_Per_CCR = 2U;
        fast_Mode = 0U;
    }
    else if (I2C_Inst.Conf.Fm_Mode == Fm_2high1low)
    {
        APB_Cycles_Per_CCR = 3U;
        fast_Mode = 1U;
    }
    else
    {
        APB_Cycles_Per_CCR = 25U;
        fast_Mode = 1U;
    }

    uint32_t CCR_Value = ((uint32_t)I2C_FREQ) / (APB_Cycles_Per_CCR * I2C_Inst.Conf.Freq);

    if (CCR_Value < 1U)
    {
        CCR_Value = 1U;
    }

    I2C1->CCR =
          ((uint32_t)fast_Mode << I2C_CCR_FS_Pos)
        | ((uint32_t)I2C_Inst.Conf.Fm_Mode << I2C_CCR_DUTY_Pos)
        | (CCR_Value << I2C_CCR_CCR_Pos);

    I2C1->TRISE =
        ((I2C_FREQ / (1000000000UL / I2C_Inst.Conf.Rise_Time_ns)) + 1U) & I2C_TRISE_TRISE_Msk;

    I2C1->CR1 |= I2C_CR1_PE;

    return 0;
}

int I2C___Write(uint8_t address, uint8_t* buffer, uint32_t write_Length)
{
    if (!I2C_Inst.is_Initialized)
    {
        return 1;
    }

    if ((buffer == NULL) && (write_Length != 0U))
    {
        return 1;
    }

    I2C_TypeDef* I2Cx = I2C1;

    uint32_t start_Tick = clock___Get_Millis();
    uint32_t loop_Tick = start_Tick;

    while ((I2Cx->SR2 & I2C_SR2_BUSY) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->CR1 |= I2C_CR1_START;

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & I2C_SR1_SB)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->DR = (uint8_t)((address << 1U) | 0U);

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))) &&
           ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    if (I2Cx->SR1 & I2C_SR1_AF)
    {
        I2Cx->SR1 &= ~I2C_SR1_AF;
        I2Cx->CR1 |= I2C_CR1_STOP;
        return 1;
    }

    (void)I2Cx->SR1;
    (void)I2Cx->SR2;

    for (uint32_t i = 0; i < write_Length; i++)
    {
        start_Tick = clock___Get_Millis();
        loop_Tick = start_Tick;

        while ((!(I2Cx->SR1 & I2C_SR1_TXE)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
        {
            loop_Tick = clock___Get_Millis();
        }

        if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
        {
            return 1;
        }

        I2Cx->DR = buffer[i];

        start_Tick = clock___Get_Millis();
        loop_Tick = start_Tick;

        while ((!(I2Cx->SR1 & (I2C_SR1_BTF | I2C_SR1_AF))) &&
               ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
        {
            loop_Tick = clock___Get_Millis();
        }

        if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
        {
            return 1;
        }

        if (I2Cx->SR1 & I2C_SR1_AF)
        {
            I2Cx->SR1 &= ~I2C_SR1_AF;
            I2Cx->CR1 |= I2C_CR1_STOP;
            return 1;
        }
    }

    I2Cx->CR1 |= I2C_CR1_STOP;

    return 0;
}

int I2C___Read_Reg(uint8_t address, uint8_t reg_Address, uint8_t* buffer, uint32_t read_Length)
{
    if (!I2C_Inst.is_Initialized)
    {
        return 1;
    }

    if ((buffer == NULL) && (read_Length != 0U))
    {
        return 1;
    }

    I2C_TypeDef* I2Cx = I2C1;

    uint32_t start_Tick = clock___Get_Millis();
    uint32_t loop_Tick = start_Tick;

    while ((I2Cx->SR2 & I2C_SR2_BUSY) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->CR1 |= I2C_CR1_START;

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & I2C_SR1_SB)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->DR = (uint8_t)((address << 1U) | 0U);

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))) &&
           ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    if (I2Cx->SR1 & I2C_SR1_AF)
    {
        I2Cx->SR1 &= ~I2C_SR1_AF;
        I2Cx->CR1 |= I2C_CR1_STOP;
        return 1;
    }

    (void)I2Cx->SR1;
    (void)I2Cx->SR2;

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & I2C_SR1_TXE)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->DR = reg_Address;

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & I2C_SR1_BTF)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->CR1 |= I2C_CR1_START;

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & I2C_SR1_SB)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->DR = (uint8_t)((address << 1U) | 1U);

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))) &&
           ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    if (I2Cx->SR1 & I2C_SR1_AF)
    {
        I2Cx->SR1 &= ~I2C_SR1_AF;
        I2Cx->CR1 |= I2C_CR1_STOP;
        return 1;
    }

    (void)I2Cx->SR1;
    (void)I2Cx->SR2;

    if (read_Length == 0U)
    {
        I2Cx->CR1 |= I2C_CR1_STOP;
    }
    else if (read_Length == 1U)
    {
        I2Cx->CR1 &= ~I2C_CR1_ACK;
        I2Cx->CR1 |= I2C_CR1_STOP;

        start_Tick = clock___Get_Millis();
        loop_Tick = start_Tick;

        while ((!(I2Cx->SR1 & I2C_SR1_RXNE)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
        {
            loop_Tick = clock___Get_Millis();
        }

        if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
        {
            return 1;
        }

        buffer[0] = (uint8_t)I2Cx->DR;
    }
    else if (read_Length == 2U)
    {
        I2Cx->CR1 &= ~I2C_CR1_ACK;
        I2Cx->CR1 |= I2C_CR1_POS;

        start_Tick = clock___Get_Millis();
        loop_Tick = start_Tick;

        while ((!(I2Cx->SR1 & I2C_SR1_BTF)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
        {
            loop_Tick = clock___Get_Millis();
        }

        if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
        {
            return 1;
        }

        I2Cx->CR1 |= I2C_CR1_STOP;

        buffer[0] = (uint8_t)I2Cx->DR;
        buffer[1] = (uint8_t)I2Cx->DR;

        I2Cx->CR1 &= ~I2C_CR1_POS;
    }
    else
    {
        I2Cx->CR1 |= I2C_CR1_ACK;

        uint32_t i = 0U;
        while (i < read_Length)
        {
            if (i == (read_Length - 3U))
            {
                start_Tick = clock___Get_Millis();
                loop_Tick = start_Tick;

                while ((!(I2Cx->SR1 & I2C_SR1_BTF)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
                {
                    loop_Tick = clock___Get_Millis();
                }

                if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
                {
                    return 1;
                }

                I2Cx->CR1 &= ~I2C_CR1_ACK;
                buffer[i++] = (uint8_t)I2Cx->DR;

                I2Cx->CR1 |= I2C_CR1_STOP;
                buffer[i++] = (uint8_t)I2Cx->DR;

                start_Tick = clock___Get_Millis();
                loop_Tick = start_Tick;

                while ((!(I2Cx->SR1 & I2C_SR1_RXNE)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
                {
                    loop_Tick = clock___Get_Millis();
                }

                if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
                {
                    return 1;
                }

                buffer[i++] = (uint8_t)I2Cx->DR;
                break;
            }

            start_Tick = clock___Get_Millis();
            loop_Tick = start_Tick;

            while ((!(I2Cx->SR1 & I2C_SR1_RXNE)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
            {
                loop_Tick = clock___Get_Millis();
            }

            if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
            {
                return 1;
            }

            buffer[i++] = (uint8_t)I2Cx->DR;
        }
    }

    I2Cx->CR1 |= I2C_CR1_ACK;
    I2Cx->CR1 &= ~I2C_CR1_POS;

    return 0;
}

int I2C___Read_Reg16(uint8_t address, uint16_t reg_Address, uint8_t* buffer, uint32_t read_Length)
{
    if (!I2C_Inst.is_Initialized)
    {
        return 1;
    }

    if ((buffer == NULL) && (read_Length != 0U))
    {
        return 1;
    }

    I2C_TypeDef* I2Cx = I2C1;

    uint32_t start_Tick = clock___Get_Millis();
    uint32_t loop_Tick = start_Tick;

    while ((I2Cx->SR2 & I2C_SR2_BUSY) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->CR1 |= I2C_CR1_START;

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & I2C_SR1_SB)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->DR = (uint8_t)((address << 1U) | 0U);

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))) &&
           ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    if (I2Cx->SR1 & I2C_SR1_AF)
    {
        I2Cx->SR1 &= ~I2C_SR1_AF;
        I2Cx->CR1 |= I2C_CR1_STOP;
        return 1;
    }

    (void)I2Cx->SR1;
    (void)I2Cx->SR2;

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & I2C_SR1_TXE)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->DR = (uint8_t)((reg_Address >> 8) & 0xFFU);

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & I2C_SR1_BTF)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->DR = (uint8_t)(reg_Address & 0xFFU);

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & I2C_SR1_BTF)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->CR1 |= I2C_CR1_START;

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & I2C_SR1_SB)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    I2Cx->DR = (uint8_t)((address << 1U) | 1U);

    start_Tick = clock___Get_Millis();
    loop_Tick = start_Tick;

    while ((!(I2Cx->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))) &&
           ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
    {
        loop_Tick = clock___Get_Millis();
    }

    if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
    {
        return 1;
    }

    if (I2Cx->SR1 & I2C_SR1_AF)
    {
        I2Cx->SR1 &= ~I2C_SR1_AF;
        I2Cx->CR1 |= I2C_CR1_STOP;
        return 1;
    }

    (void)I2Cx->SR1;
    (void)I2Cx->SR2;

    if (read_Length == 0U)
    {
        I2Cx->CR1 |= I2C_CR1_STOP;
    }
    else if (read_Length == 1U)
    {
        I2Cx->CR1 &= ~I2C_CR1_ACK;
        I2Cx->CR1 |= I2C_CR1_STOP;

        start_Tick = clock___Get_Millis();
        loop_Tick = start_Tick;

        while ((!(I2Cx->SR1 & I2C_SR1_RXNE)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
        {
            loop_Tick = clock___Get_Millis();
        }

        if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
        {
            return 1;
        }

        buffer[0] = (uint8_t)I2Cx->DR;
    }
    else if (read_Length == 2U)
    {
        I2Cx->CR1 &= ~I2C_CR1_ACK;
        I2Cx->CR1 |= I2C_CR1_POS;

        start_Tick = clock___Get_Millis();
        loop_Tick = start_Tick;

        while ((!(I2Cx->SR1 & I2C_SR1_BTF)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
        {
            loop_Tick = clock___Get_Millis();
        }

        if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
        {
            return 1;
        }

        I2Cx->CR1 |= I2C_CR1_STOP;

        buffer[0] = (uint8_t)I2Cx->DR;
        buffer[1] = (uint8_t)I2Cx->DR;

        I2Cx->CR1 &= ~I2C_CR1_POS;
    }
    else
    {
        I2Cx->CR1 |= I2C_CR1_ACK;

        uint32_t i = 0U;
        while (i < read_Length)
        {
            if (i == (read_Length - 3U))
            {
                start_Tick = clock___Get_Millis();
                loop_Tick = start_Tick;

                while ((!(I2Cx->SR1 & I2C_SR1_BTF)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
                {
                    loop_Tick = clock___Get_Millis();
                }

                if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
                {
                    return 1;
                }

                I2Cx->CR1 &= ~I2C_CR1_ACK;
                buffer[i++] = (uint8_t)I2Cx->DR;

                I2Cx->CR1 |= I2C_CR1_STOP;
                buffer[i++] = (uint8_t)I2Cx->DR;

                start_Tick = clock___Get_Millis();
                loop_Tick = start_Tick;

                while ((!(I2Cx->SR1 & I2C_SR1_RXNE)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
                {
                    loop_Tick = clock___Get_Millis();
                }

                if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
                {
                    return 1;
                }

                buffer[i++] = (uint8_t)I2Cx->DR;
                break;
            }

            start_Tick = clock___Get_Millis();
            loop_Tick = start_Tick;

            while ((!(I2Cx->SR1 & I2C_SR1_RXNE)) && ((loop_Tick - start_Tick) < I2C___LOOP_TIMEOUT))
            {
                loop_Tick = clock___Get_Millis();
            }

            if ((loop_Tick - start_Tick) >= I2C___LOOP_TIMEOUT)
            {
                return 1;
            }

            buffer[i++] = (uint8_t)I2Cx->DR;
        }
    }

    I2Cx->CR1 |= I2C_CR1_ACK;
    I2Cx->CR1 &= ~I2C_CR1_POS;

    return 0;
}

int I2C___Is_Initilized(void)
{
    return I2C_Inst.is_Initialized;
}
