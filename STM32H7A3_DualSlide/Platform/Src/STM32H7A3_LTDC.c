/*
 * STM32H7A3_LTDC.c
 *
 *  Created on: May 22, 2026
 *      Author: Jack Herron
 */
#include "STM32H7A3_LTDC.h"
#include "STM32H7xx.h"
#include "STM32H7A3_RCC.h"
#include "STDLIB.h"

#define LTDC_BLENDING_FACTOR1_CA    0x06U
#define LTDC_BLENDING_FACTOR2_CA    0x07U

LTDC_StatusTypeDef LTDC_Init(LTDC_HandleTypeDef *Handle)
{
	RCC_EnablePeripheralClock(Handle->Instance);

	(void)RCC->APB3ENR;

    uint32_t HorizontalSyncWidth;
    uint32_t AccumulatedHorizontalBackPorch;
    uint32_t AccumulatedActiveWidth;
    uint32_t TotalWidth;

    uint32_t VerticalSyncHeight;
    uint32_t AccumulatedVerticalBackPorch;
    uint32_t AccumulatedActiveHeight;
    uint32_t TotalHeight;

    uint32_t GlobalControlRegister;

    if((Handle == NULL) || (Handle->Instance == NULL))
    {
        return LTDC_STATUS_ERROR;
    }

    HorizontalSyncWidth =
        (uint32_t)Handle->HorizontalSyncWidth - 1U;

    AccumulatedHorizontalBackPorch =
        (uint32_t)Handle->HorizontalSyncWidth +
        (uint32_t)Handle->HorizontalBackPorch - 1U;

    AccumulatedActiveWidth =
        (uint32_t)Handle->HorizontalSyncWidth +
        (uint32_t)Handle->HorizontalBackPorch +
        (uint32_t)Handle->ActiveWidth - 1U;

    TotalWidth =
        (uint32_t)Handle->HorizontalSyncWidth +
        (uint32_t)Handle->HorizontalBackPorch +
        (uint32_t)Handle->ActiveWidth +
        (uint32_t)Handle->HorizontalFrontPorch - 1U;

    VerticalSyncHeight =
        (uint32_t)Handle->VerticalSyncHeight - 1U;

    AccumulatedVerticalBackPorch =
        (uint32_t)Handle->VerticalSyncHeight +
        (uint32_t)Handle->VerticalBackPorch - 1U;

    AccumulatedActiveHeight =
        (uint32_t)Handle->VerticalSyncHeight +
        (uint32_t)Handle->VerticalBackPorch +
        (uint32_t)Handle->ActiveHeight - 1U;

    TotalHeight =
        (uint32_t)Handle->VerticalSyncHeight +
        (uint32_t)Handle->VerticalBackPorch +
        (uint32_t)Handle->ActiveHeight +
        (uint32_t)Handle->VerticalFrontPorch - 1U;

    Handle->Instance->SSCR =
        ((HorizontalSyncWidth & 0x0FFFU) << LTDC_SSCR_HSW_Pos) |
        ((VerticalSyncHeight & 0x07FFU) << LTDC_SSCR_VSH_Pos);

    Handle->Instance->BPCR =
        ((AccumulatedHorizontalBackPorch & 0x0FFFU) << LTDC_BPCR_AHBP_Pos) |
        ((AccumulatedVerticalBackPorch   & 0x07FFU) << LTDC_BPCR_AVBP_Pos);

    Handle->Instance->AWCR =
        ((AccumulatedActiveWidth  & 0x0FFFU) << LTDC_AWCR_AAW_Pos) |
        ((AccumulatedActiveHeight & 0x07FFU) << LTDC_AWCR_AAH_Pos);

    Handle->Instance->TWCR =
        ((TotalWidth  & 0x0FFFU) << LTDC_TWCR_TOTALW_Pos) |
        ((TotalHeight & 0x07FFU) << LTDC_TWCR_TOTALH_Pos);

    GlobalControlRegister = 0U;

    if(Handle->HorizontalSyncPolarity == LTDC_POLARITY_ACTIVE_HIGH)
    {
        GlobalControlRegister |= LTDC_GCR_HSPOL;
    }

    if(Handle->VerticalSyncPolarity == LTDC_POLARITY_ACTIVE_HIGH)
    {
        GlobalControlRegister |= LTDC_GCR_VSPOL;
    }

    if(Handle->DataEnablePolarity == LTDC_POLARITY_ACTIVE_HIGH)
    {
        GlobalControlRegister |= LTDC_GCR_DEPOL;
    }

    if(Handle->PixelClockPolarity == LTDC_PIXEL_CLOCK_INVERTED)
    {
        GlobalControlRegister |= LTDC_GCR_PCPOL;
    }

    Handle->Instance->BCCR =
        ((uint32_t)Handle->BGColor.Red 		<< LTDC_BCCR_BCRED_Pos) |
        ((uint32_t)Handle->BGColor.Green   	<< LTDC_BCCR_BCGREEN_Pos) |
        ((uint32_t)Handle->BGColor.Blue   	<< LTDC_BCCR_BCBLUE_Pos);

    Handle->Instance->GCR = GlobalControlRegister;

    Handle->Instance->SRCR = LTDC_SRCR_IMR;

    Handle->Instance->GCR |= LTDC_GCR_LTDCEN;

    return LTDC_STATUS_OK;
}

