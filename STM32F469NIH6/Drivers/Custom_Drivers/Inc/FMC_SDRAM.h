/*
 * FMC_SDRAM.h
 *
 *  Created on: Dec 22, 2025
 *      Author: jackh
 */

#ifndef INC_FMC_SDRAM_H_
#define INC_FMC_SDRAM_H_

#include <stdint.h>

#define NUM_COLUMN_BITS 	0		// 0: 8 bits	1: 9 bits	2: 10 bits	3: 11 bits
#define NUM_ROW_BITS    	1		// 0: 11 bits 	1: 12bits	2: 13bits
#define MEMORY_WIDTH		2		// 0: 8bits 	1: 16bits 	2: 32bits
#define NUM_INTERNAL_BANKS	1		// 0: 2 banks	1: 4 banks
#define CAS_LATENCY			3		// CAS latency 1 to 3 cycles supported
#define MEM_FREQ_DIV		2		// 2: HCLK/2	3: HCLK/3
#define READ_BURST_ENABLE	1		// 0: False		1: True
#define READ_PIPE_DELAY		2		// extra waitstates added to CAS_LATENCY to account for non idealities (clock skew, routing etc)

#define TRC_DELAY			6		// Row cycle delay						0 -> 1cycle ... 15 -> 16cycles
#define TRP_DELAY			1		// Row precharge delay					0 -> 1cycle ... 15 -> 16cycles
#define TWR_DELAY			1		// Recovery delay						0 -> 1cycle ... 15 -> 16cycles
#define TRCD_DELAY			1		// Row to column delay					0 -> 1cycle ... 15 -> 16cycles
#define TRAS_DELAY			3		// Self refresh time					0 -> 1cycle ... 15 -> 16cycles
#define TXSR_DELAY			6		// Exit self-refresh delay				0 -> 1cycle ... 15 -> 16cycles
#define TMRD_DELAY			1		// load mode register to active delay	0 -> 1cycle ... 15 -> 16cycles

#define REFRESH_COUNT		0x0569	// TCLKS per refresh

#define FMC_SDRAM___CMD_CLK_ENABLE		1
#define FMC_SDRAM___CMD_AUTOREFRESH		3
#define FMC_SDRAM___CMD_PALL			2
#define FMC_SDRAM___CMD_LOAD_MODE		4
#define FMC_SDRAM___CMD_TARGET_BANK2	1
#define FMC_SDRAM___CMD_TARGET_BANK1	2
#define FMC_SDRAM___CMD_TARGET_BANK1_2	3

#define FMC_SDRAM___MODEREG_BURST_LENGTH	          		0		// 0: 1		1: 2	2: 4	3: 8	7: Full Page (only if burst type = 0)
#define FMC_SDRAM___MODEREG_BURST_LENGTH_Pos				0		// Burst length value position
#define FMC_SDRAM___MODEREG_BURST_TYPE				   		0		// 0: Sequential	1: Interleaved
#define FMC_SDRAM___MODEREG_BURST_TYPE_Pos					3		// Burst type value position
#define FMC_SDRAM___MODEREG_CAS_LATENCY		           		0x3		// 2: 2 CLKS		3: 3 CLKS
#define FMC_SDRAM___MODEREG_CAS_LATENCY_Pos					4		// CAS latency value position
#define FMC_SDRAM___MODEREG_OPERATING_MODE 					0		// 0 (only option): standard operation
#define FMC_SDRAM___MODEREG_OPERATING_MODE_Pos				7		// Operating mode value position
#define FMC_SDRAM___MODEREG_WRITEBURST_MODE					0x1		// 0: programmed burst length		1: Single location access
#define FMC_SDRAM___MODEREG_WRITEBURST_MODE_Pos				9		// write burst mode value position


typedef struct FMC_Command_Struct
{
	uint16_t CommandMode;
	uint16_t CommandTarget;
	uint16_t AutoRefreshNumber;
	uint16_t ModeRegisterDefinition;
} FMC_SDRAM___Command_TypeDef;

void FMC_SDRAM___SDRAM_Init(void);

#endif /* INC_FMC_SDRAM_H_ */
