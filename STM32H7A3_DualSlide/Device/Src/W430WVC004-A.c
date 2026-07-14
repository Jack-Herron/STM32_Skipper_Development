/*
 * W430WVC004_A.c
 *
 *  Created on: Jul 13, 2026
 *      Author: Jack Herron
 */

#include "W430WVC004_A.h"

#include <stddef.h>

static W430WVC004_A_StatusTypeDef W430WVC004_A_ConvertStatus(
    ST7701S_StatusTypeDef Status)
{
    switch(Status)
    {
        case ST7701S_STATUS_OK:
            return W430WVC004_A_STATUS_OK;

        case ST7701S_STATUS_TIMEOUT:
            return W430WVC004_A_STATUS_TIMEOUT;

        case ST7701S_STATUS_INVALID_PARAMETER:
            return W430WVC004_A_STATUS_INVALID_PARAMETER;

        default:
            return W430WVC004_A_STATUS_ERROR;
    }
}

static W430WVC004_A_StatusTypeDef W430WVC004_A_WRITE_COMMANDand(
    W430WVC004_A_HandleTypeDef *Handle,
    uint8_t Command)
{
    return W430WVC004_A_ConvertStatus(
        ST7701S_WriteCommand(
            Handle->Controller,
            Command));
}

static W430WVC004_A_StatusTypeDef W430WVC004_A_WRITE_DATA(
    W430WVC004_A_HandleTypeDef *Handle,
    uint8_t Data)
{
    return W430WVC004_A_ConvertStatus(
        ST7701S_WriteData(
            Handle->Controller,
            Data));
}