static uint32_t LTDC_GetPixelFormatBytesPerPixel(LTDC_PixelFormatTypeDef PixelFormat)
{
    uint32_t BytesPerPixel;

    switch(PixelFormat)
    {
        case LTDC_PIXEL_FORMAT_ARGB8888:
            BytesPerPixel = 4U;
            break;

        case LTDC_PIXEL_FORMAT_RGB888:
            BytesPerPixel = 3U;
            break;

        case LTDC_PIXEL_FORMAT_RGB565:
        case LTDC_PIXEL_FORMAT_ARGB1555:
        case LTDC_PIXEL_FORMAT_ARGB4444:
        case LTDC_PIXEL_FORMAT_AL88:
            BytesPerPixel = 2U;
            break;

        case LTDC_PIXEL_FORMAT_L8:
        case LTDC_PIXEL_FORMAT_AL44:
            BytesPerPixel = 1U;
            break;

        default:
            BytesPerPixel = 0U;
            break;
    }

    return BytesPerPixel;
}

LTDC_StatusTypeDef LTDC_LayerInit(LTDC_HandleTypeDef *Handle, LTDC_LayerHandleTypeDef *Layer)
{
    LTDC_Layer_TypeDef *LayerRegisters;

    uint32_t BytesPerPixel;
    uint32_t LineLengthBytes;
    uint32_t WindowStartX;
    uint32_t WindowStopX;
    uint32_t WindowStartY;
    uint32_t WindowStopY;

    if((Handle == NULL) || (Handle->Instance == NULL) || (Layer == NULL))
    {
        return LTDC_STATUS_ERROR;
    }

    if(Layer->LayerIndex == LTDC_LAYER_1)
    {
        LayerRegisters = LTDC_Layer1;
    }
    else if(Layer->LayerIndex == LTDC_LAYER_2)
    {
        LayerRegisters = LTDC_Layer2;
    }
    else
    {
        return LTDC_STATUS_INVALID_PARAMETER;
    }

    BytesPerPixel = LTDC_GetPixelFormatBytesPerPixel(Layer->PixelFormat);

    if(BytesPerPixel == 0U)
    {
        return LTDC_STATUS_INVALID_PARAMETER;
    }

    WindowStartX = (uint32_t)Handle->HorizontalSyncWidth + (uint32_t)Handle->HorizontalBackPorch + (uint32_t)Layer->X;
    WindowStopX  = WindowStartX + (uint32_t)Layer->Width - 1U;

    WindowStartY = (uint32_t)Handle->VerticalSyncHeight + (uint32_t)Handle->VerticalBackPorch + (uint32_t)Layer->Y;
    WindowStopY  = WindowStartY + (uint32_t)Layer->Height - 1U;

    LineLengthBytes = (uint32_t)Layer->Width * BytesPerPixel;

    LayerRegisters->WHPCR =
        ((WindowStopX  & 0x0FFFU) << LTDC_LxWHPCR_WHSPPOS_Pos) |
        ((WindowStartX & 0x0FFFU) << LTDC_LxWHPCR_WHSTPOS_Pos);

    LayerRegisters->WVPCR =
        ((WindowStopY  & 0x07FFU) << LTDC_LxWVPCR_WVSPPOS_Pos) |
        ((WindowStartY & 0x07FFU) << LTDC_LxWVPCR_WVSTPOS_Pos);

    LayerRegisters->PFCR = (uint32_t)Layer->PixelFormat;

    LayerRegisters->CACR = (uint32_t)Layer->Alpha;

    LayerRegisters->DCCR = 0x00000000U;

    LayerRegisters->BFCR =
        (LTDC_BLENDING_FACTOR1_CA << LTDC_LxBFCR_BF1_Pos) |
        (LTDC_BLENDING_FACTOR2_CA << LTDC_LxBFCR_BF2_Pos);

    LayerRegisters->CFBAR = Layer->FramebufferAddress;

    LayerRegisters->CFBLR =
        (((LineLengthBytes + 3U) & 0x1FFFU) << LTDC_LxCFBLR_CFBLL_Pos) |
        ((LineLengthBytes        & 0x1FFFU) << LTDC_LxCFBLR_CFBP_Pos);

    LayerRegisters->CFBLNR = (uint32_t)Layer->Height;

    return LTDC_STATUS_OK;
}

