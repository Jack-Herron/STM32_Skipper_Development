/*
 * RCC.c
 *
 * STM32H7A3 register-level RCC initialization.
 *
 * Requires:
 *   - stm32h7xx.h included before this file, or from your project header.
 *   - The generated clock #defines included before this file.
 *   - Error_Handler() available.
 */

#include "stm32h7xx.h"
#include "STM32H7A3_Clock_Tree.h"
#include "STM32H7A3_RCC.h"

#ifndef RCC_TIMEOUT
#define RCC_TIMEOUT                             1000000U
#endif

#ifndef RCC_HSE_BYPASS_ENABLE
#define RCC_HSE_BYPASS_ENABLE                  	1U
#endif

#ifndef CLOCK_FLASH_LATENCY
#define CLOCK_FLASH_LATENCY                     6U
#endif

#define RCC_PLL_SOURCE_HSI                      0U
#define RCC_PLL_SOURCE_CSI                      1U
#define RCC_PLL_SOURCE_HSE                      2U

#define RCC_SYSCLK_SOURCE_HSI                   0U
#define RCC_SYSCLK_SOURCE_CSI                   1U
#define RCC_SYSCLK_SOURCE_HSE                   2U
#define RCC_SYSCLK_SOURCE_PLL1_P                3U

static void RCC_WriteField(volatile uint32_t *reg,
                           uint32_t mask,
                           uint32_t pos,
                           uint32_t value)
{
    *reg = (*reg & ~mask) | ((value << pos) & mask);
}

static void RCC_WaitSet(volatile uint32_t *reg, uint32_t mask)
{
    uint32_t timeout = RCC_TIMEOUT;

    while(((*reg & mask) == 0U) && (timeout > 0U))
    {
        timeout--;
    }

    if(timeout == 0U)
    {
        while(1); // Error_Handler();
    }
}

static uint32_t RCC_PLLSourceEncode(uint32_t source)
{
    switch(source)
    {
        case RCC_PLL_SOURCE_HSI: return 0x0U;
        case RCC_PLL_SOURCE_CSI: return 0x1U;
        case RCC_PLL_SOURCE_HSE: return 0x2U;
        default:                 return 0x3U;
    }
}

static uint32_t RCC_PrescalerEncode(uint32_t div)
{
    switch(div)
    {
        case 1U:   return 0x0U;
        case 2U:   return 0x8U;
        case 4U:   return 0x9U;
        case 8U:   return 0xAU;
        case 16U:  return 0xBU;
        case 64U:  return 0xCU;
        case 128U: return 0xDU;
        case 256U: return 0xEU;
        case 512U: return 0xFU;
        default:   return 0x0U;
    }
}

static uint32_t RCC_APBPrescalerEncode(uint32_t div)
{
    switch(div)
    {
        case 1U:  return 0x0U;
        case 2U:  return 0x4U;
        case 4U:  return 0x5U;
        case 8U:  return 0x6U;
        case 16U: return 0x7U;
        default:  return 0x0U;
    }
}

static uint32_t RCC_PLLInputRangeEncode(uint32_t input_freq)
{
    if(input_freq < 2000000UL)
    {
        return 0U;
    }
    else if(input_freq < 4000000UL)
    {
        return 1U;
    }
    else if(input_freq < 8000000UL)
    {
        return 2U;
    }
    else
    {
        return 3U;
    }
}

static uint32_t RCC_PLLSourceFreq(uint32_t source)
{
    switch(source)
    {
        case RCC_PLL_SOURCE_HSI: return CLOCK_HSI_FREQ;
        case RCC_PLL_SOURCE_CSI: return CLOCK_CSI_FREQ;
        case RCC_PLL_SOURCE_HSE: return CLOCK_HSE_FREQ;
        default:                 return CLOCK_HSE_FREQ;
    }
}

static uint32_t RCC_SYSCLKSwitchEncode(uint32_t source)
{
    switch(source)
    {
        case RCC_SYSCLK_SOURCE_HSI:    return 0U;
        case RCC_SYSCLK_SOURCE_CSI:    return 1U;
        case RCC_SYSCLK_SOURCE_HSE:    return 2U;
        case RCC_SYSCLK_SOURCE_PLL1_P: return 3U;
        default:                       return 3U;
    }
}

