/* 
* Time.h
*
* Created: 2019-11-16 17:10:41
* Author: Gabriel
*/


#ifndef __TIME_H__
#define __TIME_H__
	
#include <avr/io.h>
//#include <util/delay.h>
#include <avr/interrupt.h>
#include "Time_pin_config.h"

#if TIMER0_MODE == COMPARATOR
	ISR (TIMER0_COMPA_vect);
#else
	ISR (TIMER0_OVF_vect);
#endif

class Time
{
#if TIMER0_MODE == COMPARATOR
	friend void TIMER0_COMPA_vect(void);
#else
	friend void TIMER0_OVF_vect(void);
#endif
//variables
public:
protected:
private:
	bool m_bConfigured;
	volatile unsigned long m_u32Time; //max 4294967296s
	volatile bool m_bValid;
	volatile bool m_bReading; 
	
	//int badReading;
	
//functions
public:	
	static Time& GetInstance()
	{
		// instance
		static Time instance;
		
		return instance;
	}
	
	//int GetBadReading(){return badReading;}
	
	// Set timer0 and interrupt
	void Configure();
	unsigned long GetUS();
protected:
private:
	Time();
	~Time();
	Time( const Time &c );
	Time& operator=( const Time &c );
	
	void IncreaseMs();
	void IncreaseUs(unsigned int us);

}; //Time


#endif //__TIME_H__
