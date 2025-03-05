// PWM.h
// Runs on TM4C123
// Use PWM0/PB4 and PWM0/PB5 to generate pulse-width modulated outputs.
// By Jose Ambriz, Bronson Garel, Jonathan Kim, Kyle Wyckoff
// March 5, 2025
#include <stdint.h>

#define DIRECTION (*((volatile unsigned long *)0x40005330))
// right motor pins 7,6 // left motor pins 3,2
#define FORWARD 			0xCC 	//11001100
#define BACKWARD 			0x88  //10001000
#define PIVOT_LEFT		0xC8	//11001000 right forward, left backward
#define PIVOT_RIGHT 	0x8C	//10001100 right backward, left forward

#define LIGHT (*((volatile unsigned long *)0x40025038))
#define RED 0x02
#define GREEN 0x08
#define BLUE 0x04
#define PERIOD 10000				// Total PWM period

#define STOP 1
#define SPEED_30 3000
#define SPEED_35 3500
#define SPEED_50 5000
#define SPEED_60 6000
#define SPEED_80 8000
#define SPEED_98 9800

#define PWMA 0x10 // left motor PB4 = 0x10, DIR=PB2 SLP=PB3
#define PWMB 0x20 // right motor PB5 = 0x20, DIR=PB6 SLP=PB7
#define PWM_MASK PWMA|PWMB
// direction pins
// PB7,6,3,2 = 11001100 = 0xCC
#define DIR_MASK 0xCC

static volatile uint8_t done;

void PLL_Init(void);

// period is 16-bit number of PWM clock cycles in one period 
// Output on PB6/M0PWM0
void PWM0GEN1A_Init(uint16_t period);

// change duty cycle of PB6
// duty is number of PWM clock cycles output is high  
void PWM0A_Duty(uint16_t duty);

// period is 16-bit number of PWM clock cycles in one period 
// Output on PB7/M0PWM1
void PWM0GEN1B_Init(uint16_t period);

// change duty cycle of PB7
// duty is number of PWM clock cycles output is high  
void PWM0B_Duty(uint16_t duty);

void PortB_Init(void);
void SwitchLED_Init(void);




