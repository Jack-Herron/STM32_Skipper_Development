/*
 * E43GBIMW800CB.h
 *
 *  Created on: Mar 13, 2026
 *      Author: Jack Herron
 */

/* E43GB-I-MW800-CB MIPI DSI TFT Display Driver File
   Created on August 29, 2025, 15:10 AM

   @Company
	 Focus LCDs
	 LCDs Made Simple
	 www.focuslcds.com

   @File Name
	 E43GB-I-MW800-CB-init.txt

   @Summary
	 This is the driver source file for the TFT Display
	 Display:         4.3" TFT LCD 480 x 800
	 Controller Chip: ILI9806E
	 Interface:       MIPI DSI 2-Lane
	 MCU/MPU:         Generic

   @Version
	 Version 1.0.0 (Semantic Versioning 2.0.0)

   @Description
	 This source file provides initialization for the 4.3" TFT display.
*/
/*******************************************************************************/

/*******************************************************************************
	(c) 2023 Focus LCDs and its subsidiaries. You may use this software and any
	derivatives exclusively with Focus LCDs products.

	THIS SOFTWARE IS SUPPLIED BY FOCUS LCDS "AS IS". NO WARRANTIES, WHETHER
	EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
	WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
	PARTICULAR PURPOSE, OR ITS INTERACTION WITH FOCUS LCDS PRODUCTS, COMBINATION
	WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

	IN NO EVENT WILL FOCUS LCDS BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
	INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
	WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF FOCUS LCDS HAS
	BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
	FULLEST EXTENT ALLOWED BY LAW, FOCUS LCDS' TOTAL LIABILITY ON ALL CLAIMS IN
	ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
	THAT YOU HAVE PAID DIRECTLY TO FOCUSLCDS FOR THIS SOFTWARE.

	FOCUS LCDS PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
	TERMS.
*******************************************************************************/

#ifndef BSP_E43GBIMW800CB_E43GBIMW800CB_H_
#define BSP_E43GBIMW800CB_E43GBIMW800CB_H_

#include <stdint.h>

/*
 * E43GB-I-MW800-CB 4.3" IPS LCD Panel Configuration
 *
 * Physical Specifications:
 * - Diagonal Size: 4.3 inches
 * - Resolution: 480 x 800 (WVGA)
 * - Aspect Ratio: 3:5 (Portrait)
 * - Display Type: IPS (In-Plane Switching)
 * - Polarizer: Transmissive
 * - Operating Temperature: -20°C to +70°C
 *
 * Interface Specifications:
 * - Interface: 2-Lane MIPI DSI
 * - TFT Controller: ILI9806E (based on initialization commands)
 * - Touch Controller: GT911 (Capacitive Touch Panel)
 * - Touch Panel Type: Capacitive
 * - Video Mode: Burst mode
 *
 * This file contains timing parameters and initialization commands for the
 * E43GB-I-MW800-CB 4.3" IPS LCD panel. These parameters are critical for proper
 * display timing and synchronization.
 *
 * Electrical Specifications:
 * - VDD: 1.8V (Digital supply)
 * - RESET: VDD (Active low reset)
 * - STBYB: VDD (Standby control, active high)
 * - VGH: 18V (Gate high voltage)
 * - VGL: -6V (Gate low voltage)
 * - AVDD: 9.6V (Analog supply)
 * - VCOM: 3.2V (Common voltage, typical)
 *
 * MIPI Configuration:
 * - Clock Speed: 15MHz (15000 kHz)
 * - Data Lane: 2-lane MIPI DSI
 * - DSI Flags: 0x10c
 */

/*
 * LCD Timing Parameters
 *
 * These parameters define the blanking intervals and synchronization pulses
 * required for proper LCD timing. They are specified in pixel clocks (horizontal)
 * or scan lines (vertical).
 *
 * Timing Diagram:
 *
 * Horizontal Timing:
 * |<-- HSPW --><-- HBPD --><-- Active Display (480px) --><-- HFPD -->|
 * |            |          |                             |           |
 * | Sync Pulse | Back     |                             | Front     |
 * |            | Porch    |                             | Porch     |
 *
 * Vertical Timing:
 * |<-- VSPW --><-- VBPD --><-- Active Display (800 lines) --><-- VFPD -->|
 * |            |          |                               |           |
 * | Sync Pulse | Back     |                               | Front     |
 * |            | Porch    |                               | Porch     |
 */

