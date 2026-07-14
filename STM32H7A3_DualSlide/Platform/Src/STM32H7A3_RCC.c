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
#define RCC_HSE_BYPASS_ENABLE                  	0U
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

    PWR->CR3 &= ~PWR_CR3_SMPSEN;
    PWR->CR3 |=  PWR_CR3_LDOEN;

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

int32_t RCC_DisablePeripheralClock(void *Peripheral)
{
#define RCC_DISABLE_IF(PERIPH, REG, BIT)       \
    do                                         \
    {                                          \
        if(Peripheral == (void *)(PERIPH))     \
        {                                      \
            RCC->REG &= ~(BIT);                \
            (void)RCC->REG;                    \
            return 0;                          \
        }                                      \
    } while(0)

#ifdef GPIOA
    RCC_DISABLE_IF(GPIOA, AHB4ENR, RCC_AHB4ENR_GPIOAEN);
#endif

#ifdef GPIOB
    RCC_DISABLE_IF(GPIOB, AHB4ENR, RCC_AHB4ENR_GPIOBEN);
#endif

#ifdef GPIOC
    RCC_DISABLE_IF(GPIOC, AHB4ENR, RCC_AHB4ENR_GPIOCEN);
#endif

#ifdef GPIOD
    RCC_DISABLE_IF(GPIOD, AHB4ENR, RCC_AHB4ENR_GPIODEN);
#endif

#ifdef GPIOE
    RCC_DISABLE_IF(GPIOE, AHB4ENR, RCC_AHB4ENR_GPIOEEN);
#endif

#ifdef GPIOF
    RCC_DISABLE_IF(GPIOF, AHB4ENR, RCC_AHB4ENR_GPIOFEN);
#endif

#ifdef GPIOG
    RCC_DISABLE_IF(GPIOG, AHB4ENR, RCC_AHB4ENR_GPIOGEN);
#endif

#ifdef GPIOH
    RCC_DISABLE_IF(GPIOH, AHB4ENR, RCC_AHB4ENR_GPIOHEN);
#endif

#ifdef GPIOI
    RCC_DISABLE_IF(GPIOI, AHB4ENR, RCC_AHB4ENR_GPIOIEN);
#endif

#ifdef GPIOJ
    RCC_DISABLE_IF(GPIOJ, AHB4ENR, RCC_AHB4ENR_GPIOJEN);
#endif

#ifdef GPIOK
    RCC_DISABLE_IF(GPIOK, AHB4ENR, RCC_AHB4ENR_GPIOKEN);
#endif

#ifdef I2C1
    RCC_DISABLE_IF(I2C1, APB1LENR, RCC_APB1LENR_I2C1EN);
#endif

#ifdef I2C2
    RCC_DISABLE_IF(I2C2, APB1LENR, RCC_APB1LENR_I2C2EN);
#endif

#ifdef I2C3
    RCC_DISABLE_IF(I2C3, APB1LENR, RCC_APB1LENR_I2C3EN);
#endif

#ifdef I2C4
    RCC_DISABLE_IF(I2C4, APB4ENR, RCC_APB4ENR_I2C4EN);
#endif

#ifdef USART1
    RCC_DISABLE_IF(USART1, APB2ENR, RCC_APB2ENR_USART1EN);
#endif

#ifdef USART2
    RCC_DISABLE_IF(USART2, APB1LENR, RCC_APB1LENR_USART2EN);
#endif

#ifdef USART3
    RCC_DISABLE_IF(USART3, APB1LENR, RCC_APB1LENR_USART3EN);
#endif

#ifdef UART4
    RCC_DISABLE_IF(UART4, APB1LENR, RCC_APB1LENR_UART4EN);
#endif

#ifdef UART5
    RCC_DISABLE_IF(UART5, APB1LENR, RCC_APB1LENR_UART5EN);
#endif

#ifdef USART6
    RCC_DISABLE_IF(USART6, APB2ENR, RCC_APB2ENR_USART6EN);
#endif

#ifdef UART7
    RCC_DISABLE_IF(UART7, APB1LENR, RCC_APB1LENR_UART7EN);
#endif

#ifdef UART8
    RCC_DISABLE_IF(UART8, APB1LENR, RCC_APB1LENR_UART8EN);
#endif

#ifdef LTDC
    RCC_DISABLE_IF(LTDC, APB3ENR, RCC_APB3ENR_LTDCEN);
#endif

#ifdef DMA2D
    RCC_DISABLE_IF(DMA2D, AHB3ENR, RCC_AHB3ENR_DMA2DEN);
#endif

#ifdef FMC_Bank1_R
    RCC_DISABLE_IF(FMC_Bank1_R, AHB3ENR, RCC_AHB3ENR_FMCEN);
#endif

#ifdef OCTOSPI1
    RCC_DISABLE_IF(OCTOSPI1, AHB3ENR, RCC_AHB3ENR_OSPI1EN);
#endif

#ifdef OCTOSPI2
    RCC_DISABLE_IF(OCTOSPI2, AHB3ENR, RCC_AHB3ENR_OSPI2EN);
#endif

#undef RCC_DISABLE_IF

    return -1;
}

