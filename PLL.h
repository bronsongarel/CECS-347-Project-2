#define SYSDIV2 24
// bus frequency is 400MHz/(SYSDIV2+1) = 400MHz/(24+1) = 16 MHz

// configure the system to get its clock from the PLL
void PLL_Init(void);
