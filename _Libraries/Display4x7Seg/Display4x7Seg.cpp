/* 
* Display4x7Seg.cpp
*
* Created: 2019-11-07 21:06:28
* Author: Gabriel
*/

#include "Display4x7Seg.h"

// default constructor
Display4x7Seg::Display4x7Seg()
{
	mNumber = 0;
	mNextDigit = 0;
	ConfigPorts();
}

// default destructor
Display4x7Seg::~Display4x7Seg()
{
}

// Config ports for MUX and display
void Display4x7Seg::ConfigPorts()
{
	// MUX config
	DDRC |= (1 << MUX_A_PIN);
	DDRC |= (1 << MUX_B_PIN);
	
	// Display data config
	DDRC |= (1 << DISPLAY_DATA_1_PIN);
	DDRC |= (1 << DISPLAY_DATA_2_PIN);
	DDRC |= (1 << DISPLAY_DATA_3_PIN);
	DDRC |= (1 << DISPLAY_DATA_4_PIN);
}

// Set timer0 and interrupt
void Display4x7Seg::SetTimer()
{
	cli();
	
	//1 000 000 [Hz] / 1000[ms] = 1000 [tick / ms]
	//1000 / 8 = 125
	//125 + 130 = 255 (interrupt every 1ms)
	
	// timer0 prescaler = 8
	TCCR0 |= (1 << CS01);
	
	// enable timer0 interrupt
	TIMSK |= (1 << TOIE0);
	
	//enable interrupts
	sei();         
}

// show next number
void Display4x7Seg::ShowNextDigit()
{
	switch(Display4x7Seg::GetInstance().mNextDigit)
	{
		case 0:
		Display4x7Seg::GetInstance().ShowDigit(Display4x7Seg::GetInstance().mNumber % 10000 / 1000, 0);
		break;
		
		case 1:
		Display4x7Seg::GetInstance().ShowDigit(Display4x7Seg::GetInstance().mNumber % 1000 / 100, 1);
		break;
		
		case 2:
		Display4x7Seg::GetInstance().ShowDigit(Display4x7Seg::GetInstance().mNumber % 100 / 10, 2);
		break;
		
		case 3:
		Display4x7Seg::GetInstance().ShowDigit(Display4x7Seg::GetInstance().mNumber % 10, 3);
		break;
		
	}
	++Display4x7Seg::GetInstance().mNextDigit;
	
	if(Display4x7Seg::GetInstance().mNextDigit > 3)
	Display4x7Seg::GetInstance().mNextDigit = 0;	
}

#if USE_TIMER == 1
// timer0 interrupt
ISR (TIMER0_OVF_vect)
{
	TCNT0 += 130; //125 + 130 = 255 (interrupt every 1ms)
	Display4x7Seg::GetInstance().ShowNextDigit();
}
#endif

// set number to show in timer mode
void Display4x7Seg::SetNumber(unsigned int number)
{
	if(number < 0)
		number = 0;
	
	if(number > 9999)
		number = 9999;	
		
	mNumber = number;
}

// show number on display
void Display4x7Seg::ShowNumber(unsigned int number)
{	
	if(number < 0)
		number = 0;
	
	if(number > 9999)
		number = 9999;
	
	ShowDigit(number % 10, 3);
	_delay_ms(DISPLAY_SHOW_MS);
	
	ShowDigit(number % 100 / 10, 2);
	_delay_ms(DISPLAY_SHOW_MS);
	
	ShowDigit(number % 1000 / 100, 1);
	_delay_ms(DISPLAY_SHOW_MS);
	
	ShowDigit(number % 10000 / 1000, 0);
	_delay_ms(DISPLAY_SHOW_MS);
}

