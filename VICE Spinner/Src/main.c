

#include <stdint.h>
#include <stdlib.h>
#include <stm32f1xx.h>										// include MCU specific definitions
#include <VICE_USB_Driver.h>

// TMC2130 Registers
#define IHOLD_IRUN_REG_ADDRESS 				0x10u
#define GCONF_REG_ADDRESS					0x00u
#define GSTAT_REG_ADDRESS					0x01u
#define IOIN_REG_ADDRESS					0x04u
#define TSTEP_REG_ADDRESS					0x12u
#define TPWMTHRS_REG_ADDRESS				0x13u
#define TCOOLTHRS_REG_ADDRESS				0x14u
#define THIGH_REG_ADDRESS					0x15u
#define XDIRECT_REG_ADDRESS					0x2Du
#define VDCMIN_REG_ADDRESS					0x33u
#define CHOPCONF_REG_ADDRESS				0x6Cu
#define COOLCONF_REG_ADDRESS				0x6Du
#define DCCTRL_REG_ADDRESS					0x6Eu
#define DRV_STATUS_REG_ADDRESS				0x6Fu
#define PWMCONF_REG_ADDRESS					0x70u
#define PWM_SCALE_REG_ADDRESS				0x71u
#define LOST_STEPS_REG_ADDRESS				0x73u
// -----------------------

// TMC2130 Configuration
#define toff					0x08u																				// stepper toff value
#define RunCurrent				0x0bu 																				// stepper run current (0-31)
#define HoldCurrent				0x06u 																				// stepper hold current (0-31)
// -----------------------

// program Configuration
#define ADC_Buffer_Length 		0x64																				// controls how many ADC samples will be averaged
// -----------------------

#define DEG_To_Steps			0.35555f

// Program Variables

uint8_t 	SPI_Receive_Buffer[5] = {};
uint8_t 	SPI_Receive_Buffer_Index = 0;
int32_t 	absolute_Position = 0;
uint32_t 	step_Remainder = 0;
uint8_t 	direction = 1;
uint8_t 	mode = 0;
uint16_t 	Slider_Value = 0;
uint8_t 	ADC_Buffer_Index = 0;
uint16_t 	ADC_Buffer[ADC_Buffer_Length] = {2048};

// VICE Variables

USB_VICE_Device_Typedef USB_VICE_Device;

// -----------------------



void Clock_Init(){	// initialize clock ()
	RCC -> CR |= RCC_CR_HSEON;																						// enable HSE Oscillator
	while(!(RCC -> CR & RCC_CR_HSERDY));																			// wait for HSE To Stabilize
	FLASH -> ACR |= FLASH_ACR_LATENCY_2;																			// set flash wait to 2, because 48 MHz < SYSCLK  72 MHz
	RCC -> CFGR &= ~(RCC_CFGR_USBPRE);																				// set USB Pre-scaler to /1.5
	RCC -> CFGR |= 	(RCC_CFGR_PLLXTPRE		) |																		// set PLL Entry Pre-scaler to /2
					(RCC_CFGR_PLLMULL9		) |																		// set PLL Multiplication Factor to *9
					(RCC_CFGR_PLLSRC		) |																		// select HSE as the PLL Source
					(RCC_CFGR_ADCPRE_DIV6	) |																		// set ADC Pre-scaler to /6
					(RCC_CFGR_PPRE1_DIV2	);																		// set APB1 Pre-Scaler to /2
	RCC -> CR |= (RCC_CR_PLLON);																					// enable PLL
	while(!(RCC -> CR & RCC_CR_PLLRDY));																			// wait for PLL To Stabilize
	RCC -> CFGR |= (RCC_CFGR_SW_PLL);																				// switch PLL on
	while(!(RCC -> CFGR & RCC_CFGR_SWS_PLL));																		// Wait for source to switch to PLL
	RCC -> CR &= ~(1<<0);																							// Turn off HSI
}

