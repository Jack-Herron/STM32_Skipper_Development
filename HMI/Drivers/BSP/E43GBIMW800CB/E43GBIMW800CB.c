/*
 * E43GBIMW800CB.c
 *
 *  Created on: Mar 13, 2026
 *      Author: Jack Herron
 */


#include "E43GBIMW800CB.h"
#include <stdint.h>

uint8_t E43GBIMW800CB_Init()
{
	/* Send all initialization commands */
	for (int i = 0; i < LCD_INIT_COMMANDS_COUNT; i++) {

		uint8_t param_Count = ili9806e_init_commands[i].parameter_count;
		uint8_t data[9];
		data[0]=ili9806e_init_commands[i].command;

		for (int j = 0; j < ili9806e_init_commands[i].parameter_count; j++) {

			data[j+1] = ili9806e_init_commands[i].parameters[j];
		}

		DSI_IO_WriteCmd(param_Count, data);

		/* Small delay between commands as required by the panel */
		E43GBIMW800CB_IO_Delay(1);
	}

	/* Additional delay after initialization */
	E43GBIMW800CB_IO_Delay(10);

	return(1);
}
