/*
 * Board.c
 *
 *  Created on: May 16, 2026
 *      Author: Jack Herron
 */

#include <STDIO.h>
#include <STDLIB.h>
#include "Board.h"
#include "STM32H7xx.h"
#include "STM32H7A3_RCC.h"
#include "STM32H7_GPIO.h"
#include "STM32H7A3_SPI.h"
#include "ST7701S.h"
#include "STM32H7_DELAY.h"
#include "W430WVC004_A.h"
#include "STM32H7A3_LTDC.h"
#include "IMG.h"
#include "STM32H7A3_DAC.h"
#include "STM32H7A3_DMA.h"
#include "Audio.h"
#include "Board_AudioTest.h"

#define LCD_WIDTH              480U
#define LCD_HEIGHT             800U

#define LCD_HSYNC_WIDTH        10U
#define LCD_HORIZONTAL_BP      20U
#define LCD_HORIZONTAL_FP      40U

#define LCD_VSYNC_HEIGHT       2U
#define LCD_VERTICAL_BP        18U
#define LCD_VERTICAL_FP        20U

SPI_BusHandleTypeDef SPI2_Bus =
{
    .Instance = SPI2,
    .Timeout  = 1000U
};

SPI_DeviceHandleTypeDef LCD_SPI =
{
    .Bus                  = &SPI2_Bus,

    .ChipSelectPort       = GPIOA,
    .ChipSelectPin        = (1U << 10),
    .ChipSelectPolarity   = SPI_CHIP_SELECT_ACTIVE_LOW,

    .MaxClockFrequency    = 1000000U,
    .Mode                 = SPI_MODE_0,
    .BitOrder             = SPI_BIT_ORDER_MSB_FIRST,
    .FrameSize            = SPI_FRAME_SIZE_9_BIT,
    .Direction            = SPI_DIRECTION_TX_ONLY
};

ST7701S_HandleTypeDef ST7701S_LCD_Controller =
{
    .SPI 					= &LCD_SPI,
    .ResetPort 				= GPIOB,
    .ResetPin 				= (1U << 12),
    .ResetActiveLow 		= 1,
	.Delay_ms		 		= DELAY_ms
};

W430WVC004_A_HandleTypeDef LCD =
{
    .Controller = &ST7701S_LCD_Controller
};

LTDC_HandleTypeDef LCD_LTDC =
{
    .Instance = LTDC,

    .HorizontalSyncWidth  	= LCD_HSYNC_WIDTH,
    .HorizontalBackPorch  	= LCD_HORIZONTAL_BP,
    .ActiveWidth          	= LCD_WIDTH,
    .HorizontalFrontPorch 	= LCD_HORIZONTAL_FP,

    .VerticalSyncHeight   	= LCD_VSYNC_HEIGHT,
    .VerticalBackPorch    	= LCD_VERTICAL_BP,
    .ActiveHeight         	= LCD_HEIGHT,
    .VerticalFrontPorch   	= LCD_VERTICAL_FP,

    .HorizontalSyncPolarity = LTDC_POLARITY_ACTIVE_LOW,
    .VerticalSyncPolarity   = LTDC_POLARITY_ACTIVE_LOW,
    .DataEnablePolarity     = LTDC_POLARITY_ACTIVE_LOW,

    /*
     * Start with normal polarity. Change to INVERTED if the picture
     * is unstable or sampled incorrectly.
     */
    .PixelClockPolarity = LTDC_PIXEL_CLOCK_NORMAL,

    .BGColor =
    {
        .Red   = 255U,
        .Green = 255U,
        .Blue  = 255U
    }
};

LTDC_LayerHandleTypeDef LCD_Layer =
{
    .LayerIndex = LTDC_LAYER_1,

    .FramebufferAddress = (uint32_t)LCD_Framebuffer,

    .X = 0U,
    .Y = 0U,

    .Width  = LCD_WIDTH,
    .Height = LCD_HEIGHT,

    .PixelFormat = LTDC_PIXEL_FORMAT_RGB565,

    .Alpha = 255U
};

DAC_HandleTypeDef DAC_Audio =
{
    .Instance      = DAC1,
    .Channel       = DAC_CHANNEL_1,
    .Alignment     = DAC_ALIGNMENT_12BIT_RIGHT,
    .Trigger       = DAC_TRIGGER_TIM6_TRGO,
    .OutputBuffer  = DAC_OUTPUT_BUFFER_ENABLED
};

DMA_HandleTypeDef DMA_Audio =
{
    .Instance = DMA1_Stream1,
	.InterruptPriority = 5U,
	.Config =
    {
        .Request = DMA_REQUEST_DAC1_CH1,
    }
};

Audio_HandleTypeDef Audio =
{
    .DAC = &DAC_Audio,
    .DMA = &DMA_Audio,

    .TimerInstance = TIM6,
    .TimerKernelClockHz = 200000000UL,
    .SampleRateHz = 48000UL
};