/* Vertical Timing Parameters */
#define VBPD  14   /* Vertical Back Porch - 14 scan lines before active display */
#define VFPD  14   /* Vertical Front Porch - 14 scan lines after active display */
#define VSPW  4    /* Vertical Sync Pulse Width - 4 scan lines sync pulse */

/* Horizontal Timing Parameters */
#define HBPD  5    /* Horizontal Back Porch - 5 pixel clocks before active display */
#define HFPD  5    /* Horizontal Front Porch - 5 pixel clocks after active display */
#define HSPW  4    /* Horizontal Sync Pulse Width - 4 pixel clocks sync pulse */

/*
 * ILI9806E Initialization Commands
 *
 * These commands configure the ILI9806E TFT controller for proper operation
 * with the 4.3" IPS panel. Commands are sent using the GP_COMMAD_PA and W_D
 * macros which handle the command parameter and data writing.
 */

/* ILI9806E initialization command structure */
typedef struct {
    uint8_t command;
    uint8_t parameter_count;
    uint8_t parameters[8];  /* Maximum 8 parameters per command */
} ili9806e_init_cmd_t;

/* LCD initialization commands for E43GB-I-MW800-CB 4.3" IPS panel */
static const ili9806e_init_cmd_t ili9806e_init_commands[] = {
    /* Command Set 1: Extended Function Control */
    {0xFF, 5, {0xFF, 0x98, 0x06, 0x04, 0x01}},

    /* Command Set 2: Display Control */

    {0x08, 1, {0x10, 0x00}},	// IFMODE one setting output on SDA
    {0x21, 1, {0x01, 0x00}},	// DE active high, DPL rising edge, HSPL active low, DSPL active low
    {0x30, 1, {0x02, 0x00}},	// Set resolution to 480 x 800
    {0x31, 1, {0x00, 0x00}},
    {0x60, 1, {0x06, 0x00}},
    {0x61, 1, {0x00, 0x00}},
    {0x62, 1, {0x07, 0x00}},
    {0x63, 1, {0x00, 0x00}},
    {0x40, 1, {0x16, 0x00}},
    {0x41, 1, {0x44, 0x00}},
    {0x42, 1, {0x00, 0x00}},
    {0x43, 1, {0x83, 0x00}},
    {0x44, 1, {0x89, 0x00}},
    {0x45, 1, {0x8A, 0x00}},
    {0x46, 1, {0x44, 0x00}},
    {0x47, 1, {0x44, 0x00}},
    {0x50, 1, {0x88, 0x00}},
    {0x51, 1, {0x88, 0x00}},
    {0x52, 1, {0x00, 0x00}},
    {0x53, 1, {0x47, 0x00}},
    {0x54, 1, {0x00, 0x00}},
    {0x55, 1, {0x47, 0x00}},
    {0x56, 1, {0x00, 0x00}},

    /* Command Set 3: Gamma Control (Positive) */
    {0xA0, 1, {0x00, 0x00}},
    {0xA1, 1, {0x10, 0x00}},
    {0xA2, 1, {0x0C, 0x00}},
    {0xA3, 1, {0x06, 0x00}},
    {0xA4, 1, {0x03, 0x00}},
    {0xA5, 1, {0x07, 0x00}},
    {0xA6, 1, {0x06, 0x00}},
    {0xA7, 1, {0x04, 0x00}},
    {0xA8, 1, {0x09, 0x00}},
    {0xA9, 1, {0x08, 0x00}},
    {0xAA, 1, {0x12, 0x00}},
    {0xAB, 1, {0x06, 0x00}},
    {0xAC, 1, {0x0E, 0x00}},
    {0xAD, 1, {0x0E, 0x00}},
    {0xAE, 1, {0x09, 0x00}},
    {0xAF, 1, {0x00, 0x00}},

    /* Command Set 4: Gamma Control (Negative) */
    {0xC0, 1, {0x00, 0x00}},
    {0xC1, 1, {0x00, 0x00}},
    {0xC2, 1, {0x08, 0x00}},
    {0xC3, 1, {0x08, 0x00}},
    {0xC4, 1, {0x01, 0x00}},
    {0xC5, 1, {0x07, 0x00}},
    {0xC6, 1, {0x05, 0x00}},
    {0xC7, 1, {0x04, 0x00}},
    {0xC8, 1, {0x09, 0x00}},
    {0xC9, 1, {0x0C, 0x00}},
    {0xCA, 1, {0x16, 0x00}},
    {0xCB, 1, {0x0B, 0x00}},
    {0xCC, 1, {0x11, 0x00}},
    {0xCD, 1, {0x21, 0x00}},
    {0xCE, 1, {0x0F, 0x00}},
    {0xCF, 1, {0x00, 0x00}},

    /* Command Set 5: Extended Function Control */
    {0xFF, 5, {0xFF, 0x98, 0x06, 0x04, 0x06}},
    {0x00, 1, {0x21, 0x00}},

    /* Command Set 6: Display Control */
    {0x01, 1, {0x0A, 0x00}},
    {0x02, 1, {0x60, 0x00}},
    {0x03, 1, {0x0F, 0x00}},
    {0x04, 1, {0x01, 0x00}},
    {0x05, 1, {0x01, 0x00}},
    {0x06, 1, {0x98, 0x00}},
    {0x07, 1, {0x06, 0x00}},
    {0x08, 1, {0x01, 0x00}},
    {0x09, 1, {0x00, 0x00}},
    {0x0A, 1, {0x00, 0x00}},
    {0x0B, 1, {0x00, 0x00}},
    {0x0C, 1, {0x23, 0x00}},
    {0x0D, 1, {0x23, 0x00}},
    {0x0E, 1, {0x00, 0x00}},
    {0x0F, 1, {0x00, 0x00}},
    {0x10, 1, {0xF7, 0x00}},
    {0x11, 1, {0xF0, 0x00}},
    {0x12, 1, {0x00, 0x00}},
    {0x13, 1, {0x00, 0x00}},
    {0x14, 1, {0x00, 0x00}},
    {0x15, 1, {0xC0, 0x00}},
    {0x16, 1, {0x08, 0x00}},
    {0x17, 1, {0x00, 0x00}},
    {0x18, 1, {0x00, 0x00}},
    {0x19, 1, {0x00, 0x00}},
    {0x1A, 1, {0x00, 0x00}},
    {0x1B, 1, {0x00, 0x00}},
    {0x1C, 1, {0x00, 0x00}},
    {0x1D, 1, {0x00, 0x00}},

    /* Command Set 7: Color Control */
    {0x20, 1, {0x01, 0x00}},
    {0x21, 1, {0x23, 0x00}},
    {0x22, 1, {0x44, 0x00}},
    {0x23, 1, {0x67, 0x00}},
    {0x24, 1, {0x01, 0x00}},
    {0x25, 1, {0x23, 0x00}},
    {0x26, 1, {0x45, 0x00}},
    {0x27, 1, {0x67, 0x00}},

    /* Command Set 8: Gamma Control */
    {0x30, 1, {0x01, 0x00}},
    {0x31, 1, {0x00, 0x00}},
    {0x32, 1, {0x76, 0x00}},
    {0x33, 1, {0x88, 0x00}},
    {0x34, 1, {0x22, 0x00}},
    {0x35, 1, {0x55, 0x00}},
    {0x36, 1, {0x23, 0x00}},
    {0x37, 1, {0x11, 0x00}},
    {0x38, 1, {0xBC, 0x00}},
    {0x39, 1, {0xAD, 0x00}},
    {0x3A, 1, {0xDA, 0x00}},
    {0x3B, 1, {0xCB, 0x00}},
    {0x3C, 1, {0x22, 0x00}},
    {0x3D, 1, {0x22, 0x00}},
    {0x3E, 1, {0x22, 0x00}},
    {0x3F, 1, {0x22, 0x00}},
    {0x40, 1, {0x22, 0x00}},

    /* Command Set 9: Power Control */
    {0x52, 1, {0x10, 0x00}},  /* Default */
    {0x53, 1, {0x10, 0x00}},  /* Default */
    {0x54, 1, {0x13, 0x00}},  /* Default */

    /* Command Set 10: Extended Function Control */
    {0xFF, 5, {0xFF, 0x98, 0x06, 0x04, 0x07}},
    {0x17, 1, {0x22, 0x00}},  /* Default */
    {0x18, 1, {0x1D, 0x00}},  /* Default */
    {0x02, 1, {0x77, 0x00}},
    {0xE1, 1, {0x79, 0x00}},
    {0x06, 1, {0x13, 0x00}},

    /* Command Set 11: Extended Function Control */
    {0xFF, 5, {0xFF, 0x98, 0x06, 0x04, 0x00}},

    /* Command Set 12: Display Control */
    {0x35, 0, {0x00}},        /* TE On */
    {0x36, 1, {0x00, 0x00}},  /* Memory Access Control */
    {0x3A, 1, {0x50, 0x00}},  /* Pixel Format Set */
    {0x34, 1, {0x01, 0x00}},  /* Tearing Effect Line ON */
    {0x11, 0, {0x00}},        /* Sleep Out */
    {0x29, 0, {0x00}},        /* Display On */
};

