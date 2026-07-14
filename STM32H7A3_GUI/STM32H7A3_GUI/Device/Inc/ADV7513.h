/*
 * ADV7513.h
 *
 *  Created on: May 22, 2026
 *      Author: Jack Herron
 */

#ifndef INC_ADV7513_H_
#define INC_ADV7513_H_

#include "STM32H7A3_I2C.h"
#include <stdint.h>

#define ADV7513_DEVICE_ID                         0x7511U

#define ADV7513_REG_POWER                         0x41U
#define ADV7513_REG_STATUS                        0x42U
#define ADV7513_REG_PACKET_MEMORY_ADDRESS         0x45U
#define ADV7513_REG_PACKET_READ_MODE              0x44U
#define ADV7513_REG_AVI_PACKET_ENABLE             0x40U
#define ADV7513_REG_PACKET_UPDATE                 0x4AU
#define ADV7513_REG_INPUT_ID                      0x15U
#define ADV7513_REG_INPUT_STYLE                   0x16U
#define ADV7513_REG_ASPECT_RATIO                  0x17U
#define ADV7513_REG_CSC_CONTROL                   0x18U
#define ADV7513_REG_HDMI_DVI_MODE                 0xAFU
#define ADV7513_REG_INTERRUPT_ENABLE_0            0x94U
#define ADV7513_REG_INTERRUPT_ENABLE_1            0x95U
#define ADV7513_REG_INTERRUPT_STATUS_0            0x96U
#define ADV7513_REG_INTERRUPT_STATUS_1            0x97U
#define ADV7513_REG_PLL_STATUS                    0x9EU
#define ADV7513_REG_TMDS_POWER                    0xA1U
#define ADV7513_REG_FIXED_I2C_ADDRESS             0xF9U
#define ADV7513_REG_DEVICE_ID_HIGH                0xF5U
#define ADV7513_REG_DEVICE_ID_LOW                 0xF6U

#define ADV7513_REG_FIXED_98                      0x98U
#define ADV7513_REG_FIXED_9A                      0x9AU
#define ADV7513_REG_FIXED_9C                      0x9CU
#define ADV7513_REG_FIXED_9D                      0x9DU
#define ADV7513_REG_FIXED_A2                      0xA2U
#define ADV7513_REG_FIXED_A3                      0xA3U
#define ADV7513_REG_FIXED_E0                      0xE0U

#define ADV7513_POWER_UP                          0x10U

#define ADV7513_FIXED_98_VALUE                    0x03U
#define ADV7513_FIXED_9A_VALUE                    0xE0U
#define ADV7513_FIXED_9C_VALUE                    0x30U
#define ADV7513_FIXED_9D_VALUE                    0x61U
#define ADV7513_FIXED_A2_VALUE                    0xA4U
#define ADV7513_FIXED_A3_VALUE                    0xA4U
#define ADV7513_FIXED_E0_VALUE                    0xD0U
#define ADV7513_FIXED_F9_VALUE                    0x00U

#define ADV7513_INPUT_ID_RGB_YCBCR_444_SEPARATE_SYNC 0x00U

typedef enum
{
    ADV7513_STATUS_OK = 0U,
    ADV7513_STATUS_ERROR,
    ADV7513_STATUS_I2C_ERROR,
    ADV7513_STATUS_INVALID_DEVICE,
    ADV7513_STATUS_INVALID_PARAMETER

} ADV7513_StatusTypeDef;

typedef enum
{
    ADV7513_OUTPUT_MODE_DVI = 0U,
    ADV7513_OUTPUT_MODE_HDMI

} ADV7513_OutputModeTypeDef;

typedef struct
{
    I2C_DeviceHandleTypeDef *I2C;

    ADV7513_OutputModeTypeDef OutputMode;

    uint8_t Revision;
    uint16_t DeviceID;

} ADV7513_HandleTypeDef;

ADV7513_StatusTypeDef ADV7513_Init(ADV7513_HandleTypeDef *Handle);
ADV7513_StatusTypeDef ADV7513_Configure(ADV7513_HandleTypeDef *Handle);
ADV7513_StatusTypeDef ADV7513_ReadRegister(ADV7513_HandleTypeDef *Handle, uint8_t Register, uint8_t *Data);
ADV7513_StatusTypeDef ADV7513_WriteRegister(ADV7513_HandleTypeDef *Handle, uint8_t Register, uint8_t Data);
ADV7513_StatusTypeDef ADV7513_ReadDeviceID(ADV7513_HandleTypeDef *Handle, uint16_t *DeviceID);


#endif /* INC_ADV7513_H_ */