LTDC_StatusTypeDef LTDC_LayerEnable(LTDC_HandleTypeDef *Handle, LTDC_LayerHandleTypeDef *Layer)
{
    LTDC_Layer_TypeDef *LayerRegisters;

    if((Handle == NULL) || (Handle->Instance == NULL) || (Layer == NULL))
    {
        return LTDC_STATUS_ERROR;
    }

    if(Layer->LayerIndex == LTDC_LAYER_1)
    {
        LayerRegisters = LTDC_Layer1;
    }
    else if(Layer->LayerIndex == LTDC_LAYER_2)
    {
        LayerRegisters = LTDC_Layer2;
    }
    else
    {
        return LTDC_STATUS_INVALID_PARAMETER;
    }

    LayerRegisters->CR |= LTDC_LxCR_LEN;

    return LTDC_STATUS_OK;
}

LTDC_StatusTypeDef LTDC_LayerDisable(LTDC_HandleTypeDef *Handle, LTDC_LayerHandleTypeDef *Layer)
{
    LTDC_Layer_TypeDef *LayerRegisters;

    if((Handle == NULL) || (Handle->Instance == NULL) || (Layer == NULL))
    {
        return LTDC_STATUS_ERROR;
    }

    if(Layer->LayerIndex == LTDC_LAYER_1)
    {
        LayerRegisters = LTDC_Layer1;
    }
    else if(Layer->LayerIndex == LTDC_LAYER_2)
    {
        LayerRegisters = LTDC_Layer2;
    }
    else
    {
        return LTDC_STATUS_INVALID_PARAMETER;
    }

    LayerRegisters->CR &= ~LTDC_LxCR_LEN;

    return LTDC_STATUS_OK;
}

LTDC_StatusTypeDef LTDC_Reload(LTDC_HandleTypeDef *Handle, LTDC_ReloadTypeDef Reload)
{
    if((Handle == NULL) || (Handle->Instance == NULL))
    {
        return LTDC_STATUS_ERROR;
    }

    if(Reload == LTDC_RELOAD_IMMEDIATE)
    {
        Handle->Instance->SRCR = LTDC_SRCR_IMR;
    }
    else if(Reload == LTDC_RELOAD_VERTICAL_BLANKING)
    {
        Handle->Instance->SRCR = LTDC_SRCR_VBR;
    }
    else
    {
        return LTDC_STATUS_INVALID_PARAMETER;
    }

    return LTDC_STATUS_OK;
}

LTDC_StatusTypeDef LTDC_LayerMoveWindow(LTDC_HandleTypeDef *Handle,  LTDC_LayerHandleTypeDef *Layer, uint16_t X, uint16_t Y)
{
    LTDC_Layer_TypeDef *LayerRegisters;

    uint32_t WindowStartX;
    uint32_t WindowStopX;
    uint32_t WindowStartY;
    uint32_t WindowStopY;

    if((Handle == NULL) || (Handle->Instance == NULL) || (Layer == NULL))
    {
        return LTDC_STATUS_ERROR;
    }

    if((X + Layer->Width) > Handle->ActiveWidth)
    {
        return LTDC_STATUS_INVALID_PARAMETER;
    }

    if((Y + Layer->Height) > Handle->ActiveHeight)
    {
        return LTDC_STATUS_INVALID_PARAMETER;
    }

    if(Layer->LayerIndex == LTDC_LAYER_1)
    {
        LayerRegisters = LTDC_Layer1;
    }
    else if(Layer->LayerIndex == LTDC_LAYER_2)
    {
        LayerRegisters = LTDC_Layer2;
    }
    else
    {
        return LTDC_STATUS_INVALID_PARAMETER;
    }

    Layer->X = X;
    Layer->Y = Y;

    WindowStartX = (uint32_t)Handle->HorizontalSyncWidth + (uint32_t)Handle->HorizontalBackPorch + (uint32_t)Layer->X;
    WindowStopX  = WindowStartX + (uint32_t)Layer->Width - 1U;

    WindowStartY = (uint32_t)Handle->VerticalSyncHeight + (uint32_t)Handle->VerticalBackPorch + (uint32_t)Layer->Y;
    WindowStopY  = WindowStartY + (uint32_t)Layer->Height - 1U;

    LayerRegisters->WHPCR =
        ((WindowStopX  & 0x0FFFU) << LTDC_LxWHPCR_WHSPPOS_Pos) |
        ((WindowStartX & 0x0FFFU) << LTDC_LxWHPCR_WHSTPOS_Pos);

    LayerRegisters->WVPCR =
        ((WindowStopY  & 0x07FFU) << LTDC_LxWVPCR_WVSPPOS_Pos) |
        ((WindowStartY & 0x07FFU) << LTDC_LxWVPCR_WVSTPOS_Pos);

    return LTDC_STATUS_OK;
}