#define LCD_INIT_COMMANDS_COUNT sizeof(ili9806e_init_commands) / sizeof((ili9806e_init_commands)[0])

struct ili9806e_init_cmd {
	uint8_t reg;
	uint8_t cmd_len;
	uint8_t cmd[5];
};

static const struct ili9806e_init_cmd init_cmds[] = {
	/* Change to Page 1 CMD */
	{.reg = 0xff, .cmd_len = 5, .cmd = {0xFF, 0x98, 0x06, 0x04, 0x01}},
	/* Output SDA */
	{.reg = 0x08, .cmd_len = 1, .cmd = {0x10}},
	/* DE = 1 Active */
	{.reg = 0x21, .cmd_len = 1, .cmd = {0x01}},
	/* Resolution setting 480 X 800 */
	{.reg = 0x30, .cmd_len = 1, .cmd = {0x01}},
	/* Inversion setting */
	{.reg = 0x31, .cmd_len = 1, .cmd = {0x00}},
	/* BT 15 */
	{.reg = 0x40, .cmd_len = 1, .cmd = {0x14}},
	/* avdd +5.2v,avee-5.2v */
	{.reg = 0x41, .cmd_len = 1, .cmd = {0x33}},
	/* VGL=DDVDL+VCL-VCIP,VGH=2DDVDH-DDVDL */
	{.reg = 0x42, .cmd_len = 1, .cmd = {0x02}},
	/* Set VGH clamp level */
	{.reg = 0x43, .cmd_len = 1, .cmd = {0x09}},
	/* Set VGL clamp level */
	{.reg = 0x44, .cmd_len = 1, .cmd = {0x06}},
	/* Set VREG1 */
	{.reg = 0x50, .cmd_len = 1, .cmd = {0x70}},
	/* Set VREG2 */
	{.reg = 0x51, .cmd_len = 1, .cmd = {0x70}},
	/* Flicker MSB */
	{.reg = 0x52, .cmd_len = 1, .cmd = {0x00}},
	/* Flicker LSB */
	{.reg = 0x53, .cmd_len = 1, .cmd = {0x48}},
	/* Timing Adjust */
	{.reg = 0x60, .cmd_len = 1, .cmd = {0x07}},
	{.reg = 0x61, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x62, .cmd_len = 1, .cmd = {0x08}},
	{.reg = 0x63, .cmd_len = 1, .cmd = {0x00}},
	/* Positive Gamma Control 1 */
	{.reg = 0xa0, .cmd_len = 1, .cmd = {0x00}},
	/* Positive Gamma Control 2 */
	{.reg = 0xa1, .cmd_len = 1, .cmd = {0x03}},
	/* Positive Gamma Control 3 */
	{.reg = 0xa2, .cmd_len = 1, .cmd = {0x09}},
	/* Positive Gamma Control 4 */
	{.reg = 0xa3, .cmd_len = 1, .cmd = {0x0d}},
	/* Positive Gamma Control 5 */
	{.reg = 0xa4, .cmd_len = 1, .cmd = {0x06}},
	/* Positive Gamma Control 6 */
	{.reg = 0xa5, .cmd_len = 1, .cmd = {0x16}},
	/* Positive Gamma Control 7 */
	{.reg = 0xa6, .cmd_len = 1, .cmd = {0x09}},
	/* Positive Gamma Control 8 */
	{.reg = 0xa7, .cmd_len = 1, .cmd = {0x08}},
	/* Positive Gamma Control 9 */
	{.reg = 0xa8, .cmd_len = 1, .cmd = {0x03}},
	/* Positive Gamma Control 10 */
	{.reg = 0xa9, .cmd_len = 1, .cmd = {0x07}},
	/* Positive Gamma Control 11 */
	{.reg = 0xaa, .cmd_len = 1, .cmd = {0x06}},
	/* Positive Gamma Control 12 */
	{.reg = 0xab, .cmd_len = 1, .cmd = {0x05}},
	/* Positive Gamma Control 13 */
	{.reg = 0xac, .cmd_len = 1, .cmd = {0x0d}},
	/* Positive Gamma Control 14 */
	{.reg = 0xad, .cmd_len = 1, .cmd = {0x2c}},
	/* Positive Gamma Control 15 */
	{.reg = 0xae, .cmd_len = 1, .cmd = {0x26}},
	/* Positive Gamma Control 16 */
	{.reg = 0xaf, .cmd_len = 1, .cmd = {0x00}},
	/* Negative Gamma Correction 1 */
	{.reg = 0xc0, .cmd_len = 1, .cmd = {0x00}},
	/* Negative Gamma Correction 2 */
	{.reg = 0xc1, .cmd_len = 1, .cmd = {0x04}},
	/* Negative Gamma Correction 3 */
	{.reg = 0xc2, .cmd_len = 1, .cmd = {0x0b}},
	/* Negative Gamma Correction 4 */
	{.reg = 0xc3, .cmd_len = 1, .cmd = {0x0f}},
	/* Negative Gamma Correction 5 */
	{.reg = 0xc4, .cmd_len = 1, .cmd = {0x09}},
	/* Negative Gamma Correction 6 */
	{.reg = 0xc5, .cmd_len = 1, .cmd = {0x18}},
	/* Negative Gamma Correction 7 */
	{.reg = 0xc6, .cmd_len = 1, .cmd = {0x07}},
	/* Negative Gamma Correction 8 */
	{.reg = 0xc7, .cmd_len = 1, .cmd = {0x08}},
	/* Negative Gamma Correction 9 */
	{.reg = 0xc8, .cmd_len = 1, .cmd = {0x05}},
	/* Negative Gamma Correction 10 */
	{.reg = 0xc9, .cmd_len = 1, .cmd = {0x09}},
	/* Negative Gamma Correction 11 */
	{.reg = 0xca, .cmd_len = 1, .cmd = {0x07}},
	/* Negative Gamma Correction 12 */
	{.reg = 0xcb, .cmd_len = 1, .cmd = {0x05}},
	/* Negative Gamma Correction 13 */
	{.reg = 0xcc, .cmd_len = 1, .cmd = {0x0c}},
	/* Negative Gamma Correction 14 */
	{.reg = 0xcd, .cmd_len = 1, .cmd = {0x2d}},
	/* Negative Gamma Correction 15 */
	{.reg = 0xce, .cmd_len = 1, .cmd = {0x28}},
	/* Negative Gamma Correction 16 */
	{.reg = 0xcf, .cmd_len = 1, .cmd = {0x00}},

	/* Change to Page 6 CMD for GIP timing */
	{.reg = 0xff, .cmd_len = 5, .cmd = {0xFF, 0x98, 0x06, 0x04, 0x06}},
	/* GIP Control 1 */
	{.reg = 0x00, .cmd_len = 1, .cmd = {0x21}},
	{.reg = 0x01, .cmd_len = 1, .cmd = {0x09}},
	{.reg = 0x02, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x03, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x04, .cmd_len = 1, .cmd = {0x01}},
	{.reg = 0x05, .cmd_len = 1, .cmd = {0x01}},
	{.reg = 0x06, .cmd_len = 1, .cmd = {0x80}},
	{.reg = 0x07, .cmd_len = 1, .cmd = {0x05}},
	{.reg = 0x08, .cmd_len = 1, .cmd = {0x02}},
	{.reg = 0x09, .cmd_len = 1, .cmd = {0x80}},
	{.reg = 0x0a, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x0b, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x0c, .cmd_len = 1, .cmd = {0x0a}},
	{.reg = 0x0d, .cmd_len = 1, .cmd = {0x0a}},
	{.reg = 0x0e, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x0f, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x10, .cmd_len = 1, .cmd = {0xe0}},
	{.reg = 0x11, .cmd_len = 1, .cmd = {0xe4}},
	{.reg = 0x12, .cmd_len = 1, .cmd = {0x04}},
	{.reg = 0x13, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x14, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x15, .cmd_len = 1, .cmd = {0xc0}},
	{.reg = 0x16, .cmd_len = 1, .cmd = {0x08}},
	{.reg = 0x17, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x18, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x19, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x1a, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x1b, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x1c, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x1d, .cmd_len = 1, .cmd = {0x00}},
	/* GIP Control 2 */
	{.reg = 0x20, .cmd_len = 1, .cmd = {0x01}},
	{.reg = 0x21, .cmd_len = 1, .cmd = {0x23}},
	{.reg = 0x22, .cmd_len = 1, .cmd = {0x45}},
	{.reg = 0x23, .cmd_len = 1, .cmd = {0x67}},
	{.reg = 0x24, .cmd_len = 1, .cmd = {0x01}},
	{.reg = 0x25, .cmd_len = 1, .cmd = {0x23}},
	{.reg = 0x26, .cmd_len = 1, .cmd = {0x45}},
	{.reg = 0x27, .cmd_len = 1, .cmd = {0x67}},
	/* GIP Control 3 */
	{.reg = 0x30, .cmd_len = 1, .cmd = {0x01}},
	{.reg = 0x31, .cmd_len = 1, .cmd = {0x11}},
	{.reg = 0x32, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x33, .cmd_len = 1, .cmd = {0xee}},
	{.reg = 0x34, .cmd_len = 1, .cmd = {0xff}},
	{.reg = 0x35, .cmd_len = 1, .cmd = {0xcb}},
	{.reg = 0x36, .cmd_len = 1, .cmd = {0xda}},
	{.reg = 0x37, .cmd_len = 1, .cmd = {0xad}},
	{.reg = 0x38, .cmd_len = 1, .cmd = {0xbc}},
	{.reg = 0x39, .cmd_len = 1, .cmd = {0x76}},
	{.reg = 0x3a, .cmd_len = 1, .cmd = {0x67}},
	{.reg = 0x3b, .cmd_len = 1, .cmd = {0x22}},
	{.reg = 0x3c, .cmd_len = 1, .cmd = {0x22}},
	{.reg = 0x3d, .cmd_len = 1, .cmd = {0x22}},
	{.reg = 0x3e, .cmd_len = 1, .cmd = {0x22}},
	{.reg = 0x3f, .cmd_len = 1, .cmd = {0x22}},
	{.reg = 0x40, .cmd_len = 1, .cmd = {0x22}},
	/* GOUT VGLO Control */
	{.reg = 0x53, .cmd_len = 1, .cmd = {0x10}},
	{.reg = 0x54, .cmd_len = 1, .cmd = {0x10}},
	/* Change to Page 7 CMD for Normal command */
	{.reg = 0xff, .cmd_len = 5, .cmd = {0xff, 0x98, 0x06, 0x04, 0x07}},
	/* VREG1/2OUT ENABLE */
	{.reg = 0x18, .cmd_len = 1, .cmd = {0x1d}},
	{.reg = 0x26, .cmd_len = 1, .cmd = {0xb2}},
	{.reg = 0x02, .cmd_len = 1, .cmd = {0x77}},
	{.reg = 0xe1, .cmd_len = 1, .cmd = {0x79}},
	{.reg = 0x17, .cmd_len = 1, .cmd = {0x22}},
	/* Change to Page 0 CMD for Normal command */
	{.reg = 0x3A, .cmd_len = 5, .cmd = {0xff, 0x98, 0x06, 0x04, 0x00}},
	{.reg = 0x34, .cmd_len = 1, .cmd = {0x01}},
	{.reg = 0x11, .cmd_len = 1, .cmd = {0x00}},
	{.reg = 0x29, .cmd_len = 1, .cmd = {0x00}}};