void TIM1_Init(){
	RCC -> APB2ENR 	|= (RCC_APB2ENR_TIM1EN);																			// Enable Timer 1 Clock
	TIM1 -> CCMR1 	|= (6<<4);																						// Set Capture-Compare Mode to PWM1
	TIM1 -> CCER 	|= (TIM_CCER_CC1E);																				// Enable Capture-Compare Channel 1 Output to Pin
	TIM1 -> BDTR 	|= (1<<15);																						// Enable Output Through Break and Dead Time Register
	TIM1 -> ARR 	 = 2880-1;																							// Set ARR to generate 25Khz PWM (72000000/2880 = 25000)
	TIM1 -> CCR1 	 = 1440-1;																							// Set Capture-Compare Register to default fan speed
	TIM1 -> CR1 	|= (1<<0);																							// Enable the counter to begin PWM output
}

void TIM2_Init(){
	RCC -> APB1ENR |= (RCC_APB1ENR_TIM2EN);																			// Enable Timer 2 Clock
	TIM2 -> CR2 |= (3 << 4);																						// Set Master Mode to Send TRGO when CCR1 matches CNT
	TIM2 -> CCMR1 |= (3 << 4);																						// Set Capture-Compare Mode to Toggle on Match
	TIM2 -> CCER |= (1 << 0);																						// Enable Capture-Compare Channel 1 Output to Pin
	TIM2 -> PSC = 8-1;																								// Set Pre-scaler to 8
}

void TIM3_Init(){
	RCC -> APB1ENR |= (RCC_APB1ENR_TIM3EN);																			// Enable Timer 3 Clock
	TIM3->CR2 |= (2<<4);																							// Set Master Mode to Send TRGO when CNT = ARR
	TIM3->PSC = 72-1;																								// Set Pre-Scaler Register so (1CNT = 1us)
	TIM3->ARR = 1000;																								// set auto reload register 1ms = 1000us
	TIM3->CR1 |= (1<<0);																							// enable timer
	TIM3->SR = 0;																									// Clear Any Status Flags
}

void TIM4_Init(){
	NVIC_SetPriority (TIM4_IRQn, 0);																				// Set Interrupt Priority
	NVIC_EnableIRQ (TIM4_IRQn);																						// Enable Interrupt
	RCC -> APB1ENR |= (RCC_APB1ENR_TIM4EN);																			// Enable Timer 4 Clock
	TIM4 -> SMCR |= ((7 << 0) | (1 << 4));																			// Turn on Slave-Mode, and Select ITR1(TIM2) as Master
	TIM4 -> DIER |= (1 << 1);																						// Enable Interrupt on CC1 (when CNT = CCR1)
	TIM2 -> CCMR1 |= (1 << 4);																						// Set capture-compare mode to active on match
	TIM4-> SR = 0;																									// Clear Any Status Flags
}
/*
void EXTI4_Init(){
	NVIC_SetPriority (EXTI4_IRQn, 0);																				// Set Interrupt Priority
	NVIC_EnableIRQ (EXTI4_IRQn);																					// Enable Interrupt
	EXTI -> IMR |= (1<<4);																							// Enable EXTI interrupt on PA4
	EXTI -> FTSR |= (1<<4);																							// Set falling edge trigger on PA4
}*/

