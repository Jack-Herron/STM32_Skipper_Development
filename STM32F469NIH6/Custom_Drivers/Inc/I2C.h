/*
 * I2C.h
 *
 *  Created on: Jan 2, 2026
 *      Author: jackh
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "stm32f4xx.h"

enum I2C_Fm_Mode
{
	Fm_2high1low,
	Fm_16high9low
};

typedef struct
{
	uint32_t 			Freq;
	enum I2C_Fm_Mode    Fm_Mode;
	uint32_t			Rise_Time_ns;
}I2C___Conf_TypeDef;

typedef struct
{
	I2C_TypeDef* 		I2Cx;
	I2C___Conf_TypeDef	Conf;
	uint8_t				is_Initialized;
} I2C_Inst_TypeDef;


int I2C___Init(uint8_t port, I2C___Conf_TypeDef* I2C_Conf);
int I2C___Is_Initilized(uint8_t port);
int I2C___Read_Reg(uint8_t port, uint8_t address, uint8_t reg_Address, uint8_t* buffer, uint32_t read_Length);
int I2C___Write(uint8_t port, uint8_t address, uint8_t* buffer, uint32_t write_Length);
#endif /* INC_I2C_H_ */
