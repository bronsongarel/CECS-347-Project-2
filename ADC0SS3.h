// ADC0SS3.h
// Runs on TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result.
// Bronson Garel
// March 5, 2025


// This file provide initialization function for two analog channels:
// PE2/AIN1 and PD0/AIN7

#include <stdint.h>
#define TOO_FAR 			(1580)  // replace the zero with the ADC output value for maximum distance					(MODIFY)
#define FOLLOW_DIST 	(1610)  // replace the zero with the ADC output value for object following distance (MODIFY)
#define TOO_CLOSE 		(1640)  // replace the zero with the ADC output value for minimum distance					(MODIFY)
#define RIGHT_IR			0x04		// PE2
#define LEFT_IR				0x02		// PE1
#define FRONT_IR			0x01		// PE0
#define SENSOR_MASK		RIGHT_IR|LEFT_IR|FRONT_IR

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain7 (PD0)
// SS3 interrupts: flag set on completion but no interrupt requested
void ADC0_InitSWTriggerSeq3_Ch7(void);

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: Ain1 (PE2)
// SS3 interrupts: flag set on completion but no interrupt requested
void ADC0_InitSWTriggerSeq3_Ch1(void); 

//------------ADC0_InSeq3------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint16_t ADC0_InSeq3(void);
