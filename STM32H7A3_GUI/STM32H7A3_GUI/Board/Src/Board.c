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

TIM_Handle_t red_LED_TIM =
{
    .instance = TIM12,
	.mode = TIM_MODE_PWM1,
	.frequency = 1000U,
	.enable_update_interrupt = 0U,
	.channel = 1U,
	.active_low = 1U,
	.duty_permille = 250U,
};

void Board_GPIOInit(void)
{
    /*
     * LTDC RGB interface.
     *
     * Medium speed for now, push-pull, no pull resistors.
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
     * Board-controlled outputs.
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
    //GPIO_Init(GPIOB, 14U, &output_low); /* LD3 red LED */
    GPIO_Init(GPIOE, 1U,  &output_low); /* LD2 yellow LED */
    GPIO_Init(GPIOF, 10U, &output_low); /* USB_FS_PWR_EN */

    // temp \/\/\/\/\/

    GPIO_Config_t tim12_ch1 =
    {
        .mode = GPIO_MODE_AF,
        .output_type = GPIO_OTYPE_PP,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_PULL_NONE,

        .alternate_function = 2u,

        .initial_output_state = 0u
    };

    GPIO_Init(GPIOB, 14u, &tim12_ch1);
}

void Board_Init()
{
	RCC_Init();
	Board_GPIOInit();
	TIM_Init(&red_LED_TIM);
	TIM_Start(&red_LED_TIM);
	while(1)
	{
		for(uint32_t i = 0; i < 1000; i++)
		{
			for(uint32_t j = 0; j < 10000; j++);
			TIM_SetPWMDutyPermille(&red_LED_TIM, i);
		}
	}
}
