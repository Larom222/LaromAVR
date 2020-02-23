/*
 * BlinkTest.cpp
 *
 * Created: 2019-11-01 11:17:09
 * Author : Gabriel
 */ 

#define F_CPU 1000000UL //1 MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// MUX pins
#define MUX_A_PORT PORTC
#define MUX_A_PIN 4

#define MUX_B_PORT PORTC
#define MUX_B_PIN 5

#define MUX_C_PORT PORTB
#define MUX_C_PIN 1

// Display pins
#define DISPLAY_DATA_1_PORT PORTC
#define DISPLAY_DATA_1_PIN 0

#define DISPLAY_DATA_2_PORT PORTC
#define DISPLAY_DATA_2_PIN 1

#define DISPLAY_DATA_3_PORT PORTC
#define DISPLAY_DATA_3_PIN 2

#define DISPLAY_DATA_4_PORT PORTC
#define DISPLAY_DATA_4_PIN 3


int counter = 1;
int overflow = 0;
int number = 0;

void Test();

void ControlBlink();

void TestMUX();
void TestMUX2();
void TestNumber();
void ShowDigit(char digit, char display);
void ShowNumber(int number);

void ConfigDisplay()
{
	// MUX config
	DDRC |= (1 << MUX_A_PIN);
	DDRC |= (1 << MUX_B_PIN);
	DDRB |= (1 << MUX_C_PIN);
	
	// Display data config
	DDRC |= (1 << DISPLAY_DATA_1_PIN);
	DDRC |= (1 << DISPLAY_DATA_2_PIN);
	DDRC |= (1 << DISPLAY_DATA_3_PIN);
	DDRC |= (1 << DISPLAY_DATA_4_PIN);
}


int main(void)
{
	ConfigDisplay();
	
	counter = 0;
		
	// LED output
	DDRD |= (1<<4);
		
	// EXT INT1 conf - falling edge
	MCUCR |= (1<<3);
	//MCUCR |= (1<<2);
	GICR |= (1<<7);
	
	// EXT INT0 conf - falling edge
	MCUCR |= (1<<1);
	//MCUCR |= (1<<0);
	GICR |= (1<<6);
	
	
	//INT1 as input
	DDRD &= ~(1<<3);
	PORTD |= (1<<3);
	
	//INT0 as input
	DDRD &= ~(1<<2);
	PORTD |= (1<<2);
			
	sei();
			
    /* Replace with your application code */
    while (1) 
    {		
		//Test();
		//TestMUX();
		//TestMUX2();
		//TestNumber();
		
		
		++overflow;
		if(overflow > 100)
		{
			overflow = 0;
			
			switch(counter)
			{
				case 0:
					++number;
				break;
				
				case 1:
					number += 10;
				break;
				
				case 2:
					number += 100;
				break;
				
				case 3:
					number += 1000;
				break;
			}
			
			if(number > 9999)
				number = 0;
		}
		
		ShowNumber(number);		
		
    }
}

void ShowNumber(int number)
{
	char displayDelay = 1;
	
	if(number < 0)
		number = 0;
		
	if(number > 9999)
		number = 9999;
		
	ShowDigit(number % 10, 3);
	_delay_ms(displayDelay);
	
	ShowDigit(number % 100 / 10, 2);
	_delay_ms(displayDelay);
	
	ShowDigit(number % 1000 / 100, 1);
	_delay_ms(displayDelay);
	
	ShowDigit(number % 10000 / 1000, 0);
	_delay_ms(displayDelay);
}

void ShowDigit(char digit, char display)
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

void TestNumber()
{
	++overflow;
	if(overflow > 200)
	{
		overflow = 0;
		++counter;
	}	
	
	switch(counter)
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
		counter = 0;
		break;
	}	
}

