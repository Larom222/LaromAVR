/*
 * BlinkTest.cpp
 *
 * Created: 2019-11-01 11:17:09
 * Author : Gabriel
 */ 

#define F_CPU 8000000UL //8 MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../../_Libraries/Display4x7Seg/Display4x7Seg.h"
#include "../../_Libraries/Time/Time.h"
#include "../../_Libraries/Manchester/Manchester.h"
#include "../../_Libraries/DebugTools/DebugTools.h"

int counter = 1;
int overflow = 0;
int number = 0;

unsigned long secondsTime = 0;
unsigned long displayTime = 0;
unsigned long currentTime = 0;
int seconds = 0;

unsigned long error1 = 0;
unsigned long error2 = 0;

unsigned char messageId = 0;
unsigned char messageVal = 0;

int main(void)
{
	counter = 0;
	
	DebugTools::GetInstance().m_bError = false;
		
	// LED output
	//DDRD |= (1<<4);
		
	// EXT INT1 conf - falling edge
	MCUCR |= (1<<3);
	GICR |= (1<<7);
	
	// EXT INT0 conf - falling edge
	//MCUCR |= (1<<1);
	//GICR |= (1<<6);
	
	//INT1 as input
	DDRD &= ~(1<<3);
	PORTD |= (1<<3);
	
	//INT0 as input
	//DDRD &= ~(1<<2);
	//PORTD |= (1<<2);
			
	//Display4x7Seg::GetInstance().SetTimer();
	
	
	// timer0 8 prescaler
	//TCCR0 |= (1 << CS01);
	
	// enable timer0 interrupt
	//TIMSK |= (1 << TOIE0);
	
	
	
	Time::GetInstance().Configure();
	Manchester::GetInstance().ConfigureRx();
			
	//enable interrupts
	sei();
			
	secondsTime = displayTime = currentTime = Time::GetInstance().GetUS();
	seconds = 0;
	Display4x7Seg::GetInstance().SetNumber(0);
		
    /* Replace with your application code */
    while (1) 
    {		
		currentTime = Time::GetInstance().GetUS();
					
		if(currentTime >= displayTime + 2000) //refresh display every 2ms
		{
			displayTime = currentTime;
			Display4x7Seg::GetInstance().ShowNextDigit();
		}
		
		/*if(currentTime >= secondsTime + 1000000) //refresh display every 2ms
		{
			secondsTime = currentTime;
			Display4x7Seg::GetInstance().SetNumber(seconds++);
		}*/
		
		
		Manchester::GetInstance().ProcessRx();
		
			
		if(DebugTools::GetInstance().m_bError)
		{			
			if( DebugTools::GetInstance().m_iDebugCounter > 9999)
				DebugTools::GetInstance().m_iDebugCounter = 0;
				
			seconds = DebugTools::GetInstance().m_iDebugCounter2 * 1000 + DebugTools::GetInstance().m_iDebugCounter;
			Display4x7Seg::GetInstance().SetNumber(seconds);
		}
		
		if(Manchester::GetInstance().ReadMessage(messageId, messageVal))
		{
			//DebugTools::GetInstance().m_bError = true;
			//DebugTools::GetInstance().m_iDebugCounter++;
			seconds = (int)messageVal + 1000 * messageId;
			Display4x7Seg::GetInstance().SetNumber(seconds);
			//DebugTools::GetInstance().m_bError = false;
		}
    }
}

ISR(INT1_vect)
{
	DebugTools::GetInstance().m_bError = false;
	DebugTools::GetInstance().m_iDebugCounter = 0;
	//Manchester::GetInstance().Debug_Clear();
}

/*
ISR(INT0_vect)
{
	DebugTools::GetInstance().m_bError = true;
	DebugTools::GetInstance().m_iDebugCounter++;
}
*/