W430WVC004_A_StatusTypeDef W430WVC004_A_Init(
    W430WVC004_A_HandleTypeDef *Handle)
{
    W430WVC004_A_StatusTypeDef Status;
    ST7701S_StatusTypeDef ControllerStatus;

#define WRITE_COMMAND(Value)                         \
    do                                               \
    {                                                \
        Status = W430WVC004_A_WRITE_COMMANDand(          \
            Handle,                                  \
            (uint8_t)(Value));                       \
                                                     \
        if(Status != W430WVC004_A_STATUS_OK)         \
        {                                            \
            return Status;                           \
        }                                            \
    } while(0)

#define WRITE_DATA(Value)                            \
    do                                               \
    {                                                \
        Status = W430WVC004_A_WRITE_DATA(             \
            Handle,                                  \
            (uint8_t)(Value));                       \
                                                     \
        if(Status != W430WVC004_A_STATUS_OK)         \
        {                                            \
            return Status;                           \
        }                                            \
    } while(0)

    if((Handle == NULL) ||
       (Handle->Controller == NULL))
    {
        return W430WVC004_A_STATUS_INVALID_PARAMETER;
    }

    ControllerStatus =
        ST7701S_HardwareReset(
            Handle->Controller);

    if(ControllerStatus != ST7701S_STATUS_OK)
    {
        return W430WVC004_A_ConvertStatus(
            ControllerStatus);
    }

    WRITE_COMMAND (0xFF);
    WRITE_DATA (0x77);
    WRITE_DATA (0x01);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x13);
    WRITE_COMMAND (0xEF);
    WRITE_DATA (0x08);
    WRITE_COMMAND (0xFF);
    WRITE_DATA (0x77);
    WRITE_DATA (0x01);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x10);
    WRITE_COMMAND (0xC0);
    WRITE_DATA (0x63);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0xC1);
    WRITE_DATA (0x0A);
    WRITE_DATA (0x0C);
    WRITE_COMMAND (0xC2);
    WRITE_DATA (0x01);
    WRITE_DATA (0x08);
    WRITE_COMMAND (0xC7);
    WRITE_DATA (0x04);
    WRITE_COMMAND (0xCC);
    WRITE_DATA (0x18);
    WRITE_COMMAND (0xB0);
    WRITE_DATA (0x00);
    WRITE_DATA (0x0A);
    WRITE_DATA (0x10);
    WRITE_DATA (0x0F);
    WRITE_DATA (0x11);
    WRITE_DATA (0x06);
    WRITE_DATA (0x01);
    WRITE_DATA (0x09);
    WRITE_DATA (0x09);
    WRITE_DATA (0x1E);
    WRITE_DATA (0x06);
    WRITE_DATA (0x13);
    WRITE_DATA (0x11);
    WRITE_DATA (0x24);
    WRITE_DATA (0x2B);
    WRITE_DATA (0x1F);
    WRITE_COMMAND (0xB1);
    WRITE_DATA (0x0C);
    WRITE_DATA (0x13);
    WRITE_DATA (0x18);
    WRITE_DATA (0x0A);
    WRITE_DATA (0x0E);
    WRITE_DATA (0x04);
    WRITE_DATA (0x07);
    WRITE_DATA (0x07);
    WRITE_DATA (0x06);
    WRITE_DATA (0x24);
    WRITE_DATA (0x05);
    WRITE_DATA (0x12);
    WRITE_DATA (0x11);
    WRITE_DATA (0x29);
    WRITE_DATA (0x30);
    WRITE_DATA (0x1F);
    WRITE_COMMAND (0xFF);
    WRITE_DATA (0x77);
    WRITE_DATA (0x01);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x11);
    WRITE_COMMAND (0xB0);
    WRITE_DATA (0x4D);
    WRITE_COMMAND (0xB1);
    WRITE_DATA (0x2F);
    WRITE_COMMAND (0xB2);
    WRITE_DATA (0x87);
    WRITE_COMMAND (0xB3);
    WRITE_DATA (0x80);
    WRITE_COMMAND (0xB5);
    WRITE_DATA (0x47);
    WRITE_COMMAND (0xB7);
    WRITE_DATA (0x8A);
    WRITE_COMMAND (0xB8);
    WRITE_DATA (0x20);
    WRITE_COMMAND (0xB9);
    WRITE_DATA (0x10);
    WRITE_DATA (0x13);
    WRITE_COMMAND (0xC0);
    WRITE_DATA (0x09);
    WRITE_COMMAND (0xC1);
    WRITE_DATA (0x78);
    WRITE_COMMAND (0xC2);
    WRITE_DATA (0x78);
    WRITE_COMMAND (0xD0);
    WRITE_DATA (0x88);
    WRITE_COMMAND (0xE0);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x02);
    WRITE_COMMAND (0xE1);
    WRITE_DATA (0x04);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x05);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x20);
    WRITE_DATA (0x20);
    WRITE_COMMAND (0xE2);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0xE3);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x33);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0xE4);
    WRITE_DATA (0x22);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0xE5);
    WRITE_DATA (0x04);
    WRITE_DATA (0x34);
    WRITE_DATA (0xAA);
    WRITE_DATA (0xAA);
    WRITE_DATA (0x06);
    WRITE_DATA (0x34);
    WRITE_DATA (0xAA);
    WRITE_DATA (0xAA);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0xE6);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x33);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0xE7);
    WRITE_DATA (0x22);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0xE8);
    WRITE_DATA (0x05);
    WRITE_DATA (0x34);
    WRITE_DATA (0xAA);
    WRITE_DATA (0xAA);
    WRITE_DATA (0x07);
    WRITE_DATA (0x34);
    WRITE_DATA (0xAA);
    WRITE_DATA (0xAA);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0xEB);
    WRITE_DATA (0x02);
    WRITE_DATA (0x00);
    WRITE_DATA (0x40);
    WRITE_DATA (0x40);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0xEC);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0xED);
    WRITE_DATA (0xAA);
    WRITE_DATA (0x45);
    WRITE_DATA (0x0B);
    WRITE_DATA (0xFF);
    WRITE_DATA (0xFF);
    WRITE_DATA (0xFF);
    WRITE_DATA (0xFF);
    WRITE_DATA (0xFF);
    WRITE_DATA (0xFF);
    WRITE_DATA (0xFF);
    WRITE_DATA (0xFF);
    WRITE_DATA (0xFF);
    WRITE_DATA (0xFF);
    WRITE_DATA (0xB0);
    WRITE_DATA (0x54);
    WRITE_DATA (0xAA);
    WRITE_COMMAND (0xEF);
    WRITE_DATA (0x08);
    WRITE_DATA (0x08);
    WRITE_DATA (0x08);
    WRITE_DATA (0x45);
    WRITE_DATA (0x3F);
    WRITE_DATA (0x54);
    WRITE_COMMAND (0xFF);
    WRITE_DATA (0x77);
    WRITE_DATA (0x01);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x13);
    WRITE_COMMAND (0xE8);
    WRITE_DATA (0x00);
    WRITE_DATA (0x0E);
    WRITE_COMMAND (0xFF);
    WRITE_DATA (0x77);
    WRITE_DATA (0x01);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0x11);
    Handle->Controller->Delay_ms(120);
    WRITE_COMMAND (0xFF);
    WRITE_DATA (0x77);
    WRITE_DATA (0x01);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_DATA (0x13);
    WRITE_COMMAND (0xE8);
    WRITE_DATA (0x00);
    WRITE_DATA (0x0C);
    Handle->Controller->Delay_ms(10);
    WRITE_COMMAND (0xE8);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0xFF);
    WRITE_DATA (0x77);
    WRITE_DATA (0x01);
    WRITE_DATA (0x00);
    WRITE_DATA (0x00);
    WRITE_COMMAND (0x36);
    WRITE_DATA (0x10);

    WRITE_COMMAND(0xFFU);
    WRITE_DATA(0x77U);
    WRITE_DATA(0x01U);
    WRITE_DATA(0x00U);
    WRITE_DATA(0x00U);
    WRITE_DATA(0x00U);

    WRITE_COMMAND(0x3AU);
    WRITE_DATA(0x66U);


    WRITE_COMMAND (0x29);

#undef WRITE_COMMAND
#undef WRITE_DATA

    return W430WVC004_A_STATUS_OK;
}
