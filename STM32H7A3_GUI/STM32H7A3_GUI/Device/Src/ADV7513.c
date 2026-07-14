/*
 * ADV7513.c
 */

#include "ADV7513.h"
#include <stddef.h>

#define ADV7513_WRITE_CHECK(Register, Data)                                      \
    do                                                                           \
    {                                                                            \
        if(ADV7513_WriteRegister(Handle, (Register), (Data)) != ADV7513_STATUS_OK) \
        {                                                                        \
            return ADV7513_STATUS_I2C_ERROR;                                     \
        }                                                                        \
    } while(0)

ADV7513_StatusTypeDef ADV7513_ReadRegister(ADV7513_HandleTypeDef *Handle, uint8_t Register, uint8_t *Data)
{
    if((Handle == NULL) || (Handle->I2C == NULL) || (Data == NULL))
    {
        return ADV7513_STATUS_INVALID_PARAMETER;
    }

    if(I2C_DeviceMemRead8(Handle->I2C, Register, Data) != I2C_STATUS_OK)
    {
        return ADV7513_STATUS_I2C_ERROR;
    }

    return ADV7513_STATUS_OK;
}

ADV7513_StatusTypeDef ADV7513_WriteRegister(ADV7513_HandleTypeDef *Handle, uint8_t Register, uint8_t Data)
{
    if((Handle == NULL) || (Handle->I2C == NULL))
    {
        return ADV7513_STATUS_INVALID_PARAMETER;
    }

    if(I2C_DeviceMemWrite8(Handle->I2C, Register, Data) != I2C_STATUS_OK)
    {
        return ADV7513_STATUS_I2C_ERROR;
    }

    return ADV7513_STATUS_OK;
}

ADV7513_StatusTypeDef ADV7513_ReadDeviceID(ADV7513_HandleTypeDef *Handle, uint16_t *DeviceID)
{
    uint8_t DeviceIDHigh;
    uint8_t DeviceIDLow;

    if((Handle == NULL) || (DeviceID == NULL))
    {
        return ADV7513_STATUS_INVALID_PARAMETER;
    }

    if(ADV7513_ReadRegister(Handle, ADV7513_REG_DEVICE_ID_HIGH, &DeviceIDHigh) != ADV7513_STATUS_OK)
    {
        return ADV7513_STATUS_I2C_ERROR;
    }

    if(ADV7513_ReadRegister(Handle, ADV7513_REG_DEVICE_ID_LOW, &DeviceIDLow) != ADV7513_STATUS_OK)
    {
        return ADV7513_STATUS_I2C_ERROR;
    }

    *DeviceID = ((uint16_t)DeviceIDHigh << 8U) | (uint16_t)DeviceIDLow;

    return ADV7513_STATUS_OK;
}

ADV7513_StatusTypeDef ADV7513_Configure(ADV7513_HandleTypeDef *Handle)
{
    if((Handle == NULL) || (Handle->I2C == NULL))
    {
        return ADV7513_STATUS_INVALID_PARAMETER;
    }

    ADV7513_WRITE_CHECK(0x41U, 0x10U); /* Power up */

    ADV7513_WRITE_CHECK(0x98U, 0x03U); /* Fixed */
    ADV7513_WRITE_CHECK(0x9AU, 0xE0U); /* Fixed */
    ADV7513_WRITE_CHECK(0x9CU, 0x30U); /* Fixed */
    ADV7513_WRITE_CHECK(0x9DU, 0x61U); /* Fixed */
    ADV7513_WRITE_CHECK(0xA2U, 0xA4U); /* Fixed */
    ADV7513_WRITE_CHECK(0xA3U, 0xA4U); /* Fixed */
    ADV7513_WRITE_CHECK(0xE0U, 0xD0U); /* Fixed */
    ADV7513_WRITE_CHECK(0xF9U, 0x00U); /* Fixed I2C address */

    ADV7513_WRITE_CHECK(0x49U, 0xA8U); /* Input config */
    ADV7513_WRITE_CHECK(0x4CU, 0x00U); /* CSC disabled */

    ADV7513_WRITE_CHECK(0x15U, 0x00U); /* RGB 4:4:4 input */
    ADV7513_WRITE_CHECK(0x16U, 0x30U); /* 8-bit, style config */
    ADV7513_WRITE_CHECK(0x17U, 0x02U); /* Aspect/sync config */
    ADV7513_WRITE_CHECK(0x18U, 0x00U); /* Embedded sync disabled */

    ADV7513_WRITE_CHECK(0xAFU, 0x06U); /* HDMI mode */
    ADV7513_WRITE_CHECK(0xBAU, 0x60U); /* Clock config */

    ADV7513_WRITE_CHECK(0x94U, 0x00U); /* Mask interrupts */
    ADV7513_WRITE_CHECK(0x95U, 0x00U); /* Mask interrupts */
    ADV7513_WRITE_CHECK(0x96U, 0xFFU); /* Clear interrupts */
    ADV7513_WRITE_CHECK(0x97U, 0xFFU); /* Clear interrupts */

    return ADV7513_STATUS_OK;
}

