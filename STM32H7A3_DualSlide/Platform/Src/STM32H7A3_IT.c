/*
 * STM32H7A3_IT.c
 *
 *  Created on: Jul 16, 2026
 *      Author: Jack Herron
 */

/*
 * STM32H7A3_IT.c
 */

#include "STM32H7xx.h"

// peripheral function includes
#include "STM32h7A3_DMA.h"

static void NoInterrupt_Handler(void)
{
    for(;;)
    {
        __BKPT(0);
    }
}

void NMI_Handler(void)
{
    NoInterrupt_Handler();
}

void HardFault_Handler(void)
{
    NoInterrupt_Handler();
}

void MemManage_Handler(void)
{
    NoInterrupt_Handler();
}

void BusFault_Handler(void)
{
    NoInterrupt_Handler();
}

void UsageFault_Handler(void)
{
    NoInterrupt_Handler();
}

void SVC_Handler(void)
{
    NoInterrupt_Handler();
}

void DebugMon_Handler(void)
{
    NoInterrupt_Handler();
}

void PendSV_Handler(void)
{
    NoInterrupt_Handler();
}

void SysTick_Handler(void)
{
    NoInterrupt_Handler();
}

void WWDG_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void PVD_PVM_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void RTC_TAMP_STAMP_CSS_LSE_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void RTC_WKUP_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void FLASH_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void RCC_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void EXTI0_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void EXTI1_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void EXTI2_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void EXTI3_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void EXTI4_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DMA_STR0_IRQHandler(void)
{
	DMA_IRQHandler(DMA1, 0);
}

void DMA_STR1_IRQHandler(void)
{
	DMA_IRQHandler(DMA1, 1);
}

void DMA_STR2_IRQHandler(void)
{
	DMA_IRQHandler(DMA1, 2);
}

void DMA_STR3_IRQHandler(void)
{
	DMA_IRQHandler(DMA1, 3);
}

void DMA_STR4_IRQHandler(void)
{
	DMA_IRQHandler(DMA1, 4);
}

void DMA_STR5_IRQHandler(void)
{
	DMA_IRQHandler(DMA1, 5);
}

void DMA_STR6_IRQHandler(void)
{
	DMA_IRQHandler(DMA1, 6);
}

void ADC1_2_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void FDCAN1_IT0_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void FDCAN2_IT0_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void FDCAN1_IT1_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void FDCAN2_IT1_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void EXTI9_5_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM1_BRK_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM1_UP_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM1_TRG_COM_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM1_CC_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM2_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM3_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM4_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void I2C1_EV_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void I2C1_ER_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void I2C2_EV_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void I2C2_ER_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void SPI1_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void SPI2_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void USART1_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void USART2_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void USART3_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void EXTI15_10_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void RTC_ALARM_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM8_BRK_TIM12_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM8_UP_TIM13_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM8_CC_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DMA1_STR7_IRQHandler(void)
{
	DMA_IRQHandler(DMA1, 7);
}

void FMC_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void SDMMC1_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM5_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void SPI3_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void UART4_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void UART5_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DMA2_STR0_IRQHandler(void)
{
	DMA_IRQHandler(DMA2, 0);
}

void DMA2_STR1_IRQHandler(void)
{
	DMA_IRQHandler(DMA2, 1);
}

void DMA2_STR2_IRQHandler(void)
{
	DMA_IRQHandler(DMA2, 2);
}

void DMA2_STR3_IRQHandler(void)
{
	DMA_IRQHandler(DMA2, 3);
}

void DMA2_STR4_IRQHandler(void)
{
	DMA_IRQHandler(DMA2, 4);
}

void ETH_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void ETH_WKUP_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DMA2_STR5_IRQHandler(void)
{
	DMA_IRQHandler(DMA2, 5);
}

void DMA2_STR6_IRQHandler(void)
{
	DMA_IRQHandler(DMA2, 6);
}

void DMA2_STR7_IRQHandler(void)
{
	DMA_IRQHandler(DMA2, 7);
}

void USART6_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void I2C3_EV_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void I2C3_ER_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void OTG_HS_EP1_OUT_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void OTG_HS_EP1_IN_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void OTG_HS_WKUP_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void OTG_HS_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DCMI_PSSI_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void CRYP_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void UART7_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void UART8_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void SPI4_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void SPI5_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void SPI6_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void LTDC_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void LTDC_ER_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DMA2D_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void SAI2_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void QUADSPI_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void LPTIM1_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void CEC_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void I2C4_EV_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void I2C4_ER_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DMAMUX1_OV_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DFSDM1_FLT0_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DFSDM1_FLT1_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DFSDM1_FLT2_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DFSDM1_FLT3_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void SWPMI1_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM15_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM16_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void TIM17_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void MDIOS_WKUP_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void MDIOS_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void JPEG_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void MDMA_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void SDMMC_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void HSEM0_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DAC2_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void DMAMUX2_OVR_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void BDMA2_CH0_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void BDMA2_CH1_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void BDMA2_CH2_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void BDMA2_CH3_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void BDMA2_CH4_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void BDMA2_CH5_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void BDMA_CH6_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void BDMA1_CH7_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void COMP_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void LPTIM2_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void LPTIM3_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void LPUART_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void WWDG1_RST_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void CRS_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void WKUP_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void OCTOSPI2_IRQHandler(void)
{
    NoInterrupt_Handler();
}

void BDMA1_IRQHandler(void)
{
    NoInterrupt_Handler();
}
