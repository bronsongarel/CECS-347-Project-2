#include <stdint.h>

#define SW_MASK 		0x11
#define LED_MASK		0x0E
#define SWLED_MASK	0x1F

#define LIGHT 	(*((volatile unsigned long *)0x40025038))
#define RED 		0x02
#define GREEN 	0x08
#define BLUE 		0x04

void SW_LED_Init(void);