void GPIO_Init(){
	RCC -> APB2ENR |= (RCC_APB2ENR_IOPAEN);																			// Enable GPIOA Clock
	RCC -> APB2ENR |= (RCC_APB2ENR_AFIOEN);																			// Enable Alternate-Function Clock

	// TIM1CH1 	- PWM_Fan

	GPIOA -> CRH &= ~((3<<0) | (3<<2));																				// clear CFG and Mode for PA8 (TIM1CH1, PWM_Fan)
	GPIOA -> CRH |= ((2<<0) | (3<<2));																				// set PA8 (TIM1CH1, PWM_Fan) to output mode max 2Mhz, alternate function mode open drain

	// SPI1 	- TMC_SPI

	GPIOA -> CRL &= ~((3<<8) | (3<<10));																			// clear CFG and Mode for PA2 (SPI1 CSS, TMC_SPI)
	GPIOA -> CRL |= ((1<<8) | (0<<10));																				// set PA2 (SPI1 CSS, TMC_SPI) to output mode max 10Mhz, mode push pull
	GPIOA -> ODR |= (1<<2);

	GPIOA -> CRL &= ~((3<<20) | (3<<22));																			// clear CFG and Mode for PA5 (SPI1 SCK, TMC_SPI)
	GPIOA -> CRL |= ((1<<20) | (2<<22));																			// set PA5 (SPI1 SCK, TMC_SPI) to output mode max 10Mhz, alternate function mode push pull

	GPIOA -> CRL &= ~((3<<24) | (3<<26));																			// clear CFG and Mode for PA6 (SPI1 MISO, TMC_SPI)
	GPIOA -> CRL |= ((1<<24) | (2<<26));																			// set PA6 (SPI1 MISO, TMC_SPI) to output mode max 10Mhz, alternate function mode push pull

	GPIOA -> CRL &= ~((3<<28) | (3<<30));																			// clear CFG and Mode for PA7 (SPI1 MOSI, TMC_SPI)
	GPIOA -> CRL |= ((1<<28) | (2<<30));																			// set PA7 (SPI1 MOSI, TMC_SPI) to output mode max 10Mhz, alternate function mode push pull

	// TIM2CH1 	- TMC_Interface

	GPIOA -> CRL &= ~((3<<0) | (3<<2));																				// clear CFG and Mode for PA0 (TIM2 CH1, TMC_Interface)
	GPIOA -> CRL |= ((1<<0) | (2<<2));																				// set PA0 (TIM2 CH1, TMC_Interface) to output mode max 10Mhz, alternate function mode push pull

	GPIOA -> CRL &= ~((3<<4) | (3<<6));																				// clear CFG and Mode for PA1 (Direction, TMC_Interface)
	GPIOA -> CRL |= ((1<<4) | (0<<6));																				// set PA1 (Direction, TMC_Interface) to output mode max 10Mhz, mode push pull
	GPIOA -> ODR |= (1<<1);

	// ADC2		- Slider

	GPIOA -> CRL &= ~((3<<12) | (3<<14));																			// clear CFG and Mode for PA3 (ADC2CH3, Slider)
	GPIOA -> CRL |= ((0<<12) | (0<<14));																			// set PA3 (ADC2CH3, Slider) to Input mode, Analog

	// EXTI4 	- VICE interrupt

	GPIOA -> CRL &= ~((3<<16) | (3<<18));																			// clear CFG and Mode for PA4 (EXTI4, VICE interrupt)
	GPIOA -> CRL |= ((0<<16) | (1<<18));																			// set PA4 (EXTI4, VICE interrupt) to Input mode


}

void SPI1_Init(){
	RCC -> APB2ENR |= (RCC_APB2ENR_SPI1EN);																			// Enable SPI1 Clock
	SPI1 -> CR1 = 0; 																								// clear SPI1 CR1
	SPI1 -> CR1 |= ((4 << 3) |																						// Set Baud Rate Clock Divider to /32 (72000000Mhz/32 = 2.25Mhz)
					(1 << 2) |																						// Enable Master Mode
					(3 << 0));																						// Set SPI Mode to 3 (CPOL = 1, CPHA = 1)
	SPI1 -> CR1 |= (1 << 6);																						// Enable SPI
}

void SPI1_Transmit40(uint8_t pRW, uint8_t pAddress, uint32_t pData){
	SPI_Receive_Buffer_Index = 0;
	uint8_t transmission[5];																						// Create an Array to Hold 40-Bit(4 Byte) Transmission
	transmission[0] = pAddress + (pRW * 0x80);																		// Set the First Byte to The Address + Write Bit
	for(uint8_t i = 0; i<4; i++){																					// loop through the data, adding it to the array one byte at a time
		transmission[4-i] = (pData>>i*8);																			// add the least significant byte of the data to the end of the array first, and then go byte by byte backwards
	}
	GPIOA -> ODR &= ~(1<<2);																						// Set Chip Select LOW
	for(int i = 0; i<5; i++){																						// loop through the data array to transmit each byte
	while(!((SPI1->SR) & (1<<1))){}; 																				// wait for TX buffer to be empty
		SPI1->DR = transmission[i];																					// copy data to TX buffer
	}
	while((SPI1->SR) & (1<<7)); 																					// Wait for SPI to Not Be Busy
	GPIOA -> ODR |= (1<<2);																							// set Chip-Select HIGH
}

