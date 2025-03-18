// FollowingRobot.c
// Runs TM4C123
// Starter program CECS 347 project 2 - A Follwoing Robot
// by Jose Ambriz, Bronson Garel, Jonathan Kim, Kyle Wyckoff
// 03/17/2025

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <stdlib.h>
#include "ADC1SS2.h"  
#include "Motors.h"
#include "SW_LED.h"
#include "PLL.h"

enum robot_modes {OBJECT_FOLLOWER, WALL_FOLLOWER};
enum activity {ACTIVE, INACTIVE};
enum dir {LEFT, RIGHT};

// Function prototypes
// external functions
extern void DisableInterrupts(void);
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // low power mode
extern void GPIOPortF_Handler(void);

// functions defined in this file
void System_Init(void);
void object_follower(void);
void wall_follower(void);

void stop_the_car(void);    // (ADDED)
void move_backward(void);   // (ADDED)
void move_forward(void);    // (ADDED)
void turn_right(void);
void turn_left(void);
void backup_left(void);
void backup_right(void);
void pivot_left(void);
void pivot_right(void);
void uturn_left(void);
void uturn_right(void);
uint8_t check_closest(unsigned long l, unsigned long forward, unsigned long right); // (ADDED)

enum robot_modes mode= OBJECT_FOLLOWER;
enum activity curr = INACTIVE;
enum dir state = LEFT;

#define TOO_FAR 		(1550)  // replace the zero with the ADC output value for maximum distance					(MODIFY) (1550)
#define FOLLOW_DIST (2000)  // replace the zero with the ADC output value for object following distance (MODIFY) (2000)
#define TOO_CLOSE 	(2800)  // replace the zero with the ADC output value for minimum distance					(MODIFY) (2800)

unsigned long median(unsigned long u1, unsigned long u2, unsigned long u3);
void ReadADCMedianFilter(unsigned long *ain2, unsigned long *ain3, unsigned long *ain1);

unsigned long frwdleft, frwdright, ahead, object, i, count;
int error;
volatile uint16_t ADCvalue;

int main(void){	
	System_Init();
	count = 0;
	curr = INACTIVE;
	mode = OBJECT_FOLLOWER;
  while(1){
		if (curr == ACTIVE)
		{
			for (i=0;i<10;i++) {
				ReadADCMedianFilter(&frwdleft, &ahead, &frwdright);
			}
			switch (mode) {
				case OBJECT_FOLLOWER:
					object_follower();
					break;
				case WALL_FOLLOWER:
					wall_follower();
					break;
				default:
					WaitForInterrupt();
					break;				
			}
		}
		else
		{
			stop_the_car();
			WaitForInterrupt();
		}
	}
}

void System_Init(void){
	DisableInterrupts();
  PLL_Init();            // set system clock to 16 MHz 
	ADC1_SS2_Init();       // Initialize ADC1 Sample sequencer 1 
	PortB_Init();	
	Motors_Init(PERIOD);
  SW_LED_Init();         // initialize switches and LEDs in SW_LED module
  EnableInterrupts();	
}

void object_follower(void)
{
	ReadADCMedianFilter(&frwdleft, &ahead, &frwdright);
	error = frwdright-frwdleft;
	uint8_t i; 
	
	stop_the_car();
	
	// wait until an obstacle is in the right distant range.
	do {
			ReadADCMedianFilter(&frwdleft, &ahead, &frwdright);
	} while (((ahead>TOO_CLOSE)  
						|| (frwdleft>TOO_CLOSE) 
						|| (frwdright>TOO_CLOSE)) || 
					((ahead<TOO_FAR) 
						&& (frwdleft<TOO_FAR) 
						&& (frwdright<TOO_FAR)));
	
	object = check_closest(frwdleft, ahead, frwdright);
	
	switch(object)
	{
		case 0:	// Front Sensor
			if (ahead > FOLLOW_DIST+250)
			{
				move_backward();
			}
			else if (ahead < FOLLOW_DIST-250)
			{
				move_forward();
			}
			else
			{
				stop_the_car();
			}
			break;
		case 1: // Left Sensor
			if (frwdleft > FOLLOW_DIST)
			{
				backup_right();
			}
			else
			{
				turn_right();
			}
			break;
		case 2:	// Right Sensor
			if (frwdright > FOLLOW_DIST)
			{
				backup_left();
			}
			else
			{
				turn_left();
			}
			break;
		default:
			stop_the_car();
			break;
	}
}

void wall_follower(void){
	if(ahead > FOLLOW_DIST)
	{
		if(state == LEFT)
		{
			pivot_right();
		}
		else
		{
			pivot_left();
		}
	}
	else if((frwdleft < 1000) && (frwdright < 1000))
	{
		if(state == LEFT)
		{
			uturn_right();
		}
		else
		{
			uturn_left();
		}
	}
	else
	{
		DIRECTION = FORWARD;
		PWM0A_Duty(SPEED_30);
		PWM0B_Duty(SPEED_30);
	}        
}

