#include <stdint.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "indicate.h"
#include "boost.h"
#include "CCS.h"
#include "CAN.h"


int main(void)
{
	clock___Init();
	indicate___Init();
	boost___Init();
	CCS___Init();
	CAN___Init();
	CCS___Write_Channel(0, 0.180);
	boost___Set_Voltage(52.0);

	printf("HELLOTEST\n");

	clock___Delay_ms(1000);
	indicate___Set_Value(1);
	clock___Delay_ms(1000);
	indicate___Set_Value(3);
	clock___Delay_ms(1000);
	indicate___Set_Value(7);
	boost___Enable();

	for(;;)
	{
		clock___Delay_ms(1000);
		printf("This is a test of printf through CAN->HMI->USB\n");
	}
}
