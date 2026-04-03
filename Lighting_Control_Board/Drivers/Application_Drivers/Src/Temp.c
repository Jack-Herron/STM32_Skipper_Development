/*
 * Temp.c
 *
 *  Created on: Apr 2, 2026
 *      Author: Jack Herron
 */
#include <stdint.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "Temp.h"
#include "I2C.h"


void Temp___Init()
{
	I2C___Conf_TypeDef conf =
	{
	    .Freq = 100000,          // 100 kHz
	    .Fm_Mode = Fm_2high1low, // ignored in standard mode, but fine
	    .Rise_Time_ns = 1000     // standard-mode spec
	};

	I2C___Init(&conf);
}

float Temp___Get_Temp(uint8_t address)
{
    uint8_t dev_addr = (0x48 | (address & 0x07));  // A2 A1 A0
    uint8_t reg = 0x00;                            // temperature register
    uint8_t data[2];

    // Read 2 bytes from temp register
    if (I2C___Read_Reg(dev_addr, reg, data, 2) != 0)
    {
        return -1000.0f; // error value
    }

    // Combine bytes
    int16_t raw = (data[0] << 8) | data[1];

    /*
     * LM75 (classic):
     * D15..D7 = temp
     * D6..D0  = unused
     *
     * So shift right by 7
     */
    raw >>= 7;

    // Handle sign (9-bit signed)
    if (raw & 0x0100)   // sign bit
    {
        raw |= 0xFE00;  // sign extend
    }

    // Convert to °C (0.5°C per LSB)
    return raw * 0.5f;
}
