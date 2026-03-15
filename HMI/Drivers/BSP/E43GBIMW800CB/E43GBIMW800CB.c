/*
 * E43GBIMW800CB.c
 *
 *  Created on: Mar 13, 2026
 *      Author: Jack Herron
 */


#include "E43GBIMW800CB.h"
#include <stdint.h>

uint8_t packet[8];
uint8_t cmd;
uint8_t packet_in_Progress = 0;
uint8_t packet_Length = 0;

void delay()
{
	E43GBIMW800CB_IO_Delay(2);
}

void send_packet()
{
	packet[packet_Length] = cmd;
	DSI_IO_WriteCmd(packet_Length, packet);
}

void write_command(uint8_t command)
{
	if(packet_in_Progress)
	{
		send_packet();
	}
	packet_in_Progress=1;
	cmd = command;
	packet_Length = 0;
}

void write_data(uint8_t data)
{
	packet[packet_Length] = data;
	packet_Length++;
}



uint8_t E43GBIMW800CB_Init()
{

	for (int i = 0; i < LCD_INIT_COMMANDS_COUNT; i++)
	{

		uint8_t param_Count = ili9806e_init_commands[i].parameter_count;
		uint8_t data[9];
		data[0]=ili9806e_init_commands[i].command;

		for (int j = 0; j < ili9806e_init_commands[i].parameter_count; j++) {

			data[j+1] = ili9806e_init_commands[i].parameters[j];
		}

		DSI_IO_WriteCmd(param_Count, data);

		E43GBIMW800CB_IO_Delay(1);
	}
	E43GBIMW800CB_IO_Delay(10);

/*
	write_command(0xFF); // switch to Page1
	write_data(0xFF);
	write_data(0x98);
	write_data(0x06);
	write_data(0x04);
	write_data(0x01);
	delay(10);

	write_command(0x03);
	delay(50);

	write_command(0xFF); // switch to Page1
	write_data(0xFF);
	write_data(0x98);
	write_data(0x06);
	write_data(0x04);
	write_data(0x01);
	delay(50);

	write_command(0x08);// IFMOD SDO always output/SDA
	write_data(0x10);

	write_command(0x20);
	write_data(0x00);

	write_command(0x21);// DE = 1 EN/PCLK Rise/HSYNC = 0/VSYNC = 0
	write_data(0x01);

	write_command(0x30);// 480 x 800
	write_data(0x02);

	write_command(0x31);// Column inversion
	write_data(0x00);

	write_command(0x40);// VCI2x/Output2x/Internal charge pump
	write_data(0x14);

	write_command(0x41);// +5.2V/-5.2V
	write_data(0x33);

	write_command(0x42);// VGL= DDVDH + VCIP - DDVDL, VGH=2DDVDL-VCIP
	write_data(0x01);

	write_command(0x43);// VGH +15V Disabled
	write_data(0x09);

	write_command(0x44);
	write_data(0x09);

	write_command(0x45);
	write_data(0x0A);

	write_command(0x50);// VREG1
	write_data(0x78);

	write_command(0x51);// VREG2
	write_data(0x78);

	write_command(0x52);
	write_data(0x00);

	write_command(0x53);// Forward Flicker VCOM
	write_data(0x3A);

	write_command(0x54);// VCOM
	write_data(0x00);

	write_command(0x55);// Backward Flicker VCOM
	write_data(0x3A);

	write_command(0x57);
	write_data(0x50);

	write_command(0x60);
	write_data(0x07);

	write_command(0x61);
	write_data(0x06);

	write_command(0x62);
	write_data(0x06);

	write_command(0x63);
	write_data(0x04);

	// Positive Gamma
	write_command(0xA0);
	write_data(0x00);

	write_command(0xA1);
	write_data(0x13);

	write_command(0xA2);
	write_data(0x19);

	write_command(0xA3);
	write_data(0x0C);

	write_command(0xA4);
	write_data(0x06);

	write_command(0xA5);
	write_data(0x0A);

	write_command(0xA6);
	write_data(0x06);

	write_command(0xA7);
	write_data(0x04);

	write_command(0xA8);
	write_data(0x09);

	write_command(0xA9);
	write_data(0x08);

	write_command(0xAA);
	write_data(0x12);

	write_command(0xAB);
	write_data(0x06);

	write_command(0xAC);
	write_data(0x0E);

	write_command(0xAD);
	write_data(0x0E);

	write_command(0xAE);
	write_data(0x09);

	write_command(0xAF);
	write_data(0x00);

	// Negative Gamma
	write_command(0xC0);
	write_data(0x00);

	write_command(0xC1);
	write_data(0x0D);

	write_command(0xC2);
	write_data(0x18);

	write_command(0xC3);
	write_data(0x0D);

	write_command(0xC4);
	write_data(0x06);

	write_command(0xC5);
	write_data(0x09);

	write_command(0xC6);
	write_data(0x07);

	write_command(0xC7);
	write_data(0x05);

	write_command(0xC8);
	write_data(0x08);

	write_command(0xC9);
	write_data(0x0E);

	write_command(0xCA);
	write_data(0x12);

	write_command(0xCB);
	write_data(0x09);

	write_command(0xCC);
	write_data(0x0E);

	write_command(0xCD);
	write_data(0x0E);

	write_command(0xCE);
	write_data(0x08);

	write_command(0xCF);
	write_data(0x00);

	write_command(0xFF); // switch to Page6
	write_data(0xFF);
	write_data(0x98);
	write_data(0x06);
	write_data(0x04);
	write_data(0x06);

	// GIP Timing
	write_command(0x00);
	write_data(0x20);

	write_command(0x01);
	write_data(0x04);

	write_command(0x02);
	write_data(0x00);

	write_command(0x03);
	write_data(0x00);

	write_command(0x04);
	write_data(0x01);

	write_command(0x05);
	write_data(0x01);

	write_command(0x06);
	write_data(0x88);

	write_command(0x07);
	write_data(0x04);

	write_command(0x08);
	write_data(0x01);

	write_command(0x09);
	write_data(0x90);

	write_command(0x0A);
	write_data(0x03);

	write_command(0x0B);
	write_data(0x01);

	write_command(0x0C);
	write_data(0x01);

	write_command(0x0D);
	write_data(0x01);

	write_command(0x0E);
	write_data(0x00);

	write_command(0x0F);
	write_data(0x00);


	write_command(0x10);
	write_data(0x55);

	write_command(0x11);
	write_data(0x53);

	write_command(0x12);
	write_data(0x01);

	write_command(0x13);
	write_data(0x0D);

	write_command(0x14);
	write_data(0x0D);

	write_command(0x15);
	write_data(0x43);

	write_command(0x16);
	write_data(0x0B);

	write_command(0x17);
	write_data(0x00);

	write_command(0x18);
	write_data(0x00);

	write_command(0x19);
	write_data(0x00);

	write_command(0x1A);
	write_data(0x00);

	write_command(0x1B);
	write_data(0x00);

	write_command(0x1C);
	write_data(0x00);

	write_command(0x1D);
	write_data(0x00);


	write_command(0x20);
	write_data(0x01);

	write_command(0x21);
	write_data(0x23);

	write_command(0x22);
	write_data(0x45);

	write_command(0x23);
	write_data(0x67);

	write_command(0x24);
	write_data(0x01);

	write_command(0x25);
	write_data(0x23);

	write_command(0x26);
	write_data(0x45);

	write_command(0x27);
	write_data(0x67);


	write_command(0x30);
	write_data(0x02);

	write_command(0x31);
	write_data(0x22);

	write_command(0x32);
	write_data(0x11);

	write_command(0x33);
	write_data(0xAA);

	write_command(0x34);
	write_data(0xBB);

	write_command(0x35);
	write_data(0x66);

	write_command(0x36);
	write_data(0x00);

	write_command(0x37);
	write_data(0x22);

	write_command(0x38);
	write_data(0x22);

	write_command(0x39);
	write_data(0x22);

	write_command(0x3A);
	write_data(0x22);

	write_command(0x3B);
	write_data(0x22);

	write_command(0x3C);
	write_data(0x22);

	write_command(0x3D);
	write_data(0x22);

	write_command(0x3E);
	write_data(0x22);

	write_command(0x3F);
	write_data(0x22);

	write_command(0x40);
	write_data(0x22);

	write_command(0x52);
	write_data(0x10);

	write_command(0x53);
	write_data(0x12);

	write_command(0x54);
	write_data(0x13);

	write_command(0xFF); // switch to Page7
	write_data(0xFF);
	write_data(0x98);
	write_data(0x06);
	write_data(0x04);
	write_data(0x07);


	write_command(0x17);
	write_data(0x32);

	write_command(0x18);
	write_data(0x1D);

	write_command(0x26);
	write_data(0xB2);

	write_command(0x02);
	write_data(0x77);

	write_command(0xE1);
	write_data(0x79);

	write_command(0xB3);
	write_data(0x10);

	write_command(0xFF); // switch to Page0
	write_data(0xFF);
	write_data(0x98);
	write_data(0x06);
	write_data(0x04);
	write_data(0x00);

	write_command(0x36);
	write_data(0x02);

	write_command(0x3A);
	write_data(0x50);

	write_command(0x11); // Sleep Out
	delay(300);

	write_command(0x29); // Display On
	write_command(0x23); // all pixels off
	send_packet();
	delay(50);
	*/

	return(1);
}