static void RCC_ConfigureFlash(void)
{
#ifdef FLASH_ACR_LATENCY_Msk
    RCC_WriteField(&FLASH->ACR,
                   FLASH_ACR_LATENCY_Msk,
                   FLASH_ACR_LATENCY_Pos,
                   CLOCK_FLASH_LATENCY);
#else
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | CLOCK_FLASH_LATENCY;
#endif

#ifdef FLASH_ACR_LATENCY_Msk
    while((FLASH->ACR & FLASH_ACR_LATENCY_Msk) !=
          ((CLOCK_FLASH_LATENCY << FLASH_ACR_LATENCY_Pos) & FLASH_ACR_LATENCY_Msk))
    {
    }
#endif
}

static void RCC_EnableOscillators(void)
{
#if RCC_HSE_BYPASS_ENABLE
    RCC->CR |= RCC_CR_HSEBYP;
    RCC->CR |= RCC_CR_HSEEXT;
#else
    RCC->CR &= ~RCC_CR_HSEBYP;
#endif

    RCC->CR |= RCC_CR_HSEON;
    RCC_WaitSet(&RCC->CR, RCC_CR_HSERDY);

    RCC->CR |= RCC_CR_HSI48ON;
    RCC_WaitSet(&RCC->CR, RCC_CR_HSI48RDY);

    RCC->CSR |= RCC_CSR_LSION;
    RCC_WaitSet(&RCC->CSR, RCC_CSR_LSIRDY);
}

static void RCC_ConfigurePLL1(void)
{
    RCC->CR &= ~RCC_CR_PLL1ON;

    while((RCC->CR & RCC_CR_PLL1RDY) != 0U)
    {
    }

    RCC_WriteField(&RCC->PLLCKSELR,
                   RCC_PLLCKSELR_PLLSRC_Msk,
                   RCC_PLLCKSELR_PLLSRC_Pos,
                   RCC_PLLSourceEncode(PLL1_SOURCE));

    RCC_WriteField(&RCC->PLLCKSELR,
                   RCC_PLLCKSELR_DIVM1_Msk,
                   RCC_PLLCKSELR_DIVM1_Pos,
                   PLL1_M);

    RCC_WriteField(&RCC->PLLCFGR,
                   RCC_PLLCFGR_PLL1RGE_Msk,
                   RCC_PLLCFGR_PLL1RGE_Pos,
                   RCC_PLLInputRangeEncode(RCC_PLLSourceFreq(PLL1_SOURCE) / PLL1_M));

    /*
     * PLL1 VCO = 8 MHz / 1 * 70 = 560 MHz.
     * Wide VCO range is required.
     */
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL1VCOSEL;

    RCC->PLLCFGR |= RCC_PLLCFGR_DIVP1EN;
    RCC->PLLCFGR |= RCC_PLLCFGR_DIVQ1EN;
    RCC->PLLCFGR |= RCC_PLLCFGR_DIVR1EN;

#if PLL1_FRACTIONAL_ENABLE
    RCC->PLLCFGR |= RCC_PLLCFGR_PLL1FRACEN;
#else
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL1FRACEN;
#endif

    RCC->PLL1DIVR =
        ((PLL1_N     - 1U) << RCC_PLL1DIVR_N1_Pos) |
        ((PLL1_P_DIV - 1U) << RCC_PLL1DIVR_P1_Pos) |
        ((PLL1_Q_DIV - 1U) << RCC_PLL1DIVR_Q1_Pos) |
        ((PLL1_R_DIV - 1U) << RCC_PLL1DIVR_R1_Pos);

    RCC->PLL1FRACR = PLL1_FRACN << RCC_PLL1FRACR_FRACN1_Pos;

    RCC->CR |= RCC_CR_PLL1ON;
    RCC_WaitSet(&RCC->CR, RCC_CR_PLL1RDY);
}

