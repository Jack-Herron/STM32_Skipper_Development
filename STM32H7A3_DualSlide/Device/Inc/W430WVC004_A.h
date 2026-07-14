/*
 * W430WVC004_A.h
 *
 *  Created on: Jul 13, 2026
 *      Author: Jack Herron
 */

#ifndef INC_W430WVC004_A_H_
#define INC_W430WVC004_A_H_

#include <stdint.h>

#include "ST7701S.h"

#define W430WVC004_A_WIDTH     480U
#define W430WVC004_A_HEIGHT    800U

typedef enum
{
    W430WVC004_A_STATUS_OK = 0U,
    W430WVC004_A_STATUS_ERROR,
    W430WVC004_A_STATUS_TIMEOUT,
    W430WVC004_A_STATUS_INVALID_PARAMETER

} W430WVC004_A_StatusTypeDef;

typedef struct
{
    ST7701S_HandleTypeDef *Controller;
} W430WVC004_A_HandleTypeDef;

W430WVC004_A_StatusTypeDef W430WVC004_A_Init(
    W430WVC004_A_HandleTypeDef *Handle);

#endif /* INC_W430WVC004_A_H_ */
