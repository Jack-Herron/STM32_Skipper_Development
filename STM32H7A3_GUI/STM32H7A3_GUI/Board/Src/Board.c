/*
 * Board.c
 *
 *  Created on: May 16, 2026
 *      Author: Jack Herron
 */

#include "Board.h"
#include "STM32H7xx.h"
#include "STM32H7A3_RCC.h"
#include "STM32H7_GPIO.h"
#include "STM32H7A3_TIM.h"
#include "STM32H7A3_LTDC.h"
#include "STM32H7A3_I2C.h"
#include "ADV7513.h"

uint16_t Framebuffer[480U * 800U] = {0};
uint16_t BoxBuffer[32U * 32U];

static I2C_BusHandleTypeDef I2C4_BusHandle =
{
    .Instance = I2C4,
    .BusFreq = 100000U,
    .Timeout = 100000U
};

static I2C_DeviceHandleTypeDef ADV7513_I2CHandle =
{
    .Bus = &I2C4_BusHandle,
    .Address = 0x39U,
    .AddressingMode = I2C_ADDRESSING_MODE_7BIT
};

static ADV7513_HandleTypeDef ADV7513_Handle =
{
    .I2C = &ADV7513_I2CHandle,

    .OutputMode = ADV7513_OUTPUT_MODE_HDMI
};

static LTDC_LayerHandleTypeDef MainLayer =
{
    .LayerIndex = LTDC_LAYER_1,

    .FramebufferAddress = (uint32_t)Framebuffer,

    .X = 0U,
    .Y = 0U,

    .Width = 480U,
    .Height = 800U,

    .PixelFormat = LTDC_PIXEL_FORMAT_RGB565,

    .Alpha = 255U
};

static LTDC_LayerHandleTypeDef OverlayLayer =
{
    .LayerIndex = LTDC_LAYER_2,

    .FramebufferAddress = (uint32_t)BoxBuffer,

    .X = 0U,
    .Y = 0U,

    .Width = 32U,
    .Height = 32U,

    .PixelFormat = LTDC_PIXEL_FORMAT_RGB565,

    .Alpha = 255U
};

