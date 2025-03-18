#include "SW_LED.h"
#include "tm4c123gh6pm.h"

// Initilize port F and arm PF4, PF0 for falling edge interrupts
void SW_LED_Init(void){  
	unsigned long volatile delay;
  SYSCTL_RCGC2_R 			|= SYSCTL_RCGC2_GPIOF; 	// 1) activate clock for port F
  delay 							 = SYSCTL_RCGC2_R;
	
  GPIO_PORTF_LOCK_R  	 = GPIO_LOCK_KEY; 			// 2) unlock GPIO Port F
  GPIO_PORTF_CR_R 		|= SWLED_MASK;         	// 3) allow changes to PF0-4
  GPIO_PORTF_DIR_R 		&= ~SW_MASK;    				// 4) make PF0,4 in (built-in button)
  GPIO_PORTF_DIR_R 		|= LED_MASK;     				// 5) make PF1-3 out
  GPIO_PORTF_AFSEL_R 	&= ~SWLED_MASK;  				// 6) disable alt funct on PF0-4
  GPIO_PORTF_DEN_R 		|= SWLED_MASK;     			// 7) enable digital I/O on PF0-4
  GPIO_PORTF_PCTL_R 	&= ~0x000FFFFF; 				// 8) configure PF0-4 as GPIO
  GPIO_PORTF_AMSEL_R 	&= ~SWLED_MASK;  				// 9) disable analog functionality on PF0-4
  GPIO_PORTF_PUR_R 		|= SW_MASK;     				// 10) enable weak pull-up on PF4,0
  GPIO_PORTF_IS_R 		&= ~SW_MASK;     				// 11) PF4,PF0 is edge-sensitive
  GPIO_PORTF_IBE_R 		&= ~SW_MASK;    				// 12) PF4,PF0 is not both edges
  GPIO_PORTF_IEV_R 		|= SW_MASK;    					// 13) PF4,PF0 rising edge event
  GPIO_PORTF_ICR_R 		 = SW_MASK;      				// 14) clear flags 4,0
  GPIO_PORTF_IM_R 		|= SW_MASK;      				// 15) arm interrupt on PF4,PF0
	
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF1FFFFF)|			// 16) bits:23-21 for PORTF, set priority to 5
								0x00400000; 
  NVIC_EN0_R |= 0x40000000;      							// 17) enable interrupt 30 in NVIC
}

// Initilize port F and arm PF4, PF0 for falling edge interrupts
//void SwitchLED_Init(void){  
//	unsigned long volatile delay;
//  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // (a) activate clock for port F
//  delay = SYSCTL_RCGC2_R;
//  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY; 		// unlock GPIO Port F
//  GPIO_PORTF_CR_R |= 0x1F;         			// allow changes to PF4,0
//  GPIO_PORTF_DIR_R &= ~0x11;    				// (c) make PF4,0 in (built-in button)
//  GPIO_PORTF_DIR_R |= 0x0E;     				// make PF1-3 out
//  GPIO_PORTF_AFSEL_R &= ~0x11;  				//     disable alt funct on PF4,0
//  GPIO_PORTF_DEN_R |= 0x1F;     				//     enable digital I/O on PF4,0
//  GPIO_PORTF_PCTL_R &= ~0x000FFFFF; 		//  configure PF4,0 as GPIO
//  GPIO_PORTF_AMSEL_R &= ~0x1F;  				//     disable analog functionality on PF4,0
//  GPIO_PORTF_PUR_R |= 0x11;     				//     enable weak pull-up on PF4,0
//  GPIO_PORTF_IS_R &= ~0x11;     				// (d) PF4,PF0 is edge-sensitive
//  GPIO_PORTF_IBE_R &= ~0x11;    				//     PF4,PF0 is not both edges
//  GPIO_PORTF_IEV_R |= 0x11;    					//     PF4,PF0 rising edge event
//  GPIO_PORTF_ICR_R = 0x11;      				// (e) clear flags 4,0
//  GPIO_PORTF_IM_R |= 0x11;      				// (f) arm interrupt on PF4,PF0
//  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF1FFFFF)|0x00400000; // (g) bits:23-21 for PORTF, set priority to 5
//  NVIC_EN0_R |= 0x40000000;      				// (h) enable interrupt 30 in NVIC
//}