void Board_GPIOInit(void)
{
    /*
     * LTDC RGB interface.
     *
     * Medium speed, push-pull, no pull resistors.
     */

    const GPIO_Config_t ltdc_af14 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 14U,
        .initial_output_state = 0U
    };

    const GPIO_Config_t ltdc_af13 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 13U,
        .initial_output_state = 0U
    };

    const GPIO_Config_t ltdc_af11 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 11U,
        .initial_output_state = 0U
    };

    const GPIO_Config_t ltdc_af10 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 10U,
        .initial_output_state = 0U
    };

    const GPIO_Config_t ltdc_af9 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 9U,
        .initial_output_state = 0U
    };

    GPIO_Init(GPIOC, 0U,  &ltdc_af14);  /* LTDC_R5 */
    GPIO_Init(GPIOC, 1U,  &ltdc_af14);  /* LTDC_G5 */
    GPIO_Init(GPIOA, 5U,  &ltdc_af14);  /* LTDC_R4 */
    GPIO_Init(GPIOA, 6U,  &ltdc_af14);  /* LTDC_G2 */
    GPIO_Init(GPIOA, 7U,  &ltdc_af14);  /* LTDC_VSYNC */
    GPIO_Init(GPIOC, 4U,  &ltdc_af14);  /* LTDC_R7 */
    GPIO_Init(GPIOC, 5U,  &ltdc_af14);  /* LTDC_DE */
    GPIO_Init(GPIOB, 10U, &ltdc_af14);  /* LTDC_G4 */
    GPIO_Init(GPIOB, 14U, &ltdc_af14);  /* LTDC_CLK */
    GPIO_Init(GPIOB, 15U, &ltdc_af14);  /* LTDC_G7 */
    GPIO_Init(GPIOC, 6U,  &ltdc_af14);  /* LTDC_HSYNC */
    GPIO_Init(GPIOC, 7U,  &ltdc_af14);  /* LTDC_G6 */
    GPIO_Init(GPIOC, 10U, &ltdc_af14);  /* LTDC_R2 */
    GPIO_Init(GPIOC, 11U, &ltdc_af14);  /* LTDC_B4 */
    GPIO_Init(GPIOD, 2U,  &ltdc_af14);  /* LTDC_B2 */
    GPIO_Init(GPIOB, 8U,  &ltdc_af14);  /* LTDC_B6 */
    GPIO_Init(GPIOB, 9U,  &ltdc_af14);  /* LTDC_B7 */

    GPIO_Init(GPIOA, 8U,  &ltdc_af13);  /* LTDC_B3 */

    GPIO_Init(GPIOB, 5U,  &ltdc_af11);  /* LTDC_B5 */

    GPIO_Init(GPIOC, 9U,  &ltdc_af10);  /* LTDC_G3 */

    GPIO_Init(GPIOB, 0U,  &ltdc_af9);   /* LTDC_R3 */
    GPIO_Init(GPIOB, 1U,  &ltdc_af9);   /* LTDC_R6 */


    /*
     * SPI2 LCD interface.
     *
     * PB13 = SPI2_SCK
     * PC3  = SPI2_MOSI
     */

    const GPIO_Config_t spi2_af5 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 5U,
        .initial_output_state = 0U
    };

    GPIO_Init(GPIOB, 13U, &spi2_af5);   /* SPI2_SCK */
    GPIO_Init(GPIOC, 3U,  &spi2_af5);   /* SPI2_MOSI */


    /*
     * Timer outputs.
     */

    const GPIO_Config_t timer_af2 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 2U,
        .initial_output_state = 0U
    };

    //GPIO_Init(GPIOB, 4U, &timer_af2);   /* TIM3_CH1 / LCD_BL*/
    //GPIO_Init(GPIOB, 6U, &timer_af2);   /* TIM4_CH1 / RED_LED */
    //GPIO_Init(GPIOB, 7U, &timer_af2);   /* TIM4_CH2 / AMBER_LED */


    /*
     * Analog inputs and DAC output.
     *
     * Analog mode disables the digital input and output circuitry.
     */

    const GPIO_Config_t analog =
    {
        .mode                 = GPIO_MODE_ANALOG,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_LOW,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 0U,
        .initial_output_state = 0U
    };

    GPIO_Init(GPIOA, 0U, &analog);      /* ADC1_INP16 / POT_A */
    GPIO_Init(GPIOA, 1U, &analog);      /* ADC1_INP17 / POT_B */
    GPIO_Init(GPIOA, 2U, &analog);      /* ADC1_INP14 / USB_CC1 */
    GPIO_Init(GPIOA, 3U, &analog);      /* ADC1_INP15 / USB_CC2 */
    GPIO_Init(GPIOA, 4U, &analog);      /* DAC1_OUT1 */
    GPIO_Init(GPIOC, 2U, &analog);      /* ADC1_INP12 / BAT_VSENSE */


    /*
     * USB OTG HS peripheral using the internal full-speed PHY.
     *
     * PA11 and PA12 are controlled by the USB PHY and do not need normal
     * GPIO alternate-function configuration.
     *
     * PA9 is used as the VBUS sensing input.
     */

    const GPIO_Config_t usb_vbus_input =
    {
        .mode                 = GPIO_MODE_INPUT,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 0U,
        .initial_output_state = 0U
    };

    GPIO_Init(GPIOA, 9U, &usb_vbus_input); /* USB_OTG_HS_VBUS */


    /*
     * Button and status inputs.
     *
     * BAT_CHRG is expected to be an active-low, open-drain charger output.
     * USER_BTN and POWER_BTN are expected to connect to ground when pressed.
     */

    const GPIO_Config_t input_pullup =
    {
        .mode                 = GPIO_MODE_INPUT,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_LOW,
        .pull                 = GPIO_PULL_UP,
        .alternate_function   = 0U,
        .initial_output_state = 0U
    };

    GPIO_Init(GPIOC, 14U, &input_pullup); 	/* BAT_CHRG */
    GPIO_Init(GPIOB, 2U, &input_pullup); 	/* USER_BTN / EXTI2 */
    GPIO_Init(GPIOC, 12U, &input_pullup); 	/* POWER_BTN / EXTI12 */


    /*
     * Board control outputs.
     */

    const GPIO_Config_t output_low =
    {
        .mode                 = GPIO_MODE_OUTPUT,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_LOW,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 0U,
        .initial_output_state = 0U
    };

    const GPIO_Config_t output_high =
    {
        .mode                 = GPIO_MODE_OUTPUT,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_LOW,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 0U,
        .initial_output_state = 1U
    };

    GPIO_Init(GPIOC, 15U, &output_low);  /* BAT_ISET */
    GPIO_Init(GPIOB, 12U, &output_low);  /* LCD_RESET */

    GPIO_Init(GPIOC, 13U, &output_high); /* PWR_EN */
    GPIO_Init(GPIOA, 10U, &output_high); /* LCD_CS */
    GPIO_Init(GPIOB, 6U,  &output_high); /* RED_LED */
    GPIO_Init(GPIOB, 7U,  &output_high); /* ORANGE_LED */
    GPIO_Init(GPIOB, 4U,  &output_high); /* BL */
    /*
     * SWD/JTAG debug interface.
     *
     * These pins normally default to their debug functions after reset.
     * Explicit configuration keeps their intended use documented.
     */

    const GPIO_Config_t debug_af0 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_HIGH,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 0U,
        .initial_output_state = 0U
    };

    GPIO_Init(GPIOA, 13U, &debug_af0);  /* JTMS / SWDIO */
    GPIO_Init(GPIOA, 14U, &debug_af0);  /* JTCK / SWCLK */
    GPIO_Init(GPIOA, 15U, &debug_af0);  /* JTDI */
    GPIO_Init(GPIOB, 3U,  &debug_af0);  /* JTDO / SWO */
}

