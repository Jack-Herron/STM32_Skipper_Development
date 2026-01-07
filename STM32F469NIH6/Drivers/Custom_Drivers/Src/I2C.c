/*
 * I2C.c
 *
 *  Created on: Jan 2, 2026
 *      Author: jackh
 */
#include "I2C.h"
#include "stm32f4xx.h"
#include "Clock.h"
#include <stdlib.h>

#define I2C___LOOP_TIMEOUT	20

I2C_Inst_TypeDef I2C_Inst[3];

I2C_Inst_TypeDef* I2C___Get_Instance_From_Port(uint8_t port)
{
	if(port-1 < 3)
	{
		return(&I2C_Inst[port-1]);
	}
	else
	{
		return(NULL);
	}
}

int I2C___Init(uint8_t port, I2C___Conf_TypeDef* I2C_Conf)
{
	I2C_Inst_TypeDef* Inst;

	switch(port)
	{
	case 1:
		Inst 	=  	&I2C_Inst[0];
		Inst 	-> 	I2Cx = I2C1;
		RCC 	-> 	APB1ENR |= RCC_APB1ENR_I2C1EN;
		//NVIC_EnableIRQ(I2C1_ER_IRQn);
		//NVIC_SetPriority(I2C1_ER_IRQn, 14);
		//NVIC_EnableIRQ(I2C1_EV_IRQn);
		//NVIC_SetPriority(I2C1_EV_IRQn, 15);
		break;
	case 2:
		Inst 	= 	&I2C_Inst[1];
		Inst 	-> 	I2Cx = I2C2;
		RCC 	-> 	APB1ENR |= RCC_APB1ENR_I2C2EN;
		//NVIC_EnableIRQ(I2C2_ER_IRQn);
		//NVIC_SetPriority(I2C2_ER_IRQn, 14);
		//NVIC_EnableIRQ(I2C2_EV_IRQn);
		//NVIC_SetPriority(I2C2_EV_IRQn, 15);
		break;
	case 3:
		Inst 	= 	&I2C_Inst[2];
		Inst 	-> 	I2Cx = I2C3;
		RCC 	-> 	APB1ENR |= RCC_APB1ENR_I2C3EN;
		//NVIC_EnableIRQ(I2C3_ER_IRQn);
		//NVIC_SetPriority(I2C3_ER_IRQn, 14);
		//NVIC_EnableIRQ(I2C3_EV_IRQn);
		//NVIC_SetPriority(I2C3_EV_IRQn, 15);
		break;
	default:
		return(1);
	}

	Inst->is_Initialized = 1;

	Inst->Conf = *I2C_Conf;
	Inst -> I2Cx -> CR1 =	0;														// Clear CR1
	Inst -> I2Cx -> CR2 =	//I2C_CR2_ITERREN | I2C_CR2_ITEVTEN |					// Enable Interrupts (Not implemented yet)
							(((uint32_t)I2C_FREQ/1000000) << I2C_CR2_FREQ_Pos);		// Set frequency

	uint32_t APB_Cycles_Per_CCR;
	uint8_t fast_Mode;

	if(Inst->Conf.Freq <= 100000)													// If I2C in standard mode
	{
		APB_Cycles_Per_CCR = 2;
		fast_Mode = 0;
	}
	else if(Inst->Conf.Fm_Mode == Fm_2high1low)										// else if I2C in fast mode with 2high1low setting
	{
		APB_Cycles_Per_CCR = 3;
		fast_Mode = 1;
	}
	else																			// else I2C in fast mode with 16high9low setting
	{
		APB_Cycles_Per_CCR = 25;
		fast_Mode = 1;
	}

	uint32_t CCR_Value = ((uint32_t)I2C_FREQ)/(APB_Cycles_Per_CCR*Inst->Conf.Freq);

	Inst -> I2Cx ->CCR 		= (fast_Mode << I2C_CCR_FS_Pos) | (((uint32_t)Inst->Conf.Fm_Mode) << I2C_CCR_DUTY_Pos) | (CCR_Value << I2C_CCR_CCR_Pos);
	Inst -> I2Cx -> TRISE 	= ((I2C_FREQ/(1000000000ul/Inst->Conf.Rise_Time_ns))+1) & I2C_TRISE_TRISE_Msk;

	Inst -> I2Cx -> CR1 |= I2C_CR1_PE;

	return(0);
}