static void RCC_ConfigurePLL2(void)
{
    RCC->CR &= ~RCC_CR_PLL2ON;

    while((RCC->CR & RCC_CR_PLL2RDY) != 0U)
    {
    }

    RCC_WriteField(&RCC->PLLCKSELR,
                   RCC_PLLCKSELR_DIVM2_Msk,
                   RCC_PLLCKSELR_DIVM2_Pos,
                   PLL2_M);

    RCC_WriteField(&RCC->PLLCFGR,
                   RCC_PLLCFGR_PLL2RGE_Msk,
                   RCC_PLLCFGR_PLL2RGE_Pos,
                   RCC_PLLInputRangeEncode(RCC_PLLSourceFreq(PLL2_SOURCE) / PLL2_M));

    RCC->PLLCFGR |= RCC_PLLCFGR_DIVP2EN;
    RCC->PLLCFGR |= RCC_PLLCFGR_DIVQ2EN;
    RCC->PLLCFGR |= RCC_PLLCFGR_DIVR2EN;

#if PLL2_FRACTIONAL_ENABLE
    RCC->PLLCFGR |= RCC_PLLCFGR_PLL2FRACEN;
#else
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL2FRACEN;
#endif

    RCC->PLL2DIVR =
        ((PLL2_N     - 1U) << RCC_PLL2DIVR_N2_Pos) |
        ((PLL2_P_DIV - 1U) << RCC_PLL2DIVR_P2_Pos) |
        ((PLL2_Q_DIV - 1U) << RCC_PLL2DIVR_Q2_Pos) |
        ((PLL2_R_DIV - 1U) << RCC_PLL2DIVR_R2_Pos);

    RCC->PLL2FRACR = PLL2_FRACN << RCC_PLL2FRACR_FRACN2_Pos;

    RCC->CR |= RCC_CR_PLL2ON;
    RCC_WaitSet(&RCC->CR, RCC_CR_PLL2RDY);
}

static void RCC_ConfigurePLL3(void)
{
    RCC->CR &= ~RCC_CR_PLL3ON;

    while((RCC->CR & RCC_CR_PLL3RDY) != 0U)
    {
    }

    RCC_WriteField(&RCC->PLLCKSELR,
                   RCC_PLLCKSELR_DIVM3_Msk,
                   RCC_PLLCKSELR_DIVM3_Pos,
                   PLL3_M);

    RCC_WriteField(&RCC->PLLCFGR,
                   RCC_PLLCFGR_PLL3RGE_Msk,
                   RCC_PLLCFGR_PLL3RGE_Pos,
                   RCC_PLLInputRangeEncode(RCC_PLLSourceFreq(PLL3_SOURCE) / PLL3_M));

    RCC->PLLCFGR |= RCC_PLLCFGR_DIVP3EN;
    RCC->PLLCFGR |= RCC_PLLCFGR_DIVQ3EN;
    RCC->PLLCFGR |= RCC_PLLCFGR_DIVR3EN;

#if PLL3_FRACTIONAL_ENABLE
    RCC->PLLCFGR |= RCC_PLLCFGR_PLL3FRACEN;
#else
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLL3FRACEN;
#endif

    RCC->PLL3DIVR =
        ((PLL3_N     - 1U) << RCC_PLL3DIVR_N3_Pos) |
        ((PLL3_P_DIV - 1U) << RCC_PLL3DIVR_P3_Pos) |
        ((PLL3_Q_DIV - 1U) << RCC_PLL3DIVR_Q3_Pos) |
        ((PLL3_R_DIV - 1U) << RCC_PLL3DIVR_R3_Pos);

    RCC->PLL3FRACR = PLL3_FRACN << RCC_PLL3FRACR_FRACN3_Pos;

    RCC->CR |= RCC_CR_PLL3ON;
    RCC_WaitSet(&RCC->CR, RCC_CR_PLL3RDY);
}

