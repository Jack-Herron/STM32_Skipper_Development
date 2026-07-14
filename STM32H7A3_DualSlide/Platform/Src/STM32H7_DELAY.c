/*
 * STM32H7A3_DELAY.c
 *
 *  Created on: Jul 13, 2026
 *      Author: Jack Herron
 */

#include "STM32H7_DELAY.h"
#include "STM32H7xx.h"

static uint32_t Delay_CoreClockHz = 0U;
static uint32_t Delay_CyclesPerMicrosecond = 0U;

DELAY_StatusTypeDef DELAY_Init(uint32_t CoreClockHz)
{
    if(CoreClockHz < 1000000U)
    {
        return DELAY_STATUS_INVALID_PARAMETER;
    }

    Delay_CoreClockHz = CoreClockHz;
    Delay_CyclesPerMicrosecond = CoreClockHz / 1000000U;

    /*
     * Enable access to the Cortex-M debug and trace blocks.
     */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /*
     * Some Cortex-M7 implementations require the DWT lock
     * access register to be unlocked before CYCCNT is enabled.
     */
#if defined(DWT_LAR)
    DWT->LAR = 0xC5ACCE55U;
#endif

    /*
     * Reset and enable the cycle counter.
     */
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    /*
     * Confirm that the counter is operating.
     */
    if((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0U)
    {
        Delay_CoreClockHz = 0U;
        Delay_CyclesPerMicrosecond = 0U;

        return DELAY_STATUS_ERROR;
    }

    return DELAY_STATUS_OK;
}

void DELAY_us(uint32_t Microseconds)
{
    uint32_t StartCycles;
    uint32_t RequiredCycles;

    if((Microseconds == 0U) ||
       (Delay_CyclesPerMicrosecond == 0U))
    {
        return;
    }

    /*
     * Keep each wait below the 32-bit CYCCNT wrap period.
     *
     * At 280 MHz, the counter wraps approximately every
     * 15.3 seconds.
     */
    while(Microseconds > 1000000U)
    {
        DELAY_us(1000000U);
        Microseconds -= 1000000U;
    }

    RequiredCycles =
        Microseconds * Delay_CyclesPerMicrosecond;

    StartCycles = DWT->CYCCNT;

    while((uint32_t)(DWT->CYCCNT - StartCycles) <
          RequiredCycles)
    {
        __NOP();
    }
}

void DELAY_ms(uint32_t Milliseconds)
{
    while(Milliseconds > 0U)
    {
        DELAY_us(1000U);
        Milliseconds--;
    }
}