void TMC_Init(){
	SPI1_Transmit40(1, GCONF_REG_ADDRESS, (1<<2));																	// write data to GCONF (enables PWM mode)
	SPI1_Transmit40(1, IHOLD_IRUN_REG_ADDRESS, (HoldCurrent | (RunCurrent<<8)));									// set Stepper Hold-Current, and Run-Current
	SPI1_Transmit40(1, CHOPCONF_REG_ADDRESS, ((toff<<0) | (0 << 24) | (1<<29))); 									// write CHOPCONF settings (enables the driver and sets resolution)
}

/*
void ADC2_Init(){
	RCC -> APB2ENR |= (RCC_APB2ENR_ADC2EN);																			// Enable ADC2 Clock
	NVIC_SetPriority (ADC1_2_IRQn, 2);  																			// Set Interrupt Priority
	NVIC_EnableIRQ (ADC1_2_IRQn);																					// Enable Interrupt
	ADC2 -> CR1 |= (1 << 5);																						// Enable Interrupt on End-Of-Conversion
	ADC2 -> SQR3 |= (3 << 0);																						// Select channel 3 as first regular Conversion
	ADC2 -> SMPR2 |= (7 << 9);																						// Set sample time for channel 3 to 239.5 cycles (20us) for stability
	ADC2 -> CR2 |= ((1 << 20) |																						// Enable External Conversion Trigger
					(4 << 17));	 																					// Select TIM 3 TRGO as Conversion Trigger

}*/

void set_Direction(uint8_t pDirection){																				// Function to easily change the state of direction variable as well as state of direction pin PA1
	direction = pDirection;																							// Set variable direction to input direction
	GPIOA -> ODR &= ~(1<<1);																						// Set PA1 Low (CCW)
	GPIOA -> ODR |= (pDirection<<1);																				// Set PA1 to input Direction (CCW or CW)
}

void start_Movement(){																								// This function enables both the timers to begin stepping
	TIM4 -> CR1 |= (1<<0);																							// Enable TIM4 to keep track of steps
	TIM2 -> CR1 |= (1<<0);																							// Enable TIM2 to start stepping
}

void load_Step_Count(uint32_t pDistance){																			// Function to load TIM2 with the appropriate amount of steps to be taken
	if(pDistance == 1){																								// If the amount of steps is 1
		TIM2 -> CR1 |= (1<<3);																						// Set timer 2 to one pulse mode
	}else{																											// Else
		TIM2 -> CR1 &= ~(1<<3);																						// Disable one pulse mode
	}
	if(pDistance > 64000){																							// If amount of steps to be taken is larger than 64000(max number able to be loaded)
		pDistance = 64000;																							// Set distance to 64000
	}
	TIM4 -> CCR1 = pDistance;																						// Load timer 2 with the adjusted ammount of steps
}

void update_Speed(uint32_t pSpeed){																					// Function to update the current speed of the stepper motor (in micro-steps per period(ms))
	TIM2 -> ARR = ((9000000)/pSpeed)-1;																				// Calculate speed ((TIM2 Freq/8) / speed) to get correct ARR value
	TIM2 -> CCR1 = TIM2 -> ARR;																						// CCR1 mirrors ARR
}

/*
uint16_t update_ADC_Buffer(uint16_t pADC_Value){																	// This function is used to add a new value to the ADC buffer for averaging
	ADC_Buffer[ADC_Buffer_Index] = pADC_Value;																		// Add the new value to the buffer
	uint32_t ADC_Sum = 0;																							// create and initialize a variable to add all the ADC values to
	for(uint8_t i = 0; i < ADC_Buffer_Length; i++){																	// Loop through the buffer
		ADC_Sum += ADC_Buffer[i];																					// Add the current value to the ADC sum variable
	}
	ADC_Buffer_Index += ((ADC_Buffer_Index < (ADC_Buffer_Length - 1)) ? (1) : ((-1) * (ADC_Buffer_Length - 1)));	// Update the current index to the next index
	return(ADC_Sum / ADC_Buffer_Length);																			// Return the average of the buffer
}*/