static void RCC_ConfigureBusDividers(void)
{
    /*
     * STM32H7A3/H7B3 RCC register layout:
     *
     * CDCFGR1:
     *   CDCPRE  = CPU domain prescaler
     *   HPRE    = AHB prescaler
     *
     * CDCFGR2:
     *   CDPPRE1 = APB1 prescaler
     *   CDPPRE2 = APB2 prescaler
     *
     * SRDCFGR:
     *   SRDPPRE = APB4 prescaler
     */

    RCC_WriteField(&RCC->CDCFGR1,
                   RCC_CDCFGR1_CDCPRE_Msk,
                   RCC_CDCFGR1_CDCPRE_Pos,
                   RCC_PrescalerEncode(CLOCK_CPU_DIV));

    RCC_WriteField(&RCC->CDCFGR1,
                   RCC_CDCFGR1_HPRE_Msk,
                   RCC_CDCFGR1_HPRE_Pos,
                   RCC_PrescalerEncode(CLOCK_AHB_DIV));

    RCC_WriteField(&RCC->CDCFGR2,
                   RCC_CDCFGR2_CDPPRE1_Msk,
                   RCC_CDCFGR2_CDPPRE1_Pos,
                   RCC_APBPrescalerEncode(CLOCK_APB1_DIV));

    RCC_WriteField(&RCC->CDCFGR2,
                   RCC_CDCFGR2_CDPPRE2_Msk,
                   RCC_CDCFGR2_CDPPRE2_Pos,
                   RCC_APBPrescalerEncode(CLOCK_APB2_DIV));

    RCC_WriteField(&RCC->SRDCFGR,
                   RCC_SRDCFGR_SRDPPRE_Msk,
                   RCC_SRDCFGR_SRDPPRE_Pos,
                   RCC_APBPrescalerEncode(CLOCK_APB4_DIV));
}

static void RCC_SwitchSYSCLK(void)
{
    uint32_t sw;

    sw = RCC_SYSCLKSwitchEncode(SYSCLK_SOURCE);

    RCC_WriteField(&RCC->CFGR,
                   RCC_CFGR_SW_Msk,
                   RCC_CFGR_SW_Pos,
                   sw);

    while(((RCC->CFGR & RCC_CFGR_SWS_Msk) >> RCC_CFGR_SWS_Pos) != sw)
    {
    }
}


static void RCC_SetVOS(void)
{
    /*
     * Enable SYSCFG clock.
     * Required for VOS0 on STM32H7A3.
     */

    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;

    /*
     * Configure power supply.
     */

    PWR->CR3 &= ~PWR_CR3_LDOEN;
    PWR->CR3 |=  PWR_CR3_SMPSEN;

#if (CLOCK_SYSCLK_FREQ > 225000000UL)

    /*
     * VOS0.
     */

    PWR->SRDCR &= ~PWR_SRDCR_VOS_Msk;
    PWR->SRDCR |=  (3UL << PWR_SRDCR_VOS_Pos);

    while((PWR->SRDCR & PWR_SRDCR_VOSRDY) == 0U);

#elif (CLOCK_SYSCLK_FREQ > 180000000UL)

    /*
     * VOS1.
     */

    PWR->SRDCR &= ~PWR_SRDCR_VOS_Msk;
    PWR->SRDCR |=  (2UL << PWR_SRDCR_VOS_Pos);

    while((PWR->SRDCR & PWR_SRDCR_VOSRDY) == 0U);

#else

    /*
     * VOS2.
     */

    PWR->SRDCR &= ~PWR_SRDCR_VOS_Msk;
    PWR->SRDCR |=  (1UL << PWR_SRDCR_VOS_Pos);

    while((PWR->SRDCR & PWR_SRDCR_VOSRDY) == 0U);

#endif
}

