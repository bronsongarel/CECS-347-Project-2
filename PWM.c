// PWM.c
// Runs on TM4C123
// Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs.
// By Jose Ambriz, Bronson Garel, Jonathan Kim, Kyle Wyckoff
// March 5, 2025

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "PWM.h"

uint16_t DUTY = STOP;

void Delay(void){unsigned long volatile time;
  time = 727240*100/91;  // 1 sec
  while(time){
		time--;
  }
	for (time=0;time<1000000;time=time+1) {
	}
}

// period is 16-bit number of PWM clock cycles in one period 
// Output on PB4/M0PWM1,PB5/M0PWM1
void PWM0Gen1AB_Init(uint16_t period){
	SYSCTL_RCGCPWM_R 		|= SYSCTL_RCGCPWM_R0;             // Activate PWM0
	SYSCTL_RCGCGPIO_R 	|= SYSCTL_RCGCGPIO_R1;            // Activate port B: 000010
  while((SYSCTL_RCGCGPIO_R&SYSCTL_RCGCGPIO_R1) == 0){};	// Wait for clock
		
	GPIO_PORTB_AFSEL_R 	|= PWM_MASK;          					 	// Enable alt funct on PB4: 0001 0000
	GPIO_PORTB_PCTL_R 	&= ~0x00FF0000;     							// configure PB4 as PWM0
  GPIO_PORTB_PCTL_R 	|= 0x00440000;
  GPIO_PORTB_AMSEL_R	&= ~PWM_MASK;          						// Disable analog functionality on PB6
  GPIO_PORTB_DEN_R 		|= PWM_MASK;             					// Enable digital I/O on PB6
	GPIO_PORTB_DR8R_R 	|= PWM_MASK;    									// Enable 8 mA drive on PB4,5
		
  SYSCTL_RCC_R = SYSCTL_RCC_USEPWMDIV |           			// Use PWM divider
    (SYSCTL_RCC_R & (~(SYSCTL_RCC_USEPWMDIV)|
		(SYSCTL_RCC_PWMDIV_M)));   													// Configure for /2 divider: PWM clock: 80Mhz/2=40MHz
		
  PWM0_1_CTL_R = 0;                     								// Re-loading down-counting mode FIXME
  PWM0_1_GENA_R = (PWM_1_GENA_ACTCMPAD_ONE |
		PWM_1_GENA_ACTLOAD_ZERO);                 					// Low on LOAD, high on CMPA down FIX ME
	PWM0_1_GENB_R = (PWM_1_GENB_ACTCMPBD_ONE |
		PWM_1_GENB_ACTLOAD_ZERO); 													// 0xC08

  PWM0_1_LOAD_R = period - 1;           								// Cycles needed to count down to 0
  PWM0_1_CMPA_R = 0;             												// Count value when output rises
	PWM0_1_CMPB_R = 0;																		// Count value when output rises
  PWM0_1_CTL_R |= PWM_1_CTL_ENABLE;           					// Start PWM0
  PWM0_ENABLE_R |= (PWM_ENABLE_PWM2EN |
	PWM_ENABLE_PWM3EN);          												// Enable PB4,5/M0PWM2,M0PWM3 FIX ME
}

// change duty cycle of PB4
// duty is number of PWM clock cycles output is high  
void PWM0A_Duty(uint16_t duty){
  PWM0_1_CMPA_R = duty - 1;   // count value when output rises
}

// change duty cycle of PB5
// duty is number of PWM clock cycles output is high  
void PWM0B_Duty(uint16_t duty){
  PWM0_1_CMPB_R = duty - 1;		//count value when output rises
}

// Initialize Port B
void PortB_Init(void){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; //activate B clock
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOB)!= SYSCTL_RCGC2_GPIOB){} //wait for clk
	
	GPIO_PORTB_AMSEL_R &= ~DIR_MASK; //disable analog function
	GPIO_PORTB_PCTL_R &= ~0xFF00FF00; //GPIO clear bit PCTL
	GPIO_PORTB_DIR_R |= DIR_MASK; //PE0-3 output
	GPIO_PORTB_AFSEL_R &= ~DIR_MASK; //no alternate function
	GPIO_PORTB_DEN_R |= DIR_MASK; //enable digital pins PE0-3
}

// Initilize port F and arm PF4, PF0 for falling edge interrupts
void SwitchLED_Init(void){  
	unsigned long volatile delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // (a) activate clock for port F
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY; // unlock GPIO Port F
  GPIO_PORTF_CR_R |= 0x1F;         // allow changes to PF4,0
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4,0 in (built-in button)
  GPIO_PORTF_DIR_R |= 0x0E;     // make PF1-3 out
  GPIO_PORTF_AFSEL_R &= ~0x11;  //     disable alt funct on PF4,0
  GPIO_PORTF_DEN_R |= 0x1F;     //     enable digital I/O on PF4,0
  GPIO_PORTF_PCTL_R &= ~0x000FFFFF; //  configure PF4,0 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x1F;  //     disable analog functionality on PF4,0
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4,0
  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4,PF0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //     PF4,PF0 is not both edges
  GPIO_PORTF_IEV_R |= 0x11;    //     PF4,PF0 rising edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flags 4,0
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4,PF0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF1FFFFF)|0x00400000; // (g) bits:23-21 for PORTF, set priority to 5
  NVIC_EN0_R |= 0x40000000;      // (h) enable interrupt 30 in NVIC
}

// Subroutine to initialize port E pins PE0-3 for output
// PE0-3 control direction of motor
// Inputs: None
// Outputs: None
//void PortE_Init(void){
//	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; //activate E clock
//	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOE)!= SYSCTL_RCGC2_GPIOE){} //wait for clk
//	
//	GPIO_PORTE_AMSEL_R &= ~0x0F; //disable analog function
//	GPIO_PORTE_PCTL_R &= ~0x0000FFFF; //GPIO clear bit PCTL
//	GPIO_PORTE_DIR_R |= 0x0F; //PE0-3 output
//	GPIO_PORTE_AFSEL_R &= ~0x0F; //no alternate function
//	GPIO_PORTE_DEN_R |= 0x0F; //enable digital pins PE0-3
//}

void GPIOPortF_Handler(void){ // called on touch of either SW1 or SW2
	int TENMS = 727240*20/91; //that's actually 20ms not 10
	while(TENMS) TENMS--;
	
  if((GPIO_PORTF_RIS_R&0x10) ){  // SW1 touch
		
	
		GPIO_PORTF_ICR_R = 0x10;  // acknowledge flag4
		
		if (done == 0) {// if car is not running
			done = 1;
			PWM0_ENABLE_R |= 0x0000000C;
		}
		else //sw pressed while driving
		{
			DIRECTION = FORWARD;
			PWM0A_Duty(STOP);
			PWM0B_Duty(STOP);
			PWM0_ENABLE_R = 0;          // enable PB6/M0PWM2 FIX ME
			done = 0;
		}
  }
}