void halt(){
	TIM2 -> CR1 &= ~(1<<0);																							// disable TIM2 to stop step output
	TIM4 -> CR1 &= ~(1<<0);																							// disable TIM5 to reset it
	absolute_Position += ((int32_t)(TIM4 -> CNT)) * (direction ? (1) : (-1));										// Add the steps taken to the current location
	TIM2 -> CNT = 0;																								// Clear Timer 2 CNT
	TIM4 -> CNT = 0;																								// Clear Timer 4 CNt
}

/*
void set_Mode(uint8_t pMode){																						// This function is used to change the mode of the spinner
	halt();																											// Halt motion and record location
	mode = pMode;																									// Update global mode variable with new mode
	switch(pMode){																									// Switch statement to set the default values of each mode after change
	case 0:																											// If mode 0
		ADC2 -> CR2 |= (1 << 0);																					// Turn ADC On
		TIM2 -> CR1 &= ~(1<<3);																						// Turn off one pulse mode
		TIM4 -> CCR1 = 64000;																						// Set TIM4 CCR1 to the same
		break;
	case 1:																											// If mode 1
		ADC2 -> CR2 &= ~(1 << 0);																					// Turn off ADC
		break;
	case 2:																											// If mode 2
		ADC2 -> CR2 &= ~(1 << 0);																					// Turn off ADC
		break;
	}
}*/

void jog(uint32_t pGoal, uint32_t pSpeed){																			// Function to move to a location at a speed (mSteps, mSteps/s)
	halt();																											// Halt motion and record location
	int32_t delta = (pGoal-absolute_Position);																		// Calculate the difference between the requested location and current location
	uint32_t deltaA = abs(delta);																					// Get the distance not including the direction
	set_Direction(!(delta<0));																						// Set the direction needed to get to a given location
	if(deltaA > 0){																									// If the goal is not the current position
		update_Speed(pSpeed);																						// Update the speed with the current
		step_Remainder = deltaA;																					// Set the global step remainder variable to the distance needed to reach the goal
		load_Step_Count(step_Remainder);																			// Load Tim2 with number of steps needed
		start_Movement();																							// Activate the timers and begin motion
	}
}

void schedule_Arrival(int32_t pGoal, uint32_t pTimeMS){																// Function to schedule the arrival at a location and time (mSteps, ms from call)
	halt();																											// Halt motion and record location
	int32_t delta = (pGoal-absolute_Position);																		// Calculate the difference between the requested location and current location
	uint32_t deltaA = abs(delta);																					// Get the distance not including the direction
	set_Direction(!(delta<0));																						// Set the direction needed to get to a given location
	if(deltaA > 0){																									// If the goal is not the current position
		update_Speed((deltaA*1000)/(pTimeMS));																		// Update the speed needed to reach the location at the time specified
		step_Remainder = deltaA;																					// Set the global step remainder variable to the distance needed to reach the goal
		load_Step_Count(step_Remainder);																			// Load Tim2 with number of steps needed
		start_Movement();																							// Activate the timers and begin motion
	}
}

void go_To_Next_Frame(int32_t pGoal, uint32_t pFPS){																// Function to schedule the arrival at a location and time (mSteps, ms from call)
	halt();																											// Halt motion and record location
	int32_t delta = (pGoal-absolute_Position);																		// Calculate the difference between the requested location and current location
	uint32_t deltaA = abs(delta);																					// Get the distance not including the direction
	set_Direction(!(delta<0));																						// Set the direction needed to get to a given location
	if(deltaA > 0){																									// If the goal is not the current position
		update_Speed((deltaA)*(pFPS));																				// Update the speed needed to reach the location at the time specified
		step_Remainder = deltaA;																					// Set the global step remainder variable to the distance needed to reach the goal
		load_Step_Count(step_Remainder);																			// Load Tim2 with number of steps needed
		start_Movement();																							// Activate the timers and begin motion
	}
}

