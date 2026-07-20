
#include <stdint.h>
#include "STM32H7xx.h"
#include "Board.h"

int main(void)
{
	SCB->CPACR |= ((3UL << 20U) | (3UL << 22U));

	__DSB();
	__ISB();

	SCB_EnableDCache();

	Board_Init();

	for(;;);
}
