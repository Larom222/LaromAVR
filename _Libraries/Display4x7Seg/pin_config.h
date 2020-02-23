
#ifndef __PIN_CONFIG_H__
#define __PIN_CONFIG_H__



//**************************************************
//					MUX pins
// 2 pins input to CD4501
// 4 pins output for 4 digits of display
//**************************************************
#ifdef PROJ_DISPLAY_TEST
	#define MUX_A_PORT PORTC
	#define MUX_A_PIN 4

	#define MUX_B_PORT PORTC
	#define MUX_B_PIN 5
#endif

//**************************************************
//					Display pins
// 4 pins input to CD4511
// 7 output to 7 segments of display HS420561k-32
//**************************************************
#ifdef PROJ_DISPLAY_TEST
	#define DISPLAY_DATA_1_PORT PORTC
	#define DISPLAY_DATA_1_PIN 0

	#define DISPLAY_DATA_2_PORT PORTC
	#define DISPLAY_DATA_2_PIN 1

	#define DISPLAY_DATA_3_PORT PORTC
	#define DISPLAY_DATA_3_PIN 2

	#define DISPLAY_DATA_4_PORT PORTC
	#define DISPLAY_DATA_4_PIN 3
#endif

#endif //__PIN_CONFIG_H__