static void RCC_ConfigurePeripheralClocks(void)
{
#ifdef RCC_D1CCIPR_CKPERSEL_Msk
    RCC_WriteField(&RCC->D1CCIPR,
                   RCC_D1CCIPR_CKPERSEL_Msk,
                   RCC_D1CCIPR_CKPERSEL_Pos,
                   PER_CLOCK_SOURCE);
#endif

#ifdef RCC_BDCR_RTCSEL_Msk
    RCC_WriteField(&RCC->BDCR,
                   RCC_BDCR_RTCSEL_Msk,
                   RCC_BDCR_RTCSEL_Pos,
                   CLOCK_RTC_SOURCE);
#endif

#ifdef RCC_CFGR_MCO1SEL_Msk
    RCC_WriteField(&RCC->CFGR,
                   RCC_CFGR_MCO1SEL_Msk,
                   RCC_CFGR_MCO1SEL_Pos,
                   CLOCK_MCO1_SOURCE);
#endif

#ifdef RCC_D1CCIPR_FMCSEL_Msk
    RCC_WriteField(&RCC->D1CCIPR,
                   RCC_D1CCIPR_FMCSEL_Msk,
                   RCC_D1CCIPR_FMCSEL_Pos,
                   CLOCK_FMC_SOURCE);
#endif

#ifdef RCC_D1CCIPR_OSPISEL_Msk
    RCC_WriteField(&RCC->D1CCIPR,
                   RCC_D1CCIPR_OSPISEL_Msk,
                   RCC_D1CCIPR_OSPISEL_Pos,
                   CLOCK_OCTOSPI_SOURCE);
#endif

#ifdef RCC_D1CCIPR_OCTOSPISEL_Msk
    RCC_WriteField(&RCC->D1CCIPR,
                   RCC_D1CCIPR_OCTOSPISEL_Msk,
                   RCC_D1CCIPR_OCTOSPISEL_Pos,
                   CLOCK_OCTOSPI_SOURCE);
#endif

#ifdef RCC_D1CCIPR_SDMMCSEL_Msk
    RCC_WriteField(&RCC->D1CCIPR,
                   RCC_D1CCIPR_SDMMCSEL_Msk,
                   RCC_D1CCIPR_SDMMCSEL_Pos,
                   CLOCK_SDMMC12_SOURCE);
#endif

#ifdef RCC_D2CCIP1R_FDCANSEL_Msk
    RCC_WriteField(&RCC->D2CCIP1R,
                   RCC_D2CCIP1R_FDCANSEL_Msk,
                   RCC_D2CCIP1R_FDCANSEL_Pos,
                   CLOCK_FDCAN_SOURCE);
#endif

#ifdef RCC_D2CCIP2R_I2C123SEL_Msk
    RCC_WriteField(&RCC->D2CCIP2R,
                   RCC_D2CCIP2R_I2C123SEL_Msk,
                   RCC_D2CCIP2R_I2C123SEL_Pos,
                   CLOCK_I2C123_SOURCE);
#endif

#ifdef RCC_D3CCIPR_I2C4SEL_Msk
    RCC_WriteField(&RCC->D3CCIPR,
                   RCC_D3CCIPR_I2C4SEL_Msk,
                   RCC_D3CCIPR_I2C4SEL_Pos,
                   CLOCK_I2C4_SOURCE);
#endif

#ifdef RCC_D2CCIP2R_USART16910SEL_Msk
    RCC_WriteField(&RCC->D2CCIP2R,
                   RCC_D2CCIP2R_USART16910SEL_Msk,
                   RCC_D2CCIP2R_USART16910SEL_Pos,
                   CLOCK_USART16910_SOURCE);
#endif

#ifdef RCC_D2CCIP2R_USART234578SEL_Msk
    RCC_WriteField(&RCC->D2CCIP2R,
                   RCC_D2CCIP2R_USART234578SEL_Msk,
                   RCC_D2CCIP2R_USART234578SEL_Pos,
                   CLOCK_USART234578_SOURCE);
#endif

#ifdef RCC_D3CCIPR_LPUART1SEL_Msk
    RCC_WriteField(&RCC->D3CCIPR,
                   RCC_D3CCIPR_LPUART1SEL_Msk,
                   RCC_D3CCIPR_LPUART1SEL_Pos,
                   CLOCK_LPUART1_SOURCE);
#endif

#ifdef RCC_D2CCIP1R_SPI123SEL_Msk
    RCC_WriteField(&RCC->D2CCIP1R,
                   RCC_D2CCIP1R_SPI123SEL_Msk,
                   RCC_D2CCIP1R_SPI123SEL_Pos,
                   CLOCK_SPI123_SOURCE);
#endif

#ifdef RCC_D2CCIP1R_SPI45SEL_Msk
    RCC_WriteField(&RCC->D2CCIP1R,
                   RCC_D2CCIP1R_SPI45SEL_Msk,
                   RCC_D2CCIP1R_SPI45SEL_Pos,
                   CLOCK_SPI45_SOURCE);
#endif

#ifdef RCC_D3CCIPR_SPI6SEL_Msk
    RCC_WriteField(&RCC->D3CCIPR,
                   RCC_D3CCIPR_SPI6SEL_Msk,
                   RCC_D3CCIPR_SPI6SEL_Pos,
                   CLOCK_SPI6_SOURCE);
#endif

#ifdef RCC_D2CCIP1R_SAI1SEL_Msk
    RCC_WriteField(&RCC->D2CCIP1R,
                   RCC_D2CCIP1R_SAI1SEL_Msk,
                   RCC_D2CCIP1R_SAI1SEL_Pos,
                   CLOCK_SAI1_SOURCE);
#endif

#ifdef RCC_D2CCIP1R_SAI23SEL_Msk
    RCC_WriteField(&RCC->D2CCIP1R,
                   RCC_D2CCIP1R_SAI23SEL_Msk,
                   RCC_D2CCIP1R_SAI23SEL_Pos,
                   CLOCK_SAI2A_SOURCE);
#endif

#ifdef RCC_D2CCIP1R_SPDIFSEL_Msk
    RCC_WriteField(&RCC->D2CCIP1R,
                   RCC_D2CCIP1R_SPDIFSEL_Msk,
                   RCC_D2CCIP1R_SPDIFSEL_Pos,
                   CLOCK_SPDIFRX_SOURCE);
#endif

#ifdef RCC_D2CCIP2R_USBSEL_Msk
    RCC_WriteField(&RCC->D2CCIP2R,
                   RCC_D2CCIP2R_USBSEL_Msk,
                   RCC_D2CCIP2R_USBSEL_Pos,
                   CLOCK_USB_SOURCE);
#endif

#ifdef RCC_D2CCIP2R_RNGSEL_Msk
    RCC_WriteField(&RCC->D2CCIP2R,
                   RCC_D2CCIP2R_RNGSEL_Msk,
                   RCC_D2CCIP2R_RNGSEL_Pos,
                   CLOCK_RNG_SOURCE);
#endif

#ifdef RCC_D3CCIPR_ADCSEL_Msk
    RCC_WriteField(&RCC->D3CCIPR,
                   RCC_D3CCIPR_ADCSEL_Msk,
                   RCC_D3CCIPR_ADCSEL_Pos,
                   CLOCK_ADC_SOURCE);
#endif

#ifdef RCC_D1CCIPR_LTDCSEL_Msk
    RCC_WriteField(&RCC->D1CCIPR,
                   RCC_D1CCIPR_LTDCSEL_Msk,
                   RCC_D1CCIPR_LTDCSEL_Pos,
                   CLOCK_LTDC_SOURCE);
#endif

#ifdef RCC_D2CCIP2R_LPTIM1SEL_Msk
    RCC_WriteField(&RCC->D2CCIP2R,
                   RCC_D2CCIP2R_LPTIM1SEL_Msk,
                   RCC_D2CCIP2R_LPTIM1SEL_Pos,
                   CLOCK_LPTIM1_SOURCE);
#endif

#ifdef RCC_D3CCIPR_LPTIM2SEL_Msk
    RCC_WriteField(&RCC->D3CCIPR,
                   RCC_D3CCIPR_LPTIM2SEL_Msk,
                   RCC_D3CCIPR_LPTIM2SEL_Pos,
                   CLOCK_LPTIM2_SOURCE);
#endif

#ifdef RCC_D3CCIPR_LPTIM3SEL_Msk
    RCC_WriteField(&RCC->D3CCIPR,
                   RCC_D3CCIPR_LPTIM3SEL_Msk,
                   RCC_D3CCIPR_LPTIM3SEL_Pos,
                   CLOCK_LPTIM3_SOURCE);
#endif

#ifdef RCC_D2CCIP1R_SWPMI1SEL_Msk
    RCC_WriteField(&RCC->D2CCIP1R,
                   RCC_D2CCIP1R_SWPMI1SEL_Msk,
                   RCC_D2CCIP1R_SWPMI1SEL_Pos,
                   CLOCK_SWPMI_SOURCE);
#endif

#ifdef RCC_D2CCIP1R_DFSDM1SEL_Msk
    RCC_WriteField(&RCC->D2CCIP1R,
                   RCC_D2CCIP1R_DFSDM1SEL_Msk,
                   RCC_D2CCIP1R_DFSDM1SEL_Pos,
                   CLOCK_DFSDM1_SOURCE);
#endif

#ifdef RCC_D3CCIPR_DFSDM2SEL_Msk
    RCC_WriteField(&RCC->D3CCIPR,
                   RCC_D3CCIPR_DFSDM2SEL_Msk,
                   RCC_D3CCIPR_DFSDM2SEL_Pos,
                   CLOCK_DFSDM2_SOURCE);
#endif

#ifdef RCC_D1CCIPR_DCMISEL_Msk
    RCC_WriteField(&RCC->D1CCIPR,
                   RCC_D1CCIPR_DCMISEL_Msk,
                   RCC_D1CCIPR_DCMISEL_Pos,
                   CLOCK_DCMI_PSSI_SOURCE);
#endif

#ifdef RCC_D1CCIPR_TRACESEL_Msk
    RCC_WriteField(&RCC->D1CCIPR,
                   RCC_D1CCIPR_TRACESEL_Msk,
                   RCC_D1CCIPR_TRACESEL_Pos,
                   CLOCK_TRACE_SOURCE);
#endif
}