// show single digit
void Display4x7Seg::ShowDigit(unsigned char digit, unsigned char display)
{
	if(display < 0)
	display = 0;
	
	if(display > 3)
	display = 3;
	
	if(digit < 0)
	digit = 0;
	
	if(digit > 9)
	digit = 9;
	
	// Set display pins
	switch(display)
	{
		case 0:
		MUX_A_PORT &= ~(1 << MUX_A_PIN);
		MUX_B_PORT |= (1 << MUX_B_PIN);
		break;
		
		case 1:
		MUX_A_PORT |= (1 << MUX_A_PIN);
		MUX_B_PORT &= ~(1 << MUX_B_PIN);
		break;
		
		case 2:
		MUX_A_PORT &= ~(1 << MUX_A_PIN);
		MUX_B_PORT &= ~(1 << MUX_B_PIN);
		break;
		
		case 3:
		MUX_A_PORT |= (1 << MUX_A_PIN);
		MUX_B_PORT |= (1 << MUX_B_PIN);
		break;
	}
	
	// Set data pins
	if(digit & (1 << 0))
	DISPLAY_DATA_1_PORT |= (1 << DISPLAY_DATA_1_PIN);
	else
	DISPLAY_DATA_1_PORT &= ~(1 << DISPLAY_DATA_1_PIN);
	
	if(digit & (1 << 1))
	DISPLAY_DATA_2_PORT |= (1 << DISPLAY_DATA_2_PIN);
	else
	DISPLAY_DATA_2_PORT &= ~(1 << DISPLAY_DATA_2_PIN);
	
	if(digit & (1 << 2))
	DISPLAY_DATA_3_PORT |= (1 << DISPLAY_DATA_3_PIN);
	else
	DISPLAY_DATA_3_PORT &= ~(1 << DISPLAY_DATA_3_PIN);
	
	if(digit & (1 << 3))
	DISPLAY_DATA_4_PORT |= (1 << DISPLAY_DATA_4_PIN);
	else
	DISPLAY_DATA_4_PORT &= ~(1 << DISPLAY_DATA_4_PIN);
}

#if DISPLAY_TEST == 1
void Display4x7Seg::TestNumber()
{
	++testOverflow;
	if(testOverflow > 200)
	{
		testOverflow = 0;
		++testCounter;
	}
	
	switch(testCounter)
	{
		case 0:
		ShowNumber(0);
		break;
		
		case 1:
		ShowNumber(1111);
		break;
		
		case 2:
		ShowNumber(2222);
		break;
		
		case 3:
		ShowNumber(3333);
		break;
		
		case 4:
		ShowNumber(4444);
		break;
		
		case 5:
		ShowNumber(5555);
		break;
		
		case 6:
		ShowNumber(6666);
		break;
		
		case 7:
		ShowNumber(7777);
		break;
		
		case 8:
		ShowNumber(8888);
		break;
		
		case 9:
		ShowNumber(9999);
		break;
		
		default:
		testCounter = 0;
		break;
	}
}

void Display4x7Seg::TestMUX()
{
	int testDelay = 5000;
	
	MUX_A_PORT &= ~(1 << MUX_A_PIN);
	MUX_B_PORT &= ~(1 << MUX_B_PIN);
	//MUX_C_PORT &= ~(1 << MUX_C_PIN);
	_delay_ms(testDelay);
	
	MUX_A_PORT |= (1 << MUX_A_PIN);
	MUX_B_PORT &= ~(1 << MUX_B_PIN);
	//MUX_C_PORT &= ~(1 << MUX_C_PIN);
	_delay_ms(testDelay);
	
	MUX_A_PORT &= ~(1 << MUX_A_PIN);
	MUX_B_PORT |= (1 << MUX_B_PIN);
	//MUX_C_PORT &= ~(1 << MUX_C_PIN);
	_delay_ms(testDelay);
	
	MUX_A_PORT |= (1 << MUX_A_PIN);
	MUX_B_PORT |= (1 << MUX_B_PIN);
	//MUX_C_PORT &= ~(1 << MUX_C_PIN);
	_delay_ms(testDelay);
	
	MUX_A_PORT &= ~(1 << MUX_A_PIN);
	MUX_B_PORT &= ~(1 << MUX_B_PIN);
	//MUX_C_PORT |= (1 << MUX_C_PIN);
	_delay_ms(testDelay);
	
	MUX_A_PORT |= (1 << MUX_A_PIN);
	MUX_B_PORT &= ~(1 << MUX_B_PIN);
	//MUX_C_PORT |= (1 << MUX_C_PIN);
	_delay_ms(testDelay);
	
	MUX_A_PORT &= ~(1 << MUX_A_PIN);
	MUX_B_PORT |= (1 << MUX_B_PIN);
	//MUX_C_PORT |= (1 << MUX_C_PIN);
	_delay_ms(testDelay);
	
	MUX_A_PORT |= (1 << MUX_A_PIN);
	MUX_B_PORT |= (1 << MUX_B_PIN);
	//MUX_C_PORT |= (1 << MUX_C_PIN);
	_delay_ms(testDelay);
}
#endif