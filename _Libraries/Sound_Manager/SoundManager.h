/* 
* SoundManager.h
*
* Created: 2019-11-07 21:06:28
* Author: Gabriel
*/


#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

#include "SoundManager_pin_config.h"
#include "../../_Libraries/Time/Time.h"
#include <avr/io.h>
#include <util/delay.h>

#define SOUND_TIME 50000

ISR (TIMER2_COMPA_vect);

class SoundManager
{
	friend void TIMER2_COMPA_vect(void);
	
//variables
public:
protected:
private:
	bool m_bConfigured;
	bool m_bPlay;
	bool m_bPinState;
	unsigned long m_ulStartTime;

//functions
public:	
	// get display instance
	static SoundManager& GetInstance()
	{		
		// instance
		static SoundManager instance;
		
		return instance;
	}
	
	void Configure();
	void Process();
	void Play();
	
protected:
private:
	SoundManager();
	~SoundManager();
	SoundManager( const SoundManager &c );
	SoundManager& operator=( const SoundManager &c );
	
}; //SoundManager

#endif //__SOUND_MANAGER_H__