void Board_Init()
{
	RCC_Init();
	DELAY_Init(280000000UL);
	Board_GPIOInit();
	SPI_Init(&SPI2_Bus);
	Audio_Init(&Audio);

	W430WVC004_A_StatusTypeDef PanelStatus;
	LTDC_StatusTypeDef LTDCStatus;

	PanelStatus = W430WVC004_A_Init(&LCD);

	if(PanelStatus != W430WVC004_A_STATUS_OK) while(1);

	LTDCStatus = LTDC_Init(&LCD_LTDC);

	if(LTDCStatus != LTDC_STATUS_OK) while(1);

	LTDCStatus = LTDC_LayerInit(&LCD_LTDC, &LCD_Layer);

	if(LTDCStatus != LTDC_STATUS_OK) while(1);

	LTDCStatus = LTDC_LayerEnable(&LCD_LTDC, &LCD_Layer);

	if(LTDCStatus != LTDC_STATUS_OK) while(1);

	LTDCStatus = LTDC_Reload(&LCD_LTDC, LTDC_RELOAD_IMMEDIATE);

	if(LTDCStatus != LTDC_STATUS_OK) while(1);


	if(Audio_Init(&Audio) != AUDIO_STATUS_OK)  while(1);

	if(Board_AudioTestStart(&Audio) != AUDIO_STATUS_OK) while(1);

	uint8_t i = 0;
	while(1)
	{

		DELAY_ms(5);

		if(GPIO_Read(GPIOC, 14U) == 0)
		{
			GPIO_Write(GPIOB, 7U, 1);
		}
		else
		{
			GPIO_Write(GPIOB, 7U, 0);
		}

		if(GPIO_Read(GPIOC, 12) == 1)
		{
			for(uint32_t i = 0; i<0x4fffff; i++)
			{
				if(GPIO_Read(GPIOC, 12) == 0)
				{
					break;
				}
			}
			if(GPIO_Read(GPIOC, 12) == 1)
			{
				GPIO_Write(GPIOC, 13U, 0);
				GPIO_Write(GPIOB,4,0);
				GPIO_Write(GPIOB,6,0);
				while(1);
			}
		}

		GPIO_Write(GPIOB,6,i%2);
		i++;
		Board_AudioTestUpdate();
	}
}
