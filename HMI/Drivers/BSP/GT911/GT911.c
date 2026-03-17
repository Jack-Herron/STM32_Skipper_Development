/*
 * GT911.c
 *
 *  Created on: Mar 17, 2026
 *      Author: Jack Herron
 */

#include "GT911.h"

TS_DrvTypeDef GT911_ts_drv =
{
	GT911_Init,
	GT911_ReadID,
	GT911_Reset,

	GT911_TS_Start,
	GT911_TS_DetectTouch,
	GT911_TS_GetXY,

	GT911_TS_EnableIT,
	GT911_TS_ClearIT,
	GT911_TS_ITStatus,
	GT911_TS_DisableIT
};

static GT911_handle_TypeDef GT911_handle = { GT911_I2C_NOT_INITIALIZED, 0, 0};

void GT911_Init(uint16_t DeviceAddr)
{
  TS_IO_Init();
}

uint16_t GT911_ReadID(uint16_t DeviceAddr)
{
	uint8_t id = TS_IO_Read(DeviceAddr, GT911_PRODUCT_ID_REG);
}

void GT911_TS_Start(uint16_t DeviceAddr)
{

}

uint8_t GT911_TS_DetectTouch(uint16_t DeviceAddr)
{

}

void GT911_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y)
{

}

void GT911_TS_EnableIT(uint16_t DeviceAddr)
{

}

void GT911_Reset(uint16_t DeviceAddr)
{

}

void GT911_TS_DisableIT(uint16_t DeviceAddr)
{

}

uint8_t GT911_TS_ITStatus (uint16_t DeviceAddr)
{

}

void GT911_TS_ClearIT (uint16_t DeviceAddr)
{

}