int32_t RCC_ResetPeripheral(void *Peripheral)
{
#define RCC_RESET_IF(PERIPH, REG, BIT)         \
    do                                         \
    {                                          \
        if(Peripheral == (void *)(PERIPH))     \
        {                                      \
            RCC->REG |= (BIT);                 \
            RCC->REG &= ~(BIT);                \
            return 0;                          \
        }                                      \
    } while(0)

#ifdef GPIOA
    RCC_RESET_IF(GPIOA, AHB4RSTR, RCC_AHB4RSTR_GPIOARST);
#endif

#ifdef GPIOB
    RCC_RESET_IF(GPIOB, AHB4RSTR, RCC_AHB4RSTR_GPIOBRST);
#endif

#ifdef GPIOC
    RCC_RESET_IF(GPIOC, AHB4RSTR, RCC_AHB4RSTR_GPIOCRST);
#endif

#ifdef GPIOD
    RCC_RESET_IF(GPIOD, AHB4RSTR, RCC_AHB4RSTR_GPIODRST);
#endif

#ifdef GPIOE
    RCC_RESET_IF(GPIOE, AHB4RSTR, RCC_AHB4RSTR_GPIOERST);
#endif

#ifdef GPIOF
    RCC_RESET_IF(GPIOF, AHB4RSTR, RCC_AHB4RSTR_GPIOFRST);
#endif

#ifdef GPIOG
    RCC_RESET_IF(GPIOG, AHB4RSTR, RCC_AHB4RSTR_GPIOGRST);
#endif

#ifdef GPIOH
    RCC_RESET_IF(GPIOH, AHB4RSTR, RCC_AHB4RSTR_GPIOHRST);
#endif

#ifdef I2C1
    RCC_RESET_IF(I2C1, APB1LRSTR, RCC_APB1LRSTR_I2C1RST);
#endif

#ifdef I2C2
    RCC_RESET_IF(I2C2, APB1LRSTR, RCC_APB1LRSTR_I2C2RST);
#endif

#ifdef I2C3
    RCC_RESET_IF(I2C3, APB1LRSTR, RCC_APB1LRSTR_I2C3RST);
#endif

#ifdef I2C4
    RCC_RESET_IF(I2C4, APB4RSTR, RCC_APB4RSTR_I2C4RST);
#endif

#ifdef USART1
    RCC_RESET_IF(USART1, APB2RSTR, RCC_APB2RSTR_USART1RST);
#endif

#ifdef USART2
    RCC_RESET_IF(USART2, APB1LRSTR, RCC_APB1LRSTR_USART2RST);
#endif

#ifdef USART3
    RCC_RESET_IF(USART3, APB1LRSTR, RCC_APB1LRSTR_USART3RST);
#endif

#ifdef UART4
    RCC_RESET_IF(UART4, APB1LRSTR, RCC_APB1LRSTR_UART4RST);
#endif

#ifdef UART5
    RCC_RESET_IF(UART5, APB1LRSTR, RCC_APB1LRSTR_UART5RST);
#endif

#ifdef USART6
    RCC_RESET_IF(USART6, APB2RSTR, RCC_APB2RSTR_USART6RST);
#endif

#ifdef UART7
    RCC_RESET_IF(UART7, APB1LRSTR, RCC_APB1LRSTR_UART7RST);
#endif

#ifdef UART8
    RCC_RESET_IF(UART8, APB1LRSTR, RCC_APB1LRSTR_UART8RST);
#endif

#ifdef LTDC
    RCC_RESET_IF(LTDC, APB3RSTR, RCC_APB3RSTR_LTDCRST);
#endif

#ifdef DMA2D
    RCC_RESET_IF(DMA2D, AHB3RSTR, RCC_AHB3RSTR_DMA2DRST);
#endif

#ifdef FMC_Bank1_R
    RCC_RESET_IF(FMC_Bank1_R, AHB3RSTR, RCC_AHB3RSTR_FMCRST);
#endif

#ifdef OCTOSPI1
    RCC_RESET_IF(OCTOSPI1, AHB3RSTR, RCC_AHB3RSTR_OSPI1RST);
#endif

#ifdef OCTOSPI2
    RCC_RESET_IF(OCTOSPI2, AHB3RSTR, RCC_AHB3RSTR_OSPI2RST);
#endif

#undef RCC_RESET_IF

    return -1;
}

