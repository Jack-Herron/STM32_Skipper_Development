#include <stdint.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "indicate.h"
#include "boost.h"
#include "CCS.h"
#include "CAN.h"

#define test_Current 250

uint8_t LED[7] = {3,2,5,4,1,6,0};

int main(void)
{
	clock___Init();
	indicate___Init();
	boost___Init();
	CCS___Init();
	CAN___Init();

	CCS___Write_Channel(0, 0);
	CCS___Write_Channel(1, 0);
	CCS___Write_Channel(2, 0);
	CCS___Write_Channel(3, 0);
	CCS___Write_Channel(4, 0);
	CCS___Write_Channel(5, 0);
	CCS___Write_Channel(6, 0);
	CCS___Write_Channel(7, 0);

	boost___Set_Voltage(59.0);

	clock___Delay_ms(1000);
	indicate___Set_Value(1);
	clock___Delay_ms(1000);
	indicate___Set_Value(3);
	clock___Delay_ms(1000);
	indicate___Set_Value(7);
	boost___Enable();

	for(;;)
	{
		for(uint8_t i = 0; i < 7; i++)
		{
			for(uint8_t j = 0; j < test_Current; j++)
			{
				CCS___Write_Channel(LED[i], (float)j * 0.001f);
				clock___Delay_ms(10);
			}
			clock___Delay_ms(10*test_Current);
			for(uint8_t j = test_Current; j > 0; j--)
			{
				CCS___Write_Channel(LED[i], (float)j * 0.001f);
				clock___Delay_ms(10);
			}
		}

		for(uint8_t j = 0; j < test_Current; j++)
		{
			CCS___Write_Channel(0, (float)j * 0.001f);
			CCS___Write_Channel(1, (float)j * 0.001f);
			CCS___Write_Channel(2, (float)j * 0.001f);
			CCS___Write_Channel(3, (float)j * 0.001f);
			CCS___Write_Channel(4, (float)j * 0.001f);
			CCS___Write_Channel(5, (float)j * 0.001f);
			CCS___Write_Channel(6, (float)j * 0.001f);
			CCS___Write_Channel(7, (float)j * 0.001f);
			clock___Delay_ms(10);
		}
		clock___Delay_ms(10*test_Current);
		for(uint8_t j = test_Current; j > 0; j--)
		{
			CCS___Write_Channel(0, (float)j * 0.001f);
			CCS___Write_Channel(1, (float)j * 0.001f);
			CCS___Write_Channel(2, (float)j * 0.001f);
			CCS___Write_Channel(3, (float)j * 0.001f);
			CCS___Write_Channel(4, (float)j * 0.001f);
			CCS___Write_Channel(5, (float)j * 0.001f);
			CCS___Write_Channel(6, (float)j * 0.001f);
			CCS___Write_Channel(7, (float)j * 0.001f);
			clock___Delay_ms(10);
		}

	}
}