//ADV7513_StatusTypeDef ADV7513_Configure(ADV7513_HandleTypeDef *Handle)
//{
//    if((Handle == NULL) || (Handle->I2C == NULL))
//    {
//        return ADV7513_STATUS_INVALID_PARAMETER;
//    }
//
//    ADV7513_WRITE_CHECK(ADV7513_REG_POWER, ADV7513_POWER_UP); 									/* Power up transmitter */
//
//    ADV7513_WRITE_CHECK(ADV7513_REG_FIXED_98, ADV7513_FIXED_98_VALUE); 							/* Fixed register, must be set after power-up */
//    ADV7513_WRITE_CHECK(ADV7513_REG_FIXED_9A, ADV7513_FIXED_9A_VALUE); 							/* Fixed register, must be set after power-up */
//    ADV7513_WRITE_CHECK(ADV7513_REG_FIXED_9C, ADV7513_FIXED_9C_VALUE); 							/* Fixed register, must be set after power-up */
//    ADV7513_WRITE_CHECK(ADV7513_REG_FIXED_9D, ADV7513_FIXED_9D_VALUE); 							/* Fixed register, must be set after power-up */
//    ADV7513_WRITE_CHECK(ADV7513_REG_FIXED_A2, ADV7513_FIXED_A2_VALUE); 							/* Fixed register, must be set after power-up */
//    ADV7513_WRITE_CHECK(ADV7513_REG_FIXED_A3, ADV7513_FIXED_A3_VALUE); 							/* Fixed register, must be set after power-up */
//    ADV7513_WRITE_CHECK(ADV7513_REG_FIXED_E0, ADV7513_FIXED_E0_VALUE); 							/* Fixed register, must be set after power-up */
//    ADV7513_WRITE_CHECK(ADV7513_REG_FIXED_I2C_ADDRESS, ADV7513_FIXED_F9_VALUE); 				/* Fixed I2C address setting */
//
//    ADV7513_WRITE_CHECK(0x49U, 0xA8U);						 									/* Input video format configuration */
//    ADV7513_WRITE_CHECK(0x4CU, 0x00U); 															/* Disable color space conversion */
//
//    ADV7513_WRITE_CHECK(ADV7513_REG_INPUT_ID, ADV7513_INPUT_ID_RGB_YCBCR_444_SEPARATE_SYNC); 	/* RGB/YCBCR 4:4:4 with separate syncs */
//    ADV7513_WRITE_CHECK(ADV7513_REG_INPUT_STYLE, 0x30U); 										/* 8-bit input, style 1/separate sync configuration */
//    ADV7513_WRITE_CHECK(ADV7513_REG_ASPECT_RATIO, 0x02U); 										/* Input aspect/sync configuration */
//    ADV7513_WRITE_CHECK(ADV7513_REG_CSC_CONTROL, 0x00U); 										/* Disable CSC */
//
//    if(Handle->OutputMode == ADV7513_OUTPUT_MODE_HDMI)
//    {
//        ADV7513_WRITE_CHECK(ADV7513_REG_HDMI_DVI_MODE, 0x06U); 									/* HDMI output mode */
//    }
//    else
//    {
//        ADV7513_WRITE_CHECK(ADV7513_REG_HDMI_DVI_MODE, 0x04U); 									/* DVI output mode */
//    }
//
//    ADV7513_WRITE_CHECK(0xBAU, 0x60U); 															/* Clock delay / polarity configuration */
//
//    ADV7513_WRITE_CHECK(ADV7513_REG_INTERRUPT_ENABLE_0, 0x00U); 								/* Disable HPD / monitor sense interrupts */
//    ADV7513_WRITE_CHECK(ADV7513_REG_INTERRUPT_ENABLE_1, 0x00U);									/* Disable DDC / controller interrupts */
//    ADV7513_WRITE_CHECK(ADV7513_REG_INTERRUPT_STATUS_0, 0xFFU); 								/* Clear pending interrupts */
//    ADV7513_WRITE_CHECK(ADV7513_REG_INTERRUPT_STATUS_1, 0xFFU); 								/* Clear pending interrupts */
//
//    return ADV7513_STATUS_OK;
//}

ADV7513_StatusTypeDef ADV7513_Init(ADV7513_HandleTypeDef *Handle)
{
    uint16_t DeviceID;

    if((Handle == NULL) || (Handle->I2C == NULL))
    {
        return ADV7513_STATUS_INVALID_PARAMETER;
    }

    if(ADV7513_ReadDeviceID(Handle, &DeviceID) != ADV7513_STATUS_OK)
    {
        return ADV7513_STATUS_I2C_ERROR;
    }

    Handle->DeviceID = DeviceID;

    if(DeviceID != ADV7513_DEVICE_ID)
    {
        return ADV7513_STATUS_INVALID_DEVICE;
    }

    return ADV7513_Configure(Handle);
}