int32_t RCC_EnablePeripheralClock(void *Peripheral)
{
#define RCC_ENABLE_IF(PERIPH, REG, BIT)        \
    do                                         \
    {                                          \
        if(Peripheral == (void *)(PERIPH))     \
        {                                      \
            RCC->REG |= (BIT);                 \
            (void)RCC->REG;                    \
            return 0;                          \
        }                                      \
    } while(0)

    /*
     * AHB1
     */

#ifdef DMA1
    RCC_ENABLE_IF(DMA1, AHB1ENR, RCC_AHB1ENR_DMA1EN);
#endif

#ifdef DMA2
    RCC_ENABLE_IF(DMA2, AHB1ENR, RCC_AHB1ENR_DMA2EN);
#endif

#ifdef ADC12_COMMON
    RCC_ENABLE_IF(ADC12_COMMON, AHB1ENR, RCC_AHB1ENR_ADC12EN);
#endif

#ifdef ETH
#ifdef RCC_AHB1ENR_ETH1MACEN
    RCC_ENABLE_IF(ETH, AHB1ENR, RCC_AHB1ENR_ETH1MACEN);
#endif
#endif

    /*
     * AHB2
     */

#ifdef DCMI
#ifdef RCC_AHB2ENR_DCMIEN
    RCC_ENABLE_IF(DCMI, AHB2ENR, RCC_AHB2ENR_DCMIEN);
#endif
#endif

#ifdef RNG
#ifdef RCC_AHB2ENR_RNGEN
    RCC_ENABLE_IF(RNG, AHB2ENR, RCC_AHB2ENR_RNGEN);
#endif
#endif

#ifdef HASH
#ifdef RCC_AHB2ENR_HASHEN
    RCC_ENABLE_IF(HASH, AHB2ENR, RCC_AHB2ENR_HASHEN);
#endif
#endif

#ifdef CRYP
#ifdef RCC_AHB2ENR_CRYPEN
    RCC_ENABLE_IF(CRYP, AHB2ENR, RCC_AHB2ENR_CRYPEN);
#endif
#endif

#ifdef SDMMC2
#ifdef RCC_AHB2ENR_SDMMC2EN
    RCC_ENABLE_IF(SDMMC2, AHB2ENR, RCC_AHB2ENR_SDMMC2EN);
#endif
#endif

    /*
     * AHB3
     */

#ifdef FMC_Bank1_R
#ifdef RCC_AHB3ENR_FMCEN
    RCC_ENABLE_IF(FMC_Bank1_R, AHB3ENR, RCC_AHB3ENR_FMCEN);
#endif
#endif

#ifdef OCTOSPI1
#ifdef RCC_AHB3ENR_OCTOSPI1EN
    RCC_ENABLE_IF(OCTOSPI1, AHB3ENR, RCC_AHB3ENR_OCTOSPI1EN);
#endif
#endif

#ifdef OCTOSPI2
#ifdef RCC_AHB3ENR_OCTOSPI2EN
    RCC_ENABLE_IF(OCTOSPI2, AHB3ENR, RCC_AHB3ENR_OCTOSPI2EN);
#endif
#endif

#ifdef SDMMC1
#ifdef RCC_AHB3ENR_SDMMC1EN
    RCC_ENABLE_IF(SDMMC1, AHB3ENR, RCC_AHB3ENR_SDMMC1EN);
#endif
#endif

#ifdef JPEG
#ifdef RCC_AHB3ENR_JPEGEN
    RCC_ENABLE_IF(JPEG, AHB3ENR, RCC_AHB3ENR_JPEGEN);
#endif
#endif

#ifdef DMA2D
#ifdef RCC_AHB3ENR_DMA2DEN
    RCC_ENABLE_IF(DMA2D, AHB3ENR, RCC_AHB3ENR_DMA2DEN);
#endif
#endif

#ifdef MDMA
#ifdef RCC_AHB3ENR_MDMAEN
    RCC_ENABLE_IF(MDMA, AHB3ENR, RCC_AHB3ENR_MDMAEN);
#endif
#endif

    /*
     * AHB4
     */

#ifdef GPIOA
    RCC_ENABLE_IF(GPIOA, AHB4ENR, RCC_AHB4ENR_GPIOAEN);
#endif
#ifdef GPIOB
    RCC_ENABLE_IF(GPIOB, AHB4ENR, RCC_AHB4ENR_GPIOBEN);
#endif
#ifdef GPIOC
    RCC_ENABLE_IF(GPIOC, AHB4ENR, RCC_AHB4ENR_GPIOCEN);
#endif
#ifdef GPIOD
    RCC_ENABLE_IF(GPIOD, AHB4ENR, RCC_AHB4ENR_GPIODEN);
#endif
#ifdef GPIOE
    RCC_ENABLE_IF(GPIOE, AHB4ENR, RCC_AHB4ENR_GPIOEEN);
#endif
#ifdef GPIOF
    RCC_ENABLE_IF(GPIOF, AHB4ENR, RCC_AHB4ENR_GPIOFEN);
#endif
#ifdef GPIOG
    RCC_ENABLE_IF(GPIOG, AHB4ENR, RCC_AHB4ENR_GPIOGEN);
#endif
#ifdef GPIOH
    RCC_ENABLE_IF(GPIOH, AHB4ENR, RCC_AHB4ENR_GPIOHEN);
#endif
#ifdef GPIOI
    RCC_ENABLE_IF(GPIOI, AHB4ENR, RCC_AHB4ENR_GPIOIEN);
#endif
#ifdef GPIOJ
    RCC_ENABLE_IF(GPIOJ, AHB4ENR, RCC_AHB4ENR_GPIOJEN);
#endif
#ifdef GPIOK
    RCC_ENABLE_IF(GPIOK, AHB4ENR, RCC_AHB4ENR_GPIOKEN);
#endif

#ifdef CRC
#ifdef RCC_AHB4ENR_CRCEN
    RCC_ENABLE_IF(CRC, AHB4ENR, RCC_AHB4ENR_CRCEN);
#endif
#endif

#ifdef BDMA
#ifdef RCC_AHB4ENR_BDMAEN
    RCC_ENABLE_IF(BDMA, AHB4ENR, RCC_AHB4ENR_BDMAEN);
#endif
#endif

#ifdef ADC3
#ifdef RCC_AHB4ENR_ADC3EN
    RCC_ENABLE_IF(ADC3, AHB4ENR, RCC_AHB4ENR_ADC3EN);
#endif
#endif

#ifdef HSEM
#ifdef RCC_AHB4ENR_HSEMEN
    RCC_ENABLE_IF(HSEM, AHB4ENR, RCC_AHB4ENR_HSEMEN);
#endif
#endif

    /*
     * APB1 Low
     */

#ifdef TIM2
    RCC_ENABLE_IF(TIM2, APB1LENR, RCC_APB1LENR_TIM2EN);
#endif
#ifdef TIM3
    RCC_ENABLE_IF(TIM3, APB1LENR, RCC_APB1LENR_TIM3EN);
#endif
#ifdef TIM4
    RCC_ENABLE_IF(TIM4, APB1LENR, RCC_APB1LENR_TIM4EN);
#endif
#ifdef TIM5
    RCC_ENABLE_IF(TIM5, APB1LENR, RCC_APB1LENR_TIM5EN);
#endif
#ifdef TIM6
    RCC_ENABLE_IF(TIM6, APB1LENR, RCC_APB1LENR_TIM6EN);
#endif
#ifdef TIM7
    RCC_ENABLE_IF(TIM7, APB1LENR, RCC_APB1LENR_TIM7EN);
#endif
#ifdef TIM12
    RCC_ENABLE_IF(TIM12, APB1LENR, RCC_APB1LENR_TIM12EN);
#endif
#ifdef TIM13
    RCC_ENABLE_IF(TIM13, APB1LENR, RCC_APB1LENR_TIM13EN);
#endif
#ifdef TIM14
    RCC_ENABLE_IF(TIM14, APB1LENR, RCC_APB1LENR_TIM14EN);
#endif

#ifdef LPTIM1
    RCC_ENABLE_IF(LPTIM1, APB1LENR, RCC_APB1LENR_LPTIM1EN);
#endif

#ifdef SPI2
    RCC_ENABLE_IF(SPI2, APB1LENR, RCC_APB1LENR_SPI2EN);
#endif
#ifdef SPI3
    RCC_ENABLE_IF(SPI3, APB1LENR, RCC_APB1LENR_SPI3EN);
#endif

#ifdef SPDIFRX
#ifdef RCC_APB1LENR_SPDIFRXEN
    RCC_ENABLE_IF(SPDIFRX, APB1LENR, RCC_APB1LENR_SPDIFRXEN);
#endif
#endif

#ifdef USART2
    RCC_ENABLE_IF(USART2, APB1LENR, RCC_APB1LENR_USART2EN);
#endif
#ifdef USART3
    RCC_ENABLE_IF(USART3, APB1LENR, RCC_APB1LENR_USART3EN);
#endif
#ifdef UART4
    RCC_ENABLE_IF(UART4, APB1LENR, RCC_APB1LENR_UART4EN);
#endif
#ifdef UART5
    RCC_ENABLE_IF(UART5, APB1LENR, RCC_APB1LENR_UART5EN);
#endif

#ifdef I2C1
    RCC_ENABLE_IF(I2C1, APB1LENR, RCC_APB1LENR_I2C1EN);
#endif
#ifdef I2C2
    RCC_ENABLE_IF(I2C2, APB1LENR, RCC_APB1LENR_I2C2EN);
#endif
#ifdef I2C3
    RCC_ENABLE_IF(I2C3, APB1LENR, RCC_APB1LENR_I2C3EN);
#endif

#ifdef CEC
#ifdef RCC_APB1LENR_CECEN
    RCC_ENABLE_IF(CEC, APB1LENR, RCC_APB1LENR_CECEN);
#endif
#endif

#ifdef HDMI_CEC
#ifdef RCC_APB1LENR_CECEN
    RCC_ENABLE_IF(HDMI_CEC, APB1LENR, RCC_APB1LENR_CECEN);
#endif
#endif

#ifdef DAC1
#ifdef RCC_APB1LENR_DAC12EN
    RCC_ENABLE_IF(DAC1, APB1LENR, RCC_APB1LENR_DAC12EN);
#endif
#endif

#ifdef UART7
    RCC_ENABLE_IF(UART7, APB1LENR, RCC_APB1LENR_UART7EN);
#endif
#ifdef UART8
    RCC_ENABLE_IF(UART8, APB1LENR, RCC_APB1LENR_UART8EN);
#endif

    /*
     * APB1 High
     */

#ifdef CRS
#ifdef RCC_APB1HENR_CRSEN
    RCC_ENABLE_IF(CRS, APB1HENR, RCC_APB1HENR_CRSEN);
#endif
#endif

#ifdef SWPMI1
#ifdef RCC_APB1HENR_SWPMIEN
    RCC_ENABLE_IF(SWPMI1, APB1HENR, RCC_APB1HENR_SWPMIEN);
#endif
#endif

#ifdef OPAMP
#ifdef RCC_APB1HENR_OPAMPEN
    RCC_ENABLE_IF(OPAMP, APB1HENR, RCC_APB1HENR_OPAMPEN);
#endif
#endif

#ifdef MDIOS
#ifdef RCC_APB1HENR_MDIOSEN
    RCC_ENABLE_IF(MDIOS, APB1HENR, RCC_APB1HENR_MDIOSEN);
#endif
#endif

#ifdef FDCAN1
#ifdef RCC_APB1HENR_FDCANEN
    RCC_ENABLE_IF(FDCAN1, APB1HENR, RCC_APB1HENR_FDCANEN);
#endif
#endif

#ifdef FDCAN2
#ifdef RCC_APB1HENR_FDCANEN
    RCC_ENABLE_IF(FDCAN2, APB1HENR, RCC_APB1HENR_FDCANEN);
#endif
#endif

    /*
     * APB2
     */

#ifdef TIM1
    RCC_ENABLE_IF(TIM1, APB2ENR, RCC_APB2ENR_TIM1EN);
#endif
#ifdef TIM8
    RCC_ENABLE_IF(TIM8, APB2ENR, RCC_APB2ENR_TIM8EN);
#endif

#ifdef USART1
    RCC_ENABLE_IF(USART1, APB2ENR, RCC_APB2ENR_USART1EN);
#endif
#ifdef USART6
    RCC_ENABLE_IF(USART6, APB2ENR, RCC_APB2ENR_USART6EN);
#endif

#ifdef SPI1
    RCC_ENABLE_IF(SPI1, APB2ENR, RCC_APB2ENR_SPI1EN);
#endif
#ifdef SPI4
    RCC_ENABLE_IF(SPI4, APB2ENR, RCC_APB2ENR_SPI4EN);
#endif

#ifdef TIM15
    RCC_ENABLE_IF(TIM15, APB2ENR, RCC_APB2ENR_TIM15EN);
#endif
#ifdef TIM16
    RCC_ENABLE_IF(TIM16, APB2ENR, RCC_APB2ENR_TIM16EN);
#endif
#ifdef TIM17
    RCC_ENABLE_IF(TIM17, APB2ENR, RCC_APB2ENR_TIM17EN);
#endif

#ifdef SPI5
    RCC_ENABLE_IF(SPI5, APB2ENR, RCC_APB2ENR_SPI5EN);
#endif

#ifdef SAI1
#ifdef RCC_APB2ENR_SAI1EN
    RCC_ENABLE_IF(SAI1, APB2ENR, RCC_APB2ENR_SAI1EN);
#endif
#endif

#ifdef SAI2
#ifdef RCC_APB2ENR_SAI2EN
    RCC_ENABLE_IF(SAI2, APB2ENR, RCC_APB2ENR_SAI2EN);
#endif
#endif

#ifdef SAI3
#ifdef RCC_APB2ENR_SAI3EN
    RCC_ENABLE_IF(SAI3, APB2ENR, RCC_APB2ENR_SAI3EN);
#endif
#endif

#ifdef DFSDM1_Channel0
#ifdef RCC_APB2ENR_DFSDM1EN
    RCC_ENABLE_IF(DFSDM1_Channel0, APB2ENR, RCC_APB2ENR_DFSDM1EN);
#endif
#endif

#ifdef HRTIM1
#ifdef RCC_APB2ENR_HRTIMEN
    RCC_ENABLE_IF(HRTIM1, APB2ENR, RCC_APB2ENR_HRTIMEN);
#endif
#endif

    /*
     * APB3
     */

#ifdef LTDC
#ifdef RCC_APB3ENR_LTDCEN
    RCC_ENABLE_IF(LTDC, APB3ENR, RCC_APB3ENR_LTDCEN);
#endif
#endif

#ifdef DSI
#ifdef RCC_APB3ENR_DSIEN
    RCC_ENABLE_IF(DSI, APB3ENR, RCC_APB3ENR_DSIEN);
#endif
#endif

#ifdef WWDG1
#ifdef RCC_APB3ENR_WWDG1EN
    RCC_ENABLE_IF(WWDG1, APB3ENR, RCC_APB3ENR_WWDG1EN);
#endif
#endif

    /*
     * APB4
     */

#ifdef SYSCFG
#ifdef RCC_APB4ENR_SYSCFGEN
    RCC_ENABLE_IF(SYSCFG, APB4ENR, RCC_APB4ENR_SYSCFGEN);
#endif
#endif

#ifdef LPUART1
#ifdef RCC_APB4ENR_LPUART1EN
    RCC_ENABLE_IF(LPUART1, APB4ENR, RCC_APB4ENR_LPUART1EN);
#endif
#endif

#ifdef SPI6
#ifdef RCC_APB4ENR_SPI6EN
    RCC_ENABLE_IF(SPI6, APB4ENR, RCC_APB4ENR_SPI6EN);
#endif
#endif

#ifdef I2C4
#ifdef RCC_APB4ENR_I2C4EN
    RCC_ENABLE_IF(I2C4, APB4ENR, RCC_APB4ENR_I2C4EN);
#endif
#endif

#ifdef LPTIM2
#ifdef RCC_APB4ENR_LPTIM2EN
    RCC_ENABLE_IF(LPTIM2, APB4ENR, RCC_APB4ENR_LPTIM2EN);
#endif
#endif

#ifdef LPTIM3
#ifdef RCC_APB4ENR_LPTIM3EN
    RCC_ENABLE_IF(LPTIM3, APB4ENR, RCC_APB4ENR_LPTIM3EN);
#endif
#endif

#ifdef LPTIM4
#ifdef RCC_APB4ENR_LPTIM4EN
    RCC_ENABLE_IF(LPTIM4, APB4ENR, RCC_APB4ENR_LPTIM4EN);
#endif
#endif

#ifdef LPTIM5
#ifdef RCC_APB4ENR_LPTIM5EN
    RCC_ENABLE_IF(LPTIM5, APB4ENR, RCC_APB4ENR_LPTIM5EN);
#endif
#endif

#ifdef COMP1
#ifdef RCC_APB4ENR_COMP12EN
    RCC_ENABLE_IF(COMP1, APB4ENR, RCC_APB4ENR_COMP12EN);
#endif
#endif

#ifdef COMP2
#ifdef RCC_APB4ENR_COMP12EN
    RCC_ENABLE_IF(COMP2, APB4ENR, RCC_APB4ENR_COMP12EN);
#endif
#endif

#ifdef VREFBUF
#ifdef RCC_APB4ENR_VREFEN
    RCC_ENABLE_IF(VREFBUF, APB4ENR, RCC_APB4ENR_VREFEN);
#endif
#endif

#ifdef RTC
#ifdef RCC_APB4ENR_RTCAPBEN
    RCC_ENABLE_IF(RTC, APB4ENR, RCC_APB4ENR_RTCAPBEN);
#endif
#endif

#ifdef SAI4
#ifdef RCC_APB4ENR_SAI4EN
    RCC_ENABLE_IF(SAI4, APB4ENR, RCC_APB4ENR_SAI4EN);
#endif
#endif

#undef RCC_ENABLE_IF

    return -1;
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