static LTDC_HandleTypeDef HDMI_LCDConfig =
{
    .Instance = LTDC,

    .HorizontalSyncWidth 	= 120U,
    .HorizontalBackPorch 	= 120U,
    .ActiveWidth 			= 480U,
    .HorizontalFrontPorch 	= 48U,

    .VerticalSyncHeight 	= 80U,
    .VerticalBackPorch 		= 80U,
    .ActiveHeight 			= 800U,
    .VerticalFrontPorch 	= 8U,

    .HorizontalSyncPolarity	= LTDC_POLARITY_ACTIVE_HIGH,
    .VerticalSyncPolarity 	= LTDC_POLARITY_ACTIVE_HIGH,
    .DataEnablePolarity 	= LTDC_POLARITY_ACTIVE_LOW,
    .PixelClockPolarity 	= LTDC_PIXEL_CLOCK_NORMAL,

	.BGColor = { .Red = 255U, .Green = 255U, .Blue = 255U }
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

    const GPIO_Config_t ltdc_af9 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 9U,
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

    GPIO_Init(GPIOE, 4U,  &ltdc_af14);  /* LTDC_B0 */
    GPIO_Init(GPIOE, 5U,  &ltdc_af14);  /* LTDC_G0 */
    GPIO_Init(GPIOE, 6U,  &ltdc_af14);  /* LTDC_G1 */
    GPIO_Init(GPIOC, 0U,  &ltdc_af14);  /* LTDC_R5 */
    GPIO_Init(GPIOC, 1U,  &ltdc_af14);  /* LTDC_G5 */
    GPIO_Init(GPIOA, 1U,  &ltdc_af14);  /* LTDC_R2 */
    GPIO_Init(GPIOA, 2U,  &ltdc_af14);  /* LTDC_R1 */
    GPIO_Init(GPIOA, 3U,  &ltdc_af14);  /* LTDC_B5 */
    GPIO_Init(GPIOA, 4U,  &ltdc_af14);  /* LTDC_VSYNC */
    GPIO_Init(GPIOA, 5U,  &ltdc_af14);  /* LTDC_R4 */
    GPIO_Init(GPIOA, 6U,  &ltdc_af14);  /* LTDC_G2 */
    GPIO_Init(GPIOC, 4U,  &ltdc_af14);  /* LTDC_R7 */
    GPIO_Init(GPIOC, 5U,  &ltdc_af14);  /* LTDC_DE */
    GPIO_Init(GPIOE, 11U, &ltdc_af14);  /* LTDC_G3 */
    GPIO_Init(GPIOE, 12U, &ltdc_af14);  /* LTDC_B4 */
    GPIO_Init(GPIOE, 14U, &ltdc_af14);  /* LTDC_CLK */
    GPIO_Init(GPIOB, 10U, &ltdc_af14);  /* LTDC_G4 */
    GPIO_Init(GPIOB, 15U, &ltdc_af14);  /* LTDC_G7 */
    GPIO_Init(GPIOD, 10U, &ltdc_af14);  /* LTDC_B3 */
    GPIO_Init(GPIOC, 6U,  &ltdc_af14);  /* LTDC_HSYNC */
    GPIO_Init(GPIOC, 7U,  &ltdc_af14);  /* LTDC_G6 */
    GPIO_Init(GPIOC, 9U,  &ltdc_af14);  /* LTDC_B2 */
    GPIO_Init(GPIOG, 13U, &ltdc_af14);  /* LTDC_R0 */
    GPIO_Init(GPIOB, 8U,  &ltdc_af14);  /* LTDC_B6 */

    GPIO_Init(GPIOB, 1U,  &ltdc_af9);   /* LTDC_R6 */
    GPIO_Init(GPIOA, 15U, &ltdc_af9);   /* LTDC_R3 */
    GPIO_Init(GPIOD, 2U,  &ltdc_af9);   /* LTDC_B7 */

    GPIO_Init(GPIOC, 10U, &ltdc_af10);  /* LTDC_B1 */


    /*
     * I2C4.
     *
     * Open-drain with pull-ups.
     */

    const GPIO_Config_t i2c4_af4 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_OD,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_UP,
        .alternate_function   = 4U,
        .initial_output_state = 1U
    };

    GPIO_Init(GPIOF, 14U, &i2c4_af4);   /* I2C4_SCL */
    GPIO_Init(GPIOF, 15U, &i2c4_af4);   /* I2C4_SDA */


    /*
     * USART3 connected to ST-LINK VCP.
     */

    const GPIO_Config_t usart3_af7 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_UP,
        .alternate_function   = 7U,
        .initial_output_state = 1U
    };

    GPIO_Init(GPIOD, 8U, &usart3_af7);  /* USART3_TX / STLINK_RX */
    GPIO_Init(GPIOD, 9U, &usart3_af7);  /* USART3_RX / STLINK_TX */


    /*
     * USB OTG FS.
     *
     * PA11 = USB_DM and PA12 = USB_DP are controlled by the USB FS PHY.
     * They are not configured here as normal GPIO alternate-function pins.
     *
     * PA10 = USB_ID is configured only if OTG ID detection is used.
     * VBUS sensing is configured in the USB peripheral, not as a normal GPIO AF.
     */

    const GPIO_Config_t usb_id_af10 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 10U,
        .initial_output_state = 0U
    };

    GPIO_Init(GPIOA, 10U, &usb_id_af10); /* USB_OTG_HS_ID */


    /*
     * HDMI-CEC.
     */

    const GPIO_Config_t cec_af5 =
    {
        .mode                 = GPIO_MODE_AF,
        .output_type          = GPIO_OTYPE_OD,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_UP,
        .alternate_function   = 5U,
        .initial_output_state = 1U
    };

    GPIO_Init(GPIOB, 6U, &cec_af5);     /* HDMI-CEC */


    /*
     * User button and interrupt inputs.
     */

    const GPIO_Config_t input_pullup =
    {
        .mode                 = GPIO_MODE_INPUT,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_UP,
        .alternate_function   = 0U,
        .initial_output_state = 0U
    };

    GPIO_Init(GPIOC, 13U, &input_pullup); /* Blue pushbutton */
    GPIO_Init(GPIOG, 6U,  &input_pullup); /* HDMI adapter interrupt */
    GPIO_Init(GPIOG, 7U,  &input_pullup); /* USB power fault input */


    /*
     * Board outputs.
     */

    const GPIO_Config_t output_low =
    {
        .mode                 = GPIO_MODE_OUTPUT,
        .output_type          = GPIO_OTYPE_PP,
        .speed                = GPIO_SPEED_MEDIUM,
        .pull                 = GPIO_PULL_NONE,
        .alternate_function   = 0U,
        .initial_output_state = 0U
    };

    GPIO_Init(GPIOB, 0U,  &output_low); /* LD1 green LED */
    GPIO_Init(GPIOB, 14U, &output_low); /* LD3 red LED */
    GPIO_Init(GPIOE, 1U,  &output_low); /* LD2 yellow LED */
    GPIO_Init(GPIOF, 10U, &output_low); /* USB_FS_PWR_EN */
}

int32_t BoxX = 0;
int32_t BoxY = 0;

int32_t BoxDX = 2;
int32_t BoxDY = 2;

void Board_Init()
{
	RCC_Init();
	Board_GPIOInit();

	/*for(uint32_t i = 0; i < 480*800; i++)
	{
		Framebuffer[i] = 0xFFFF;
	}*/

	for(uint32_t i = 0U; i < (32U * 32U); i++)
	{
	    BoxBuffer[i] = 0xF800U;
	}

	LTDC_Init(&HDMI_LCDConfig);
	LTDC_LayerInit(&HDMI_LCDConfig, &MainLayer);
	LTDC_LayerInit(&HDMI_LCDConfig, &OverlayLayer);
	LTDC_LayerEnable(&HDMI_LCDConfig, &MainLayer);
	LTDC_LayerEnable(&HDMI_LCDConfig, &OverlayLayer);
	LTDC_Reload(&HDMI_LCDConfig, LTDC_RELOAD_IMMEDIATE);
	I2C_Init(&I2C4_BusHandle);
	for(uint32_t i = 0; i < 10000000; i++);
	ADV7513_Init(&ADV7513_Handle);

	while(1)
	{
	    LTDC_LayerMoveWindow(&HDMI_LCDConfig, &OverlayLayer, BoxX, BoxY);
	    LTDC_Reload(&HDMI_LCDConfig, LTDC_RELOAD_VERTICAL_BLANKING);

	    BoxX += BoxDX;
	    BoxY += BoxDY;

	    if((BoxX <= 0) || (BoxX >= (480 - 32)))
	    {
	        BoxDX = -BoxDX;
	    }

	    if((BoxY <= 0) || (BoxY >= (800 - 32)))
	    {
	        BoxDY = -BoxDY;
	    }

	    for(volatile uint32_t Delay = 0U; Delay < 100000U; Delay++)
	    {
	    }
	}
}
