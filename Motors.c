// Motors.c
// Runs on TM4C123
// Use PWM0/PB4 and PWM0/PB5 to generate pulse-width modulated outputs.
// Jose Ambriz, Bronson Garel, Jonathan Kim, Kyle Wyckoff
// February 26, 2025

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SysTick.h"
#include "Motors.h"

uint16_t DUTY = STOP;

void Delay(void){unsigned long volatile time;
  time = 727240*100/91;  // 1 sec
  while(time){
		time--;
  }
	for (time=0;time<1000000;time=time+1) {
	}
}

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
  PWM0_1_CTL_R |= PWM_1_CTL_ENABLE;           					// Start PWM0
  PWM0_ENABLE_R |= (PWM_ENABLE_PWM2EN |
	PWM_ENABLE_PWM3EN);          													// Enable PB4,5/M0PWM2,M0PWM3 FIX ME
}

// period is 16-bit number of PWM clock cycles in one period 
// Output on PB4/M0PWM2
void PWM0GEN1A_Init(uint16_t period){ 				// gen 1, A
  SYSCTL_RCGCPWM_R 		|= SYSCTL_RCGCPWM_R0;   // 1) activate PWM0
	SYSCTL_RCGCGPIO_R 	|= SYSCTL_RCGCGPIO_R1;  // 2) activate port B: 000010
  while((SYSCTL_RCGCGPIO_R&SYSCTL_RCGCGPIO_R1) == 0){};
	GPIO_PORTB_AFSEL_R 	|= PWMA;           			// enable alt funct on PB4: 0001 0000
	GPIO_PORTB_PCTL_R 	&= ~0x000F0000;     		// configure PB4 as PWM0
  GPIO_PORTB_PCTL_R 	|= 0x00040000;
  GPIO_PORTB_AMSEL_R 	&= ~PWMA;          			// disable analog functionality on PB6
  GPIO_PORTB_DEN_R 		|= PWMA;             		// enable digital I/O on PB6
	GPIO_PORTB_DR8R_R 	|= PWM_MASK;    				// enable 8 mA drive on PB4,5
		
  SYSCTL_RCC_R = SYSCTL_RCC_USEPWMDIV |       // 3) use PWM divider
		(SYSCTL_RCC_R & (~SYSCTL_RCC_USEPWMDIV
		|SYSCTL_RCC_PWMDIV_M));   								// configure for /2 divider: PWM clock: 80Mhz/2=40MHz
		
  PWM0_1_CTL_R 				 &= ~PWM_1_CTL_ENABLE;  // 4) re-loading down-counting mode FIXME
  PWM0_1_GENA_R = PWM_1_GENA_ACTCMPAD_ONE|
		PWM_1_GENA_ACTLOAD_ZERO;                	// low on LOAD, high on CMPA down FIX ME (0xC8)
		
  // PB6 goes low on LOAD
  // PB6 goes high on CMPA down
  PWM0_1_LOAD_R 			 = period - 1;          // 5) cycles needed to count down to 0
  PWM0_1_CMPA_R 			 = 0;             			// 6) count value when output rises
  PWM0_1_CTL_R 				|= PWM_1_CTL_ENABLE;    // 7) start PWM0
  PWM0_ENABLE_R 			|= PWM_ENABLE_PWM2EN;   // enable PB6/M0PWM2 FIX ME
}

// period is 16-bit number of PWM clock cycles in one period 
// Output on PB7/M0PWM1 (PB4 NOW)
void PWM0GEN1B_Init(uint16_t period){
  volatile unsigned long delay;
  SYSCTL_RCGCPWM_R 		|= SYSCTL_RCGCPWM_R0;     // 1) activate PWM0
  SYSCTL_RCGCGPIO_R 	|= SYSCTL_RCGCGPIO_R1;    // 2) activate port B
  delay 							 = SYSCTL_RCGCGPIO_R;            			// allow time to finish activating
  GPIO_PORTB_AFSEL_R 	|= PWMB;           				// enable alt funct on PB5
	GPIO_PORTB_PCTL_R 	&= ~0x00F00000;     			// configure PB5 as M0PWM1
  GPIO_PORTB_PCTL_R 	|= 0x00400000;
  GPIO_PORTB_AMSEL_R 	&= ~PWMB;          				// disable analog functionality on PB7
  GPIO_PORTB_DEN_R 		|= PWMB;             			// enable digital I/O on PB7
  SYSCTL_RCC_R 				|= SYSCTL_RCC_USEPWMDIV; 	// 3) use PWM divider
  SYSCTL_RCC_R 				&= ~SYSCTL_RCC_PWMDIV_M; 	//    clear PWM divider field
  SYSCTL_RCC_R 				+= SYSCTL_RCC_PWMDIV_2;  	//    configure for /2 divider
  PWM0_1_CTL_R 				&= ~PWM_1_CTL_ENABLE;     // 4) re-loading down-counting mode
	
  PWM0_1_GENB_R = (PWM_1_GENB_ACTCMPBD_ONE |
		PWM_1_GENB_ACTLOAD_ZERO); // 0xC08
	
  // PB7 goes low on LOAD
  // PB7 goes high on CMPB down
  PWM0_1_LOAD_R	 			 = period - 1;           	// 5) cycles needed to count down to 0: LAB2: PF1->M1PWM5:PWM1_2_
  PWM0_1_CMPB_R 			 = 0;            					// 6) count value when output rises: Lab 2:5%2=1->CMPB, GENB
  PWM0_1_CTL_R 				|= PWM_1_CTL_ENABLE;      // 7) start PWM0: odd->B, even->A
  PWM0_ENABLE_R 			|= PWM_ENABLE_PWM3EN;     // enable PB5/M0PWM3 
}

void Motors_Init(uint16_t period)
{
	PWM0GEN1A_Init(period);
	PWM0GEN1B_Init(period);
}

// change duty cycle of PB4
// duty is number of PWM clock cycles output is high  
void PWM0A_Duty(uint16_t duty){
  PWM0_1_CMPA_R = duty - 1;             // 6) count value when output rises
}

// change duty cycle of PB5
// duty is number of PWM clock cycles output is high  
void PWM0B_Duty(uint16_t duty){
  PWM0_1_CMPB_R = duty - 1;             // 6) count value when output rises
}

void PortB_Init(void){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; //activate B clock
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOB)!= SYSCTL_RCGC2_GPIOB){} //wait for clk
	
	GPIO_PORTB_AMSEL_R &= ~DIR_MASK; 			//disable analog function
	GPIO_PORTB_PCTL_R &= ~0xFF00FF00; 		//GPIO clear bit PCTL
	GPIO_PORTB_DIR_R |= DIR_MASK; 				//PE0-3 output
	GPIO_PORTB_AFSEL_R &= ~DIR_MASK; 			//no alternate function
	GPIO_PORTB_DEN_R |= DIR_MASK; 				//enable digital pins PE0-3
}