// Added while escapes to avoid infinite loops, but I am not happy with these functions, they need to be optimized TODO
int I2C___Write(uint8_t port, uint8_t address, uint8_t* buffer, uint32_t write_Length)
{
	I2C_TypeDef* I2Cx = I2C___Get_Instance_From_Port(port)->I2Cx;

	uint32_t start_Tick = clock___millis();
	uint32_t loop_Tick = start_Tick;

	while ((I2Cx->SR2 & I2C_SR2_BUSY) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))			// wait for bus to be free
	{
		loop_Tick = clock___millis();
	}

	if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
	{
		return(1);
	}

	I2Cx->CR1 |= I2C_CR1_START;												// trigger start generation

	start_Tick = clock___millis();
	loop_Tick = start_Tick;

	while ((!(I2Cx->SR1 & I2C_SR1_SB)) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))			// wait for start to be generated
	{
		loop_Tick = clock___millis();
	}

	if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
	{
		return(1);
	}

	I2Cx->DR = (address << 1) | 0;				// sent slave address and write bit

	start_Tick = clock___millis();
	loop_Tick = start_Tick;
	while ((!(I2Cx->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))												// wait for address, or NACK
	{
		loop_Tick = clock___millis();
	}

	if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
	{
		return(1);
	}

	if (I2Cx->SR1 & I2C_SR1_AF)
	{
		I2Cx->SR1 &= ~I2C_SR1_AF; 				// if address was NACKed, return
		I2Cx->CR1 |= I2C_CR1_STOP;
		return(1);
	}

	(void)I2Cx->SR1;							// read status registers to trigger next phase
	(void)I2Cx->SR2;

	for(uint32_t i = 0; i < write_Length; i++)
	{
		start_Tick = clock___millis();
		loop_Tick = start_Tick;
		while ((!(I2Cx->SR1 & I2C_SR1_TXE)) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))			// wait for TX path to be free
		{
			loop_Tick = clock___millis();
		}

		if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
		{
			return(1);
		}

		I2Cx->DR = buffer[i];

		start_Tick = clock___millis();
		loop_Tick = start_Tick;
		while ((!(I2Cx->SR1 & (I2C_SR1_BTF | I2C_SR1_AF))) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))	// wait for the byte to be fully sent
		{
			loop_Tick = clock___millis();
		}

		if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
		{
			return(1);
		}

		if (I2Cx->SR1 & I2C_SR1_AF)
		{
			I2Cx->SR1 &= ~I2C_SR1_AF; 								// if data was NACKed, return
			I2Cx->CR1 |= I2C_CR1_STOP;
			return(1);
		}
	}

	I2Cx->CR1 |= I2C_CR1_STOP;						// Generate stop

	return(0);
}


