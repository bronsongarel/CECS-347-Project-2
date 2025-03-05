// FollowMe.c
// Runs on TM4C123
// By Jose Ambriz, Bronson Garel, Jonathan Kim, Kyle Wyckoff
// March 5, 2024

// This is an example program to show how to use hardware PWM on TM4C123.
#include <stdint.h>
#include "PWM.h"
#include "ADC0SS3.h"
#include "PLL.h"
#include "SysTick.h"

void stop_the_car(void);    //(ADDED)
void follow_me(void);				//(ADDED)
void move_backward(void);   //(ADDED)
void move_forward(void);    //(ADDED)

uint16_t median(uint16_t u1, uint16_t u2, uint16_t u3);  //(COPIED)
uint16_t ReadADCMedianFilter(void);											 //(COPIED)


int main(void){	
	PLL_Init();									 	// 16MHz system clock
	ADC0_InitSWTriggerSeq3_Ch7();	// ADC initialization PD0/AIN7
	PortB_Init();	             		// PB4:Left wheel, PB5:Right wheel
	
	// initialize PB32: left direction pins, PB76: right direction pins
	PWM0GEN1A_Init(10000);     		// initialize PWM0, PB6 LEFT MOTOR
  PWM0GEN1B_Init(10000);     		// initialize PWM0, PB7 RIGHT MOTOR
	
	while(1){
		DIRECTION = FORWARD;
	  stop_the_car();
		follow_me();
		//move_forward(); // cam side (WORKS)
		//move_backward(); //board side
  }
}

// Median function: 
// A helper function for ReadADCMedianFilter()
uint16_t median(uint16_t u1, uint16_t u2, uint16_t u3){
uint16_t result;
	
  if(u1>u2)
    if(u2>u3)   result=u2;     // u1>u2,u2>u3       u1>u2>u3
      else
        if(u1>u3) result=u3;   // u1>u2,u3>u2,u1>u3 u1>u3>u2
        else      result=u1;   // u1>u2,u3>u2,u3>u1 u3>u1>u2
  else
    if(u3>u2)   result=u2;     // u2>u1,u3>u2       u3>u2>u1
      else
        if(u1>u3) result=u1;   // u2>u1,u2>u3,u1>u3 u2>u1>u3
        else      result=u3;   // u2>u1,u2>u3,u3>u1 u2>u3>u1
  return(result);
}

// This function samples one value, apply a software filter to the value and
// returns filter result.  Some kind of filtering is required because 
// the IR distance sensors output occasional erroneous spikes.  
// This function implements a median filter:
// y(n) = median(x(n), x(n-1), x(n-2))
// Assumes: ADC has already been initialized. 
uint16_t ReadADCMedianFilter(void){
  static uint16_t oldest=0, middle=0;	
  uint16_t newest;
	uint16_t NewValue;
	
  newest = ADC0_InSeq3();  // read one value
  NewValue = median(newest, middle, oldest);
  oldest = middle; 
  middle = newest; 
	return NewValue;
}

// Implement a simple algorithm to follow an object to move forward/backward/stop.
void follow_me(void) {
	uint8_t i;
  volatile uint16_t ADCvalue;
	
	// Calibrate the sensor
	for (i=0;i<10;i++) {
    ADCvalue = ReadADCMedianFilter();  // read one value
	} 
	
  // wait until an obstacle is in the right distant range.
 	do {
			ADCvalue = ReadADCMedianFilter();  // read one value
	} while ((ADCvalue>TOO_CLOSE) || (ADCvalue<TOO_FAR));
		
  while ((ADCvalue<TOO_CLOSE) && (ADCvalue>TOO_FAR)) {								
		ADCvalue = ReadADCMedianFilter();
		
		if (ADCvalue>FOLLOW_DIST) { // negative logic: too close, move back
			move_backward();
		}
		else if (ADCvalue<FOLLOW_DIST){ // negative logic: too far, move forward
			move_forward();
		}
		else { // right distance, stop
			stop_the_car();
		}
  }	
}


//stops the car
void stop_the_car(void){
	DIRECTION = FORWARD;
	PWM0A_Duty(STOP);
	PWM0B_Duty(STOP);
}

void move_forward(void){
	// forward 50
	DIRECTION = FORWARD;
	PWM0A_Duty(SPEED_50);
	PWM0B_Duty(SPEED_50);
	SysTick_Wait(16);
}

void move_backward(void){
	DIRECTION = BACKWARD;
	PWM0A_Duty(SPEED_50);
	PWM0B_Duty(SPEED_50);
	SysTick_Wait(16);
}