void TestMUX()
{
	int testDelay = 5000;
		
	MUX_A_PORT &= ~(1 << MUX_A_PIN);
	MUX_B_PORT &= ~(1 << MUX_B_PIN);
	MUX_C_PORT &= ~(1 << MUX_C_PIN);
	_delay_ms(testDelay);
		
    MUX_A_PORT |= (1 << MUX_A_PIN);
    MUX_B_PORT &= ~(1 << MUX_B_PIN);
    MUX_C_PORT &= ~(1 << MUX_C_PIN);
    _delay_ms(testDelay);
    
    MUX_A_PORT &= ~(1 << MUX_A_PIN);
    MUX_B_PORT |= (1 << MUX_B_PIN);
    MUX_C_PORT &= ~(1 << MUX_C_PIN);
    _delay_ms(testDelay);
    
    MUX_A_PORT |= (1 << MUX_A_PIN);
    MUX_B_PORT |= (1 << MUX_B_PIN);
    MUX_C_PORT &= ~(1 << MUX_C_PIN);
    _delay_ms(testDelay);
    
    MUX_A_PORT &= ~(1 << MUX_A_PIN);
    MUX_B_PORT &= ~(1 << MUX_B_PIN);
    MUX_C_PORT |= (1 << MUX_C_PIN);
    _delay_ms(testDelay);
    
    MUX_A_PORT |= (1 << MUX_A_PIN);
    MUX_B_PORT &= ~(1 << MUX_B_PIN);
    MUX_C_PORT |= (1 << MUX_C_PIN);
    _delay_ms(testDelay);
    
    MUX_A_PORT &= ~(1 << MUX_A_PIN);
    MUX_B_PORT |= (1 << MUX_B_PIN);
    MUX_C_PORT |= (1 << MUX_C_PIN);
    _delay_ms(testDelay);
    
    MUX_A_PORT |= (1 << MUX_A_PIN);
    MUX_B_PORT |= (1 << MUX_B_PIN);
    MUX_C_PORT |= (1 << MUX_C_PIN);
    _delay_ms(testDelay);
}

void TestMUX2()
{
	int testDelay = 3000;
	
	PORTB |= (1<<1);
	PORTB &= ~(1<<1);
	
	PORTC &= ~(1<<4);
	PORTC &= ~(1<<5);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTB |= (1<<1);
	PORTB &= ~(1<<1);
	
	PORTC |= (1<<4);
	PORTC &= ~(1<<5);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTB |= (1<<1);
	PORTB &= ~(1<<1);
	
	PORTC &= ~(1<<4);
	PORTC |= (1<<5);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTB |= (1<<1);
	PORTB &= ~(1<<1);
	
	PORTC |= (1<<4);
	PORTC |= (1<<5);
	_delay_ms(testDelay);
	
	ControlBlink();
	
}

void Test()
{
	int testDelay = 5000;
	
	PORTC &= ~(1<<0);
	PORTC &= ~(1<<1);
	PORTC &= ~(1<<2);
	PORTC &= ~(1<<3);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTC |= (1<<0);
	PORTC &= ~(1<<1);
	PORTC &= ~(1<<2);
	PORTC &= ~(1<<3);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTC &= ~(1<<0);
	PORTC |= (1<<1);
	PORTC &= ~(1<<2);
	PORTC &= ~(1<<3);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTC |= (1<<0);
	PORTC |= (1<<1);
	PORTC &= ~(1<<2);
	PORTC &= ~(1<<3);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTC &= ~(1<<0);
	PORTC &= ~(1<<1);
	PORTC |= (1<<2);
	PORTC &= ~(1<<3);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTC |= (1<<0);
	PORTC &= ~(1<<1);
	PORTC |= (1<<2);
	PORTC &= ~(1<<3);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTC &= ~(1<<0);
	PORTC |= (1<<1);
	PORTC |= (1<<2);
	PORTC &= ~(1<<3);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTC |= (1<<0);
	PORTC |= (1<<1);
	PORTC |= (1<<2);
	PORTC &= ~(1<<3);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTC &= ~(1<<0);
	PORTC &= ~(1<<1);
	PORTC &= ~(1<<2);
	PORTC |= (1<<3);
	_delay_ms(testDelay);
	
	ControlBlink();
	
	PORTC |= (1<<0);
	PORTC &= ~(1<<1);
	PORTC &= ~(1<<2);
	PORTC |= (1<<3);
	_delay_ms(testDelay);
	
}

void ControlBlink()
{	
	for(int i = 1; i < /*counter*/5; i++)
	{
		PORTD &= ~(1<<4);
		_delay_ms(100);
		PORTD |= (1<<4);
		_delay_ms(100);
	}
}

ISR(INT1_vect)
{
	if(counter < 3)
		++counter;	
}

ISR(INT0_vect)
{
	if(counter)
		--counter;
}