//TODO add timeout values to while loops to avoid indefinite blocking
int I2C___Read_Reg(uint8_t port, uint8_t address, uint8_t reg_Address, uint8_t* buffer, uint32_t read_Length)
{
	I2C_TypeDef* I2Cx = I2C___Get_Instance_From_Port(port)->I2Cx;

	uint32_t start_Tick = clock___millis();
	uint32_t loop_Tick = start_Tick;
	while ((I2Cx->SR2 & I2C_SR2_BUSY) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))			// wait for bus to be free
	{
		loop_Tick = clock___millis();
	}

	if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
	{
		return(1);
	}

	I2Cx->CR1 |= I2C_CR1_START;					// trigger start generation

	start_Tick = clock___millis();
	loop_Tick = start_Tick;
	while ((!(I2Cx->SR1 & I2C_SR1_SB)) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))			// wait for start to be generated
	{
		loop_Tick = clock___millis();
	}

	if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
	{
		return(1);
	}

	I2Cx->DR = (address << 1) | 0;				// sent slave address and write bit

	start_Tick = clock___millis();
	loop_Tick = start_Tick;
	while ((!(I2Cx->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))											// wait for address, or NACK
	{
		loop_Tick = clock___millis();
	}

	if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
	{
		return(1);
	}

	if (I2Cx->SR1 & I2C_SR1_AF)
	{
		I2Cx->SR1 &= ~I2C_SR1_AF; 				// if address was NACKed, return
		I2Cx->CR1 |= I2C_CR1_STOP;
		return(1);
	}

	(void)I2Cx->SR1;							// read status registers to trigger next phase
	(void)I2Cx->SR2;


	start_Tick = clock___millis();
	loop_Tick = start_Tick;
	while ((!(I2Cx->SR1 & I2C_SR1_TXE)) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))			// wait for TX path to be free
	{
		loop_Tick = clock___millis();
	}

	if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
	{
		return(1);
	}


	I2Cx->DR = reg_Address;

	start_Tick = clock___millis();
	loop_Tick = start_Tick;
	while ((!(I2Cx->SR1 & I2C_SR1_BTF)) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))			// wait for the byte to be fully sent
	{
		loop_Tick = clock___millis();
	}

	if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
	{
		return(1);
	}

	I2Cx->CR1 |= I2C_CR1_START;					// trigger start generation

	start_Tick = clock___millis();
	loop_Tick = start_Tick;
	while ((!(I2Cx->SR1 & I2C_SR1_SB)) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))			// wait for start to be generated
	{
		loop_Tick = clock___millis();
	}

	if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
	{
		return(1);
	}

	I2Cx->DR = (address << 1) | 1;				// sent slave address and write bit

	start_Tick = clock___millis();
	loop_Tick = start_Tick;
	while ((!(I2Cx->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))												// wait for address, or NACK
	{
		loop_Tick = clock___millis();
	}

	if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
	{
		return(1);
	}

	if (I2Cx->SR1 & I2C_SR1_AF) { I2Cx->SR1 &= ~I2C_SR1_AF; I2Cx->CR1 |= I2C_CR1_STOP; return(1); }	// if address was NACKed, return

	(void)I2Cx->SR1;							// read status registers to trigger next phase
	(void)I2Cx->SR2;

    if (read_Length == 0)
    {
        I2Cx->CR1 |= I2C_CR1_STOP;				// Generate stop
    }
    else if (read_Length == 1)
    {
        I2Cx->CR1 &= ~I2C_CR1_ACK;				// Nack byte

        I2Cx->CR1 |= I2C_CR1_STOP;				// Generate stop

    	start_Tick = clock___millis();
    	loop_Tick = start_Tick;
    	while ((!(I2Cx->SR1 & I2C_SR1_RXNE)) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))
        {
        	loop_Tick = clock___millis();
        }

        if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
		{
			return(1);
		}

        buffer[0] = (uint8_t)I2Cx->DR;			// Get the byte
    }
    else if (read_Length == 2)					// if receiving 2 bytes
    {
        I2Cx->CR1 &= ~I2C_CR1_ACK;				// Special case: use POS, disable ACK
        I2Cx->CR1 |=  I2C_CR1_POS;

        start_Tick = clock___millis();
    	loop_Tick = start_Tick;
        while ((!(I2Cx->SR1 & I2C_SR1_BTF)) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT)) 		// Wait for byte transfer to finish
        {
        	loop_Tick = clock___millis();
        }

        if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
		{
			return(1);
		}

        I2Cx->CR1 |= I2C_CR1_STOP;				// Generate stop

        buffer[0] = (uint8_t)I2Cx->DR;			// Copy data to buffer
        buffer[1] = (uint8_t)I2Cx->DR;

        I2Cx->CR1 &= ~I2C_CR1_POS;				// Restore to previous state
    }
    else
    {
        I2Cx->CR1 |= I2C_CR1_ACK;				// set ack

        uint32_t i = 0;
        while (i < read_Length) {
            if (i == read_Length - 3) {
                // Handle last 3 bytes

                start_Tick = clock___millis();
            	loop_Tick = start_Tick;
                while ((!(I2Cx->SR1 & I2C_SR1_BTF)) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))
                {
                	loop_Tick = clock___millis();
                }

                if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
        		{
        			return(1);
        		}

                I2Cx->CR1 &= ~I2C_CR1_ACK;     // prepare NACK for last byte
                buffer[i++] = (uint8_t)I2Cx->DR;

                I2Cx->CR1 |= I2C_CR1_STOP;

                buffer[i++] = (uint8_t)I2Cx->DR;

                start_Tick = clock___millis();
            	loop_Tick = start_Tick;
                while ((!(I2Cx->SR1 & I2C_SR1_RXNE)) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))
                {
                	loop_Tick = clock___millis();
                }

                if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
				{
					return(1);
				}

                buffer[i++] = (uint8_t)I2Cx->DR;
                break;
            }

            start_Tick = clock___millis();
            loop_Tick = start_Tick;
            while ((!(I2Cx->SR1 & I2C_SR1_RXNE)) & (loop_Tick - start_Tick < I2C___LOOP_TIMEOUT))
            {
            	loop_Tick = clock___millis();
            }

            if(loop_Tick - start_Tick >= I2C___LOOP_TIMEOUT)
			{
				return(1);
			}

            buffer[i++] = (uint8_t)I2Cx->DR;
        }
    }

    return(0);
}

int I2C___Is_Initilized(uint8_t port)
{
	if((port-1) < 3)
	{
		return(I2C_Inst[port-1].is_Initialized);
	}
	return(0);
}


// Interrupts not implemented yet
void I2C_EV_IRQHandler(volatile I2C_Inst_TypeDef* I2C)	// All I2C Event Handler
{
	while(1);
}

void I2C_ER_IRQHandler(volatile I2C_Inst_TypeDef* I2C)	// All I2C Error Handler
{
	while(1);
}

void I2C1_EV_IRQHandler(void){I2C_EV_IRQHandler(&I2C_Inst[0]);}	// I2C1 event handler

void I2C1_ER_IRQHandler(void){I2C_EV_IRQHandler(&I2C_Inst[0]);} // I2C1 error handler

void I2C2_EV_IRQHandler(void){I2C_EV_IRQHandler(&I2C_Inst[1]);}	// I2C2 event handler

void I2C2_ER_IRQHandler(void){I2C_EV_IRQHandler(&I2C_Inst[1]);} // I2C2 error handler

void I2C3_EV_IRQHandler(void){I2C_EV_IRQHandler(&I2C_Inst[2]);}	// I2C3 event handler

void I2C3_ER_IRQHandler(void){I2C_EV_IRQHandler(&I2C_Inst[2]);} // I2C3 error handler