void Zero(){
	halt();																											// Halt motion and record location
	absolute_Position = 0;																							// Reset the position to zero
}
/*
void ADC_Buffer_Init(){																								// This function fills the ADC buffer with 2048 so that the slider starts in the centered position
	for(uint8_t i = 0; i < ADC_Buffer_Length; i++){																	// Loop through the entire Array
		ADC_Buffer[i] = 2048;																						// Set the value of the current index to 2048
	}
}*/

void VICE_Callback(uint8_t* data, uint16_t length)
{
	int32_t location = ((int32_t*)data)[1];
	location = location * DEG_To_Steps;
	go_To_Next_Frame(location, USB_VICE_Device.frame_Rate);
}

int main(void){
	for(uint32_t i = 0; i < 0xffff; i++);
	Clock_Init();
	GPIO_Init();
	TIM1_Init();
	SPI1_Init();
	TMC_Init();
	TIM2_Init();
	TIM4_Init();
	TIM3_Init();
	EXTI4_Init();
	USB_VICE_Init(&USB_VICE_Device, VICE_Callback);
	set_Mode(0);
 	for(;;){

 	}
}
/*
void ADC1_2_IRQHandler(void){																						// This function is called every time a ADC conversion has been completed, and turns it into a centered 16 bit number
	if(ADC2 -> SR & (1 << 1)){																						// If interrupt was from end of conversion
		ADC2 -> SR &= ~(1 << 1);																					// clear the ADC Status Register
		uint16_t raw_Slider_Value = update_ADC_Buffer(ADC2 -> DR);													// Create a variable containing the averaged ADC buffer value
		uint8_t polarity = raw_Slider_Value >>11;																	// Take the polarity from the averaged buffer value
		uint16_t position = (raw_Slider_Value & 0x7ff);																// take the slider position from the averaged buffer value
		Slider_Value = (polarity) ? (position) : (2048 - position);													// add the position to the global slider variable, if the slider is left of center, invert the position
		Slider_Value |= (polarity << 15);																			// add the polarity to the global slider variable
		if(mode == 0){//stand-alone																					// if mode is 0 (stand-alone)
			if((Slider_Value & 0xfff)>35){																			// if value is above 35 (dead-zone)
				if(direction == (Slider_Value >> 15)){																// if direction needs to be changed
					halt();																							// Halt motion and record position
					set_Direction(!(Slider_Value >> 15));															// set the direction to the polarity of the slider
				}
				TIM4 -> CR1 |= (1<<0);																				// make sure the Step timers are counting
				TIM2 -> CR1 |= (1<<0);																				// make sure the Step timers are counting
				update_Speed((Slider_Value & 0xfff) * 4);															// update the speed to the slider position
			}else{																									// if value is below 35 (dead-zone)
				halt();																								// halt motion
			}
		}
	}
}*/

void EXTI4_IRQHandler(void){
	EXTI -> PR |= (1 << 4);
}

void TIM4_IRQHandler(void){
	if((TIM4 -> SR) & (1<<1)){																						// If Interrupt was from CC1
		TIM4-> SR = 0;																								// clear flags
		TIM2 -> CR1 &= ~(1<<0);																						// disable TIM2 to stop step output
		TIM4 -> CR1 &= ~(1<<0);																						// disable TIM5 to reset it
		step_Remainder -= (TIM4 -> CNT);																			// Subtract steps taken from step remainder
		absolute_Position += ((int32_t)(TIM4 -> CNT)) * (direction ? (1) : (-1));									// Add steps taken to the new absolute position
		if(step_Remainder > 0){																						// if there are steps remaining
			load_Step_Count(step_Remainder);																		// load steps remaining
			start_Movement();																						// Enable timers and begin motion
		}
		TIM2 -> CNT = 0;																							// Clear Timer 2 CNT
		TIM4 -> CNT = 0;																							// Clear Timer 4 CNt
	}
}
