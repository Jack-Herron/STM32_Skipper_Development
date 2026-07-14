/*
 * STM32H7A3_LTDC.h
 *
 *  Created on: May 22, 2026
 *      Author: Jack Herron
 */

#ifndef INC_STM32H7A3_LTDC_H_
#define INC_STM32H7A3_LTDC_H_

#include <STDINT.h>
#include "STM32H7xx.h"

typedef enum
{
    LTDC_POLARITY_ACTIVE_LOW = 0U,
    LTDC_POLARITY_ACTIVE_HIGH

} LTDC_PolarityTypeDef;

typedef enum
{
    LTDC_PIXEL_CLOCK_NORMAL = 0U,
    LTDC_PIXEL_CLOCK_INVERTED

} LTDC_PixelClockPolarityTypeDef;

typedef struct
{
	uint8_t Red;
	uint8_t Green;
	uint8_t Blue;
}LTDC_Color888;

typedef struct
{
	LTDC_TypeDef *Instance;

    uint16_t HorizontalSyncWidth;
    uint16_t HorizontalBackPorch;
    uint16_t ActiveWidth;
    uint16_t HorizontalFrontPorch;

    uint16_t VerticalSyncHeight;
    uint16_t VerticalBackPorch;
    uint16_t ActiveHeight;
    uint16_t VerticalFrontPorch;

    LTDC_PolarityTypeDef HorizontalSyncPolarity;
    LTDC_PolarityTypeDef VerticalSyncPolarity;
    LTDC_PolarityTypeDef DataEnablePolarity;
    LTDC_PixelClockPolarityTypeDef PixelClockPolarity;

    LTDC_Color888 BGColor;

} LTDC_HandleTypeDef;

typedef enum
{
    LTDC_LAYER_1 = 0U,
    LTDC_LAYER_2 = 1U

} LTDC_LayerIndexTypeDef;

typedef enum
{
    LTDC_PIXEL_FORMAT_ARGB8888 = 0U,
    LTDC_PIXEL_FORMAT_RGB888   = 1U,
    LTDC_PIXEL_FORMAT_RGB565   = 2U,
    LTDC_PIXEL_FORMAT_ARGB1555 = 3U,
    LTDC_PIXEL_FORMAT_ARGB4444 = 4U,
    LTDC_PIXEL_FORMAT_L8       = 5U,
    LTDC_PIXEL_FORMAT_AL44     = 6U,
    LTDC_PIXEL_FORMAT_AL88     = 7U

} LTDC_PixelFormatTypeDef;

typedef enum
{
    LTDC_STATUS_OK = 0U,
    LTDC_STATUS_ERROR,
    LTDC_STATUS_INVALID_PARAMETER,
    LTDC_STATUS_TIMEOUT,
    LTDC_STATUS_BUSY

} LTDC_StatusTypeDef;



typedef struct
{
    LTDC_LayerIndexTypeDef LayerIndex;

    uint32_t FramebufferAddress;

    uint16_t X;
    uint16_t Y;

    uint16_t Width;
    uint16_t Height;

    LTDC_PixelFormatTypeDef PixelFormat;

    uint8_t Alpha;

} LTDC_LayerHandleTypeDef;

typedef enum
{
    LTDC_RELOAD_IMMEDIATE = 0U,
    LTDC_RELOAD_VERTICAL_BLANKING

} LTDC_ReloadTypeDef;

LTDC_StatusTypeDef LTDC_Init(LTDC_HandleTypeDef *Handle);
LTDC_StatusTypeDef LTDC_LayerInit(LTDC_HandleTypeDef *Handle, LTDC_LayerHandleTypeDef *Layer);
LTDC_StatusTypeDef LTDC_LayerEnable(LTDC_HandleTypeDef *Handle, LTDC_LayerHandleTypeDef *Layer);
LTDC_StatusTypeDef LTDC_LayerDisable(LTDC_HandleTypeDef *Handle, LTDC_LayerHandleTypeDef *Layer);
LTDC_StatusTypeDef LTDC_Reload(LTDC_HandleTypeDef *Handle, LTDC_ReloadTypeDef Reload);
LTDC_StatusTypeDef LTDC_LayerMoveWindow(LTDC_HandleTypeDef *Handle,  LTDC_LayerHandleTypeDef *Layer, uint16_t X, uint16_t Y);

#endif /* INC_STM32H7A3_LTDC_H_ */
