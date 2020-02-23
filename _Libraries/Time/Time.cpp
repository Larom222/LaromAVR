/* 
* Time.cpp
*
* Created: 2019-11-16 17:10:40
* Author: Gabriel
*/


#include "Time.h"
//#include "../../_Libraries/DebugTools/DebugTools.h"

// default constructor
Time::Time()
{
	m_bConfigured = false;
	m_u32Time = 0;
	m_bValid = true;
	m_bReading = false;
	
	//badReading = 0;
} //Time

// default destructor
Time::~Time()
{
} //~Time

// Set timer0 and interrupt
void Time::Configure()
{	
	if(!m_bConfigured)
	{
		cli();
		
		//1 000 000 [Hz] / 1000[ms] = 1000 [tick / ms]
		//1000 / 8 = 125
		//125 + 130 = 255 (interrupt every 1ms)
		
		//8 000 000 [Hz] / 1000[ms] = 8000 [tick / ms]
		//8000 / 8 = 1000
		//5 + 250 = 255 (interrupt every 250us)
		
		//125 / 2 = 62
		//62 + 193 = 255 (interrupt every 500us)
				
		// enable timer0 interrupt
#if TIMER0_MODE == COMPARATOR
		TCCR0A |= (1 << WGM01);	// Set the Timer Mode to CTC
		TIMSK0 |= (1 << OCIE0A);
		TCCR0B |= (1 << CS01); // timer0 prescaler = 8
		OCR0A = 0x60; //interrupt every 100us
		//OCR0A = 0xFF; //interrupt every 100us
#else
		TIMSK |= (1 << TCNT0);
		TCCR0 |= (1 << CS01); //timer0 prescaler = 8
#endif
		
		//enable interrupts
		sei();
	}
	
	m_bConfigured = true;
}

unsigned long Time:: GetUS()
{
	if(!m_bConfigured)
		return 0;
		
	unsigned long retTime;
	
	do 
	{
		m_bValid = true;
		m_bReading = true;
		retTime = m_u32Time;// + TCNT0 * 8L;
		m_bReading = false;
		
		//if(!m_bValid)
		{
			//++badReading;
			//DebugTools::GetInstance().m_bError = true;
			//DebugTools::GetInstance().m_iDebugCounter++;
		}
	} while (m_bValid == false);
	
	
	return retTime;
}

#if TIMER0_MODE == COMPARATOR
ISR (TIMER0_COMPA_vect)
#else
ISR (TIMER0_OVF_vect)
#endif
{
	//Time::GetInstance().IncreaseMs();
	//TCNT0 += 130; //125 + 130 = 255 (interrupt every 1ms)
	
	//Time::GetInstance().IncreaseUs(500);
	//TCNT0 += 193; //62 + 193 = 255 (interrupt every 500us)
	
	//8MHz	
	Time::GetInstance().IncreaseUs(100);
#if TIMER0_MODE == INTERRUPT
	TCNT0 += 5 + 150; //5 + 250 = 255 (interrupt every 250us)
#endif
}

void Time::IncreaseMs()
{	
	if(m_bReading)
		m_bValid = false;
	
	m_u32Time += 1000;
}

void Time::IncreaseUs(unsigned int us)
{
	if(m_bReading)
		m_bValid = false;
	
	m_u32Time += us;
}