/*
 * Display Control Functions
 *
 * These functions provide a clean interface for LCD panel control.
 * They should be called in the proper sequence during system initialization.
 */

/**
 * @brief Initialize the LCD panel via ILI9806E commands
 *
 * Sends all required initialization commands to configure the ILI9806E panel.
 * Must be called before any display operations.
 */
void lcd_panel_init(void);

/**
 * @brief Example complete panel initialization sequence
 *
 * This function demonstrates the complete initialization sequence
 * including reset, sleep out, command sending, and display enable.
 *
 * @note This is an example implementation. Replace the function calls
 *       with your platform's actual implementation.
 */


//void lcd_complete_init_sequence(void)
//{
    /* 1. Hardware reset sequence */
    //lcd_reset_low();
    //delay_ms(10);
    //lcd_reset_high();
    //delay_ms(120);

    /* 2. Send initialization commands */
    //lcd_send_init_commands();

    /* 3. Exit sleep mode */
    //lcd_sleep_out();
    //delay_ms(120);

    /* 4. Enable display */
    //lcd_display_on();
    //delay_ms(10);

    /* 5. Enable tear effect if needed */
    //lcd_enable_tear_effect();
    //delay_ms(10);
//}

/*
 * Panel Information Constants
 */
#define LCD_PANEL_MODEL           "E43GB-I-MW800-CB"
#define LCD_PANEL_VENDOR          "Focus LCDs"
#define LCD_PANEL_CONTROLLER      "ILI9806E"
#define LCD_PANEL_TYPE            "IPS"
#define LCD_PANEL_WIDTH           480
#define LCD_PANEL_HEIGHT          800
#define LCD_PANEL_PIXELS          (LCD_PANEL_WIDTH * LCD_PANEL_HEIGHT)
#define LCD_PANEL_ASPECT_RATIO    "3:5"
#define LCD_PANEL_INTERFACE       "MIPI DSI 2-lane"
#define LCD_PANEL_CLOCK_SPEED     15000000   /* 15 MHz */
#define LCD_PANEL_DIAGONAL_SIZE   4.3        /* inches */
#define LCD_PANEL_MODULE_WIDTH    56         /* mm */
#define LCD_PANEL_MODULE_HEIGHT   93         /* mm */
#define LCD_PANEL_VIEWING_WIDTH   56         /* mm (assuming full viewing area) */
#define LCD_PANEL_VIEWING_HEIGHT  93         /* mm (assuming full viewing area) */
#define LCD_PANEL_BRIGHTNESS      800        /* Nits (typical) */
#define LCD_PANEL_TFT_CONTROLLER  "ILI9806E"
#define LCD_PANEL_TOUCH_CONTROLLER "GT911"
#define LCD_PANEL_TOUCH_TYPE      "Capacitive"
#define LCD_PANEL_POLARIZER       "Transmissive"
#define LCD_PANEL_TEMP_MIN        -20        /* °C */
#define LCD_PANEL_TEMP_MAX        70         /* °C */
#define LCD_PANEL_VIDEO_MODE      "Burst"    /* Video mode: burst mode */
#define LCD_PANEL_DSI_FLAGS       0x10c      /* DSI flags */

void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams);
int32_t DSI_IO_ReadCmd(uint32_t Reg, uint8_t *pData, uint32_t Size);
uint8_t E43GBIMW800CB_Init();
uint8_t E43GBIMW800CB_DeInit(void);
void E43GBIMW800CB_IO_Delay(uint32_t Delay);
uint16_t E43GBIMW800CB_ReadID(void);

/*
 * Contact Information
 *
 * Focus LCDs
 * 162 W. Boxelder, Suite 1
 * Chandler, AZ 85225
 * M-F 7:30am to 4:00pm (Arizona Time)
 *
 * Phone: 480-503-4295
 * Email: LCD@FocusLCDs.com
 */

#endif /* BSP_E43GBIMW800CB_E43GBIMW800CB_H_ */
