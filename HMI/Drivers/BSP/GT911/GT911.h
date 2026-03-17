/*
 * GT911.h
 *
 *  Created on: Mar 17, 2026
 *      Author: Jack Herron
 */

#ifndef BSP_GT911_GT911_H_
#define BSP_GT911_GT911_H_

#include <stdint.h>
#include "TS.h"

#define  GT911_MAX_WIDTH              ((uint16_t)800)     /* Touchscreen pad max width   */
#define  GT911_MAX_HEIGHT             ((uint16_t)480)     /* Touchscreen pad max height  */

#define GT911_STATUS_OK                0
#define GT911_STATUS_NOT_OK            1

#define GT911_I2C_NOT_INITIALIZED      0
#define GT911_I2C_INITIALIZED          1

#define GT911_PRODUCT_ID_REG			0x8140
#define GT911_FIRMWARE_VER_REG			0x8144
#define GT911_CONFIG_VER_REG			0x8047
#define GT911_CONFIG_FRESH_REG			0x8100
#define GT911_READ_XY_REG				0x814E
#define GT911_POINT1_REG				0x814F
#define GT911_POINT2_REG				0x8157
#define GT911_POINT3_REG				0x815F
#define GT911_POINT4_REG				0x8167
#define GT911_POINT5_REG				0x816F
#define GT911_STATUS_REG				GT911_READ_XY_REG


typedef struct
{
  uint8_t i2cInitialized;

  /* field holding the current number of simultaneous active touches */
  uint8_t currActiveTouchNb;

  /* field holding the touch index currently managed */
  uint8_t currActiveTouchIdx;

} GT911_handle_TypeDef;


void GT911_Init(uint16_t DeviceAddr);

uint16_t GT911_ReadID(uint16_t DeviceAddr);

void GT911_TS_Start(uint16_t DeviceAddr);

uint8_t GT911_TS_DetectTouch(uint16_t DeviceAddr);

void GT911_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y);

void GT911_TS_EnableIT(uint16_t DeviceAddr);

void GT911_Reset(uint16_t DeviceAddr);

void GT911_TS_DisableIT(uint16_t DeviceAddr);

uint8_t GT911_TS_ITStatus (uint16_t DeviceAddr);

void GT911_TS_ClearIT (uint16_t DeviceAddr);

extern TS_DrvTypeDef GT911_ts_drv;

extern void     TS_IO_Init(void);
extern void     TS_IO_Write(uint8_t Addr, uint16_t Reg, uint8_t Value);
extern uint8_t  TS_IO_Read(uint8_t Addr, uint16_t Reg);
extern uint16_t TS_IO_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length);
extern void     TS_IO_Delay(uint32_t Delay);

#endif /* BSP_GT911_GT911_H_ */
