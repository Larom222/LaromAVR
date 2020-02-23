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
	
	
	//enable interrupts
	//sei();
	
	Time::GetInstance().Configure();
	Manchester::GetInstance().ConfigureTx();
			
	secondsTime = displayTime = currentTime = Time::GetInstance().GetUS();
	seconds = 0;
	Display4x7Seg::GetInstance().SetNumber(0);
	
	//Manchester::GetInstance().SendMessage(1, 222);
	
		
    /* Replace with your application code */
    while (1) 
    {				
		currentTime = Time::GetInstance().GetUS();

		/*
		if(currentTime >= secondsTime + 1000000)
		{
			secondsTime = currentTime;
			Display4x7Seg::GetInstance().SetNumber(++seconds);			
		}
		*/
		
		
		if(DebugTools::GetInstance().m_bError)
		{
			//Display4x7Seg::GetInstance().SetNumber(666);
		}
		else
		{
			//Display4x7Seg::GetInstance().SetNumber(DebugTools::GetInstance().m_iDebugCounter);
			
			/*
			if(Manchester::GetInstance().Debug_GetState())
				Display4x7Seg::GetInstance().SetNumber(1);	
			else
				Display4x7Seg::GetInstance().SetNumber(0);
				*/
		}
		
		//Display4x7Seg::GetInstance().SetNumber(Time::GetInstance().GetBadReading());
		
		//Display4x7Seg::GetInstance().SetNumber(sizeof(long long));
		
		//Display4x7Seg::GetInstance().ShowNumber(number);		
		if(currentTime >= displayTime + 2000) //refresh display every 2ms
		{
			displayTime = currentTime;
			Display4x7Seg::GetInstance().ShowNextDigit();
		}
		/*else if(currentTime < displayTime)
		{
			Display4x7Seg::GetInstance().ShowNextDigit();
		}*/
		
		/*
		if(currentTime < displayTime)
		{
			if(!error1)
			{			
				error1 = currentTime;
				error2 = displayTime;
			}
			Display4x7Seg::GetInstance().SetNumber(++seconds);
			//Display4x7Seg::GetInstance().SetNumber(currentTime / 10000);
		}*/
		
		
		if(DebugTools::GetInstance().m_bError == true)
		{
			Display4x7Seg::GetInstance().SetNumber(DebugTools::GetInstance().m_iDebugCounter);
		}
		
		if(Manchester::GetInstance().Debug_GetState() == true)
			Display4x7Seg::GetInstance().SetNumber(1);
		else
			Display4x7Seg::GetInstance().SetNumber(0);
				
		//_delay_ms(1000);
		/*if(Manchester::GetInstance().SendMessage(1, 222))
			Display4x7Seg::GetInstance().SetNumber(++number);
		Manchester::GetInstance().ProcessTx();*/
		
		Manchester::GetInstance().SendMessage(1, 222);
		Manchester::GetInstance().ProcessTx();
    }
}

/*
ISR (TIMER0_OVF_vect)
{
	TCNT0 += 130; //125 + 130 = 255 (interrupt every 1ms)
	Display4x7Seg::GetInstance().ShowNextDigit();
}
*/

ISR(INT1_vect)
{
	DebugTools::GetInstance().m_bError = false;
	DebugTools::GetInstance().m_iDebugCounter = 0;
}

