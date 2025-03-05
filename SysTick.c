// SysTick.c
// Runs on TM4C123

#include <stdint.h>
#include "SysTick.h"
#include "tm4c123gh6pm.h"

#define ONE_SECOND  4000000 // 16MHz Clock

// Time delay using busy wait.
// This function assumes 16 MHz system clock.
void SysTick_Wait(uint8_t delay){
	for (int i = 0; i < delay; i++)
	{	
		NVIC_ST_CTRL_R = 0;
		NVIC_ST_RELOAD_R = ONE_SECOND-1; // number of counts to wait
		NVIC_ST_CURRENT_R = 0; // any value written to CURRENT clears
		NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
		while((NVIC_ST_CTRL_R&NVIC_ST_CTRL_COUNT)==0); // wait for count flag
		NVIC_ST_CTRL_R = 0;
	}
}
