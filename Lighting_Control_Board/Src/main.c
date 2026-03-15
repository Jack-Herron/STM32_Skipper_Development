#include <stdint.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "indicate.h"
#include "boost.h"

int main(void)
{
	clock___Init();
	indicate___Init();
	boost___Init();

	for(;;)
	{
		for(uint8_t i = 30; i <= 60; i++)
		{
			boost___Set_Voltage((float)i);
			indicate___Set_Value(i);
			clock___Delay_ms(2000);
		}
		/*
		for(uint8_t i = 0 ; i < 8; i++)
		{
			indicate___Set_Value(i);
			clock___Delay_ms(250);
		}
		*/
	}
}
