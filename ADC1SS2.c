#include "tm4c123gh6pm.h"
#include "ADC1SS2.h"
#include <stdint.h>

#define ADC1_PSSI_SS2 0x0004    // start sample sequencer 2
#define ADC1_ISC_SS2  0x0004    // acknowledge sample sequencer 2 interrupt
#define ADC1_RIS_SS2  0x04			// acknowledge flag
#define PORTE_MASK		0x07			// PE0-2

void ADC1_SS2_Init(void){
	volatile uint16_t delay;
	SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R1;		// 1) Activate ADC1
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;	// 1) Activate clock for Port E
	
	GPIO_PORTE_DIR_R 		&= ~PORTE_MASK;      	// 2) make PE0-2 input
  GPIO_PORTE_AFSEL_R 	|= PORTE_MASK;     		// 3) enable alternate function on PE0-2
  GPIO_PORTE_DEN_R 		&= ~PORTE_MASK;      	// 4) disable digital I/O on PE0-2
  GPIO_PORTE_AMSEL_R 	|= PORTE_MASK;     		// 5) enable analog function on PE0-2
	
	ADC1_PC_R 					&= ~ADC_PC_SR_M;			// 6) clear max sample rate field
	ADC1_PC_R 					|= ADC_PC_SR_125K;		// 7) configure for 125K samples/sec
	ADC1_SSPRI_R 				 = 0x3210;          	// 8) Sequencer 3 is lowest priority
  ADC1_ACTSS_R 				&= ~ADC_ACTSS_ASEN2;  // 9) disable sample sequencer 1
  ADC1_EMUX_R 				&= ~ADC_EMUX_EM2_M;   // 10) seq1 is software trigger
  ADC1_SSMUX2_R 			 = 0x0132; 						// 11) channels Ain1, Ain3, Ain2 (PE2, PE0, PE1)
  ADC1_SSCTL2_R 			 = ADC_SSCTL1_END2|		// 12) no DO END0 IE0 TS0 D1 END1 IE1 TS1 D2 TS2, yes END2 IE2
												 ADC_SSCTL1_IE2;         		
	ADC1_IM_R						&= ~ADC_IM_MASK2;			// 13) Disable Interrupt
  ADC1_ACTSS_R 				|= ADC_ACTSS_ASEN2;   // 14) enable sample sequencer 3
}

void ADC_In321(unsigned long *ain2, unsigned long *ain3, unsigned long *ain1){
	ADC1_PSSI_R = ADC1_PSSI_SS2;
	while((ADC1_RIS_R&ADC1_RIS_SS2)==0){};
	*ain2 = ADC1_SSFIFO2_R&0xFFF;
	*ain3 = ADC1_SSFIFO2_R&0xFFF;
	*ain1 = ADC1_SSFIFO2_R&0xFFF;
	ADC1_ISC_R = ADC1_ISC_SS2;
}