// Median function from EE345M Lab 7 2011; Program 5.1 from Volume 3
// helper function for ReadADCMedianFilter() but works for general use
unsigned long median(unsigned long u1, unsigned long u2, unsigned long u3){
unsigned long result;
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

// This function samples AIN2 (PE1), AIN9 (PE4), AIN8 (PE5) and
// returns the results in the corresponding variables.  Some
// kind of filtering is required because the IR distance sensors
// output occasional erroneous spikes.  This is a median filter:
// y(n) = median(x(n), x(n-1), x(n-2))
// Assumes: ADC initialized by previously calling ADC_Init298()
void ReadADCMedianFilter(unsigned long *ain2, unsigned long *ain3, unsigned long *ain1){
  //                   x(n-2)        x(n-1)
  static unsigned long ain2oldest=0, ain2middle=0;
  static unsigned long ain3oldest=0, ain3middle=0;
  static unsigned long ain1oldest=0, ain1middle=0;
  // save some memory; these do not need to be 'static'
  //            x(n)
  unsigned long ain2newest;
  unsigned long ain3newest;
  unsigned long ain1newest;
  ADC_In321(&ain2newest, &ain3newest, &ain1newest); // sample AIN2(PE1), AIN3 (PE0), AIN1 (PE2)
  *ain2 = median(ain2newest, ain2middle, ain2oldest);
  *ain3 = median(ain3newest, ain3middle, ain3oldest);
  *ain1 = median(ain1newest, ain1middle, ain1oldest);
  ain2oldest = ain2middle; ain3oldest = ain3middle; ain1oldest = ain1middle;
  ain2middle = ain2newest; ain3middle = ain3newest; ain1middle = ain1newest;
}

//stops the car
void stop_the_car(void)
{
	DIRECTION = FORWARD;
	PWM0A_Duty(STOP);
	PWM0B_Duty(STOP);
}

void move_forward(void)
{
	DIRECTION = FORWARD;
	PWM0A_Duty(SPEED_60);
	PWM0B_Duty(SPEED_60); //speed can be 100
}

void move_backward(void)
{
	DIRECTION = BACKWARD;
	PWM0A_Duty(SPEED_60);
	PWM0B_Duty(SPEED_60); //speed can be 100
}

void turn_right(void)
{
	DIRECTION = FORWARD;
	PWM0A_Duty(SPEED_60);
	PWM0B_Duty(SPEED_30); //speed can be 100
}

void turn_left(void)
{
	DIRECTION = FORWARD;
	PWM0A_Duty(SPEED_30);
	PWM0B_Duty(SPEED_60); //speed can be 100
}

void backup_left(void)
{
	DIRECTION = BACKWARD;
	PWM0A_Duty(SPEED_60);
	PWM0B_Duty(SPEED_30); //speed can be 100
}

void backup_right(void)
{
	DIRECTION = BACKWARD;
	PWM0A_Duty(SPEED_30);
	PWM0B_Duty(SPEED_60); //speed can be 100
}

void pivot_left(void){
    PWM0A_Duty(STOP);
    PWM0B_Duty(STOP);
    //delay
    for(i=0; i<40000; i=i+1);   // several second delay before robot begins moving
    DIRECTION = PIVOT_LEFT;
    PWM0A_Duty(SPEED_50);
    PWM0B_Duty(SPEED_50);
    //delay
    for(i=0; i<40000; i=i+1);   // several second delay before robot begins moving

}
void pivot_right(void){
    PWM0A_Duty(STOP);
    PWM0B_Duty(STOP);
    //delay
    for(i=0; i<40000; i=i+1);   // several second delay before robot begins moving
    DIRECTION = PIVOT_RIGHT;
    PWM0A_Duty(SPEED_50);
    PWM0B_Duty(SPEED_50);
    //delay
    for(i=0; i<40000; i=i+1);   // several second delay before robot begins moving

}

void uturn_left(void){
    PWM0A_Duty(SPEED_30);
    PWM0B_Duty(SPEED_60);
    //delay
    for(i=0; i<120000; i=i+1);   // several second delay before robot begins moving
}
void uturn_right(void){
    PWM0A_Duty(SPEED_60);
    PWM0B_Duty(SPEED_30);
    //delay
    for(i=0; i<120000; i=i+1);   // several second delay before robot begins moving
}

uint8_t check_closest(unsigned long left, unsigned long front, unsigned long right)
{
	uint8_t largest; // 0 = front, 1 = left, 2 = right
	unsigned long current_largest = left;
	
	// Check if object is in front of front sensor
	if (current_largest < front)
	{
		current_largest = front;
	}
	
	// Check if object is in front of right sensor
	if (current_largest < right)
	{
		current_largest = right;
	}
	
	if (current_largest == front)
	{
		largest = 0;
	}
	else if (current_largest == left)
	{
		largest = 1;
	}
	else
	{
		largest = 2;
	}
		
	return largest;
}

// Handle Button Press
void GPIOPortF_Handler(void){ // called on press of either SW1 or SW2
	int TENMS = 727240*20/91; 	//that's actually 20ms not 10
	while(TENMS) TENMS--;
	
  if((GPIO_PORTF_RIS_R&0x10) ){  // SW1 pressed
		// Turn car on or off 
		if (curr == INACTIVE)
		{
			curr = ACTIVE;
		}
		else
		{
			curr = INACTIVE;
		}
		GPIO_PORTF_ICR_R = 0x10;  // acknowledge flag4
  }
	else	// SW2 pressed
	{
		// Toggle Mode 
		if (mode == OBJECT_FOLLOWER)
		{
			mode = WALL_FOLLOWER;
			for (i=0;i<10;i++) {
				ReadADCMedianFilter(&frwdleft, &ahead, &frwdright);
			}
			if(frwdright > frwdleft){
				state = RIGHT;
			}
			else{
				state = LEFT;
			}
		}
		else
		{
			mode = OBJECT_FOLLOWER;
		}
		GPIO_PORTF_ICR_R = 0x01;  // acknowledge flag4
	}
}