int32_t RCC_GetKernelFreq(void *peripheral)
{
    /*
     * Timers
     */

    if ((peripheral == TIM1)  ||
        (peripheral == TIM8)  ||
        (peripheral == TIM15) ||
        (peripheral == TIM16) ||
        (peripheral == TIM17))
    {
        if (CLOCK_APB2_DIV == 1U)
        {
            return CLOCK_PCLK2_FREQ;
        }

        return CLOCK_PCLK2_FREQ * 2U;
    }

    if ((peripheral == TIM2)  ||
        (peripheral == TIM3)  ||
        (peripheral == TIM4)  ||
        (peripheral == TIM5)  ||
        (peripheral == TIM6)  ||
        (peripheral == TIM7)  ||
        (peripheral == TIM12) ||
        (peripheral == TIM13) ||
        (peripheral == TIM14))
    {
        if (CLOCK_APB1_DIV == 1U)
        {
            return CLOCK_PCLK1_FREQ;
        }

        return CLOCK_PCLK1_FREQ * 2U;
    }

    /*
     * GPIO
     */

    if ((peripheral == GPIOA) ||
        (peripheral == GPIOB) ||
        (peripheral == GPIOC) ||
        (peripheral == GPIOD) ||
        (peripheral == GPIOE) ||
        (peripheral == GPIOF) ||
        (peripheral == GPIOG) ||
        (peripheral == GPIOH))
    {
        return CLOCK_HCLK_FREQ;
    }

    /*
     * USART / UART
     */

    if ((peripheral == USART1) ||
        (peripheral == USART6))
    {
        return CLOCK_USART16910_FREQ;
    }

#ifdef USART9
    if (peripheral == USART9)
    {
        return CLOCK_USART16910_FREQ;
    }
#endif

#ifdef USART10
    if (peripheral == USART10)
    {
        return CLOCK_USART16910_FREQ;
    }
#endif

    if ((peripheral == USART2) ||
        (peripheral == USART3) ||
        (peripheral == UART4)  ||
        (peripheral == UART5)  ||
        (peripheral == UART7)  ||
        (peripheral == UART8))
    {
        return CLOCK_USART234578_FREQ;
    }

#ifdef LPUART1
    if (peripheral == LPUART1)
    {
        return CLOCK_LPUART1_FREQ;
    }
#endif

    /*
     * SPI
     */

    if ((peripheral == SPI1) ||
        (peripheral == SPI2) ||
        (peripheral == SPI3))
    {
        return CLOCK_SPI123_FREQ;
    }

    if ((peripheral == SPI4) ||
        (peripheral == SPI5))
    {
        return CLOCK_SPI45_FREQ;
    }

#ifdef SPI6
    if (peripheral == SPI6)
    {
        return CLOCK_SPI6_FREQ;
    }
#endif

    /*
     * I2C
     */

    if ((peripheral == I2C1) ||
        (peripheral == I2C2) ||
        (peripheral == I2C3))
    {
        return CLOCK_I2C123_FREQ;
    }

#ifdef I2C4
    if (peripheral == I2C4)
    {
        return CLOCK_I2C4_FREQ;
    }
#endif

    /*
     * LPTIM
     */

#ifdef LPTIM1
    if (peripheral == LPTIM1)
    {
        return CLOCK_LPTIM1_FREQ;
    }
#endif

#ifdef LPTIM2
    if (peripheral == LPTIM2)
    {
        return CLOCK_LPTIM2_FREQ;
    }
#endif

#ifdef LPTIM3
    if (peripheral == LPTIM3)
    {
        return CLOCK_LPTIM3_FREQ;
    }
#endif

    /*
     * USB
     */

#ifdef USB_OTG_FS
    if (peripheral == USB_OTG_FS)
    {
        return CLOCK_USB_FREQ;
    }
#endif

#ifdef USB_OTG_HS
    if (peripheral == USB_OTG_HS)
    {
        return CLOCK_USB_FREQ;
    }
#endif

    /*
     * ADC
     */

#ifdef ADC1
    if (peripheral == ADC1)
    {
        return CLOCK_ADC_FREQ;
    }
#endif

#ifdef ADC2
    if (peripheral == ADC2)
    {
        return CLOCK_ADC_FREQ;
    }
#endif

    /*
     * SDMMC
     */

#ifdef SDMMC1
    if (peripheral == SDMMC1)
    {
        return CLOCK_SDMMC12_FREQ;
    }
#endif

#ifdef SDMMC2
    if (peripheral == SDMMC2)
    {
        return CLOCK_SDMMC12_FREQ;
    }
#endif

    /*
     * FDCAN
     */

#ifdef FDCAN1
    if (peripheral == FDCAN1)
    {
        return CLOCK_FDCAN_FREQ;
    }
#endif

#ifdef FDCAN2
    if (peripheral == FDCAN2)
    {
        return CLOCK_FDCAN_FREQ;
    }
#endif

    /*
     * FMC / OCTOSPI
     */

#ifdef FMC_Bank1_R
    if (peripheral == FMC_Bank1_R)
    {
        return CLOCK_FMC_FREQ;
    }
#endif

#ifdef OCTOSPI1
    if (peripheral == OCTOSPI1)
    {
        return CLOCK_OCTOSPI_FREQ;
    }
#endif

#ifdef OCTOSPI2
    if (peripheral == OCTOSPI2)
    {
        return CLOCK_OCTOSPI_FREQ;
    }
#endif

    /*
     * SAI
     */

#ifdef SAI1
    if (peripheral == SAI1)
    {
        return CLOCK_SAI1_FREQ;
    }
#endif

#ifdef SAI2
    if (peripheral == SAI2)
    {
        return CLOCK_SAI2A_FREQ;
    }
#endif

    /*
     * SPDIFRX
     */

#ifdef SPDIFRX
    if (peripheral == SPDIFRX)
    {
        return CLOCK_SPDIFRX_FREQ;
    }
#endif

    /*
     * RNG
     */

#ifdef RNG
    if (peripheral == RNG)
    {
        return CLOCK_RNG_FREQ;
    }
#endif

    /*
     * LTDC
     */

#ifdef LTDC
    if (peripheral == LTDC)
    {
        return CLOCK_LTDC_FREQ;
    }
#endif

    /*
     * CEC
     */

#ifdef HDMI_CEC
    if (peripheral == HDMI_CEC)
    {
        return CLOCK_CEC_FREQ;
    }
#endif

#ifdef CEC
    if (peripheral == CEC)
    {
        return CLOCK_CEC_FREQ;
    }
#endif

    /*
     * DCMI / PSSI
     */

#ifdef DCMI
    if (peripheral == DCMI)
    {
        return CLOCK_DCMI_PSSI_FREQ;
    }
#endif

#ifdef PSSI
    if (peripheral == PSSI)
    {
        return CLOCK_DCMI_PSSI_FREQ;
    }
#endif

    /*
     * SWPMI
     */

#ifdef SWPMI1
    if (peripheral == SWPMI1)
    {
        return CLOCK_SWPMI_FREQ;
    }
#endif

    /*
     * DFSDM
     */

#ifdef DFSDM1_Channel0
    if (peripheral == DFSDM1_Channel0)
    {
        return CLOCK_DFSDM1_FREQ;
    }
#endif

#ifdef DFSDM2_Channel0
    if (peripheral == DFSDM2_Channel0)
    {
        return CLOCK_DFSDM2_FREQ;
    }
#endif

    return 0U;
}

void RCC_Init(void)
{
    RCC->CKGAENR = 0xFFFFFFFFUL;

    RCC_SetVOS();

    RCC_ConfigureFlash();

    RCC_EnableOscillators();

    RCC_ConfigurePLL1();
    RCC_ConfigurePLL2();
    RCC_ConfigurePLL3();

    RCC_ConfigureBusDividers();

    RCC_ConfigurePeripheralClocks();

    RCC_SwitchSYSCLK();
}

