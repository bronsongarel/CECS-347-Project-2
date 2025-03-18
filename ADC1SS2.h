// ADC0SS3.h
// Runs on TM4C123
// Provide functions that initialize ADC1 SS2 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result.
// Jose Ambriz, Bronson Garel, Jonathan Kim, Kyle Wyckoff
// March 17, 2025

// This file provide initialization function for two analog channels:
// PE2/AIN1 and PD0/AIN7

#include <stdint.h>

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS2 triggering event: software trigger
// SS2 1st sample source: Ain3 (PE0)
// SS2 2nd sample source: Ain2 (PE1)
// SS2 3rd sample source: Ain1 (PE2)
// SS2 interrupts: flag set on completion but no interrupt requested
void ADC1_SS2_Init(void);

//------------ADC_InS321------------
// Busy-wait Analog to digital conversion
// Input: ain2, ain3, ain1
// Output: 3, 12-bit results of ADC conversion
void ADC_In321(unsigned long *ain2, unsigned long *ain3, unsigned long *ain1);
