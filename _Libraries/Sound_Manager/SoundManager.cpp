/* 
* SoundManager.cpp
*
* Created: 2020-03-08 16:06:28
* Author: Gabriel
*/

#include "SoundManager.h"

// default constructor
SoundManager::SoundManager()
{
	m_bConfigured = false;
	m_bPlay = false;
	m_ulStartTime = 0;
	m_bPinState = false;
}

// default destructor
SoundManager::~SoundManager()
{
}

// Config ports for MUX and display
void SoundManager::Configure()
{
	if(!m_bConfigured)
	{
		cli();				
		SOUND_DDR |= (1 << SOUND_PIN);
		
		// enable timer0 interrupt
		TCCR2A |= (1 << WGM01);	// Set the Timer Mode to CTC
		TIMSK2 |= (1 << OCIE2A);
		TCCR2B |= (1 << CS01); // timer0 prescaler = 8
		OCR2A = 0x70; //interrupt every 100us
		
		//enable interrupts
		sei();
	}
	
	m_bConfigured = true;
}

void SoundManager::Play()
{
	m_ulStartTime = Time::GetInstance().GetUS();
	m_bPlay = true;
}

void SoundManager::Process()
{
	unsigned long ulCurrentTime = Time::GetInstance().GetUS();
	if(ulCurrentTime - m_ulStartTime > SOUND_TIME)
	{
		m_bPlay = false;
		SOUND_PORT &= ~(1 << SOUND_PIN);
	}
}

ISR (TIMER2_COMPA_vect)
{
	if(SoundManager::GetInstance().m_bPlay)
	{
		if(SoundManager::GetInstance().m_bPinState)
		{
			SOUND_PORT &= ~(1 << SOUND_PIN);
		}
		else
		{
			SOUND_PORT |= (1 << SOUND_PIN);
		}
		SoundManager::GetInstance().m_bPinState = !SoundManager::GetInstance().m_bPinState;
	}
	else
	{
		SOUND_PORT &= ~(1 << SOUND_PIN);
	}
}