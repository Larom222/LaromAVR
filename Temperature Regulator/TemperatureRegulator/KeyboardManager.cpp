/* 
* KeyboardManager.cpp
*
* Created: 2020-02-22 19:35:39
* Author: Gabriel
*/


#include "KeyboardManager.h"
#include "../../_Libraries/Sound_Manager/SoundManager.h"
#include "../../_Libraries/Time/Time.h"

KeyboardManager::KeyboardManager()
{
	m_ulKeyPressedTime = 0;
	m_bContinuePressed = false;
	m_eKeyToProcess = K_LAST;
	for(int i = 0; i < K_LAST; i++)
	{
		m_bLastKeyState[i] = false;
	}
	Configure();
}

KeyboardManager::~KeyboardManager()
{
}

void KeyboardManager::Configure()
{	
	//PB6 | PB7 | PB2 | PB1
	//LEFT| ESC | OK  | RIGHT
	DDRB &= ~(1 << PB6 | 1 << PB7 | 1 << PB2 | 1 << PB1); //buttons set as input
}

KeyboardManager::E_KEY_CODE KeyboardManager::GetKeyToProcess()
{
	E_KEY_CODE ret = m_eKeyToProcess;
	m_eKeyToProcess = K_LAST;
	return ret;
}

bool KeyboardManager::ReadKeyState(unsigned char _ucKey)
{
	switch(_ucKey)
	{
		case K_LEFT:
			return PINB & (1 << PB6);
		case K_ESC:
			return PINB & (1 << PB7);
		case K_OK:
			return PINB & (1 << PB2);
		case K_RIGHT:
			return PINB & (1 << PB1);
	}
	return false;
}

void KeyboardManager::Process()
{
	bool bKeyStates[K_LAST];

	//read keys state
	for(unsigned char i = 0; i < K_LAST; i++)
	{
		bKeyStates[i] = ReadKeyState(i);
	}

	//detect pressed action
	for(unsigned char i = 0; i < K_LAST; i++)
	{
		if(m_bLastKeyState[i] == false && bKeyStates[i] == true)
		{
			m_eKeyToProcess = (E_KEY_CODE)i;
			m_ulKeyPressedTime = Time::GetInstance().GetUS();
			SendBeep();
			m_bContinuePressed = false;
			break;
		}
		else if(m_bLastKeyState[i] == true && bKeyStates[i] == true)
		{
			unsigned long ulCurrentTime = Time::GetInstance().GetUS();
			if(ulCurrentTime - m_ulKeyPressedTime >= (m_bContinuePressed ? PRESSED_TIME : PRESSED_START_DELAY) )
			{
				m_eKeyToProcess = (E_KEY_CODE)i;
				m_ulKeyPressedTime = Time::GetInstance().GetUS();
				SendBeep();
				m_bContinuePressed = true;
			}
			break;
		}
	}

	//save last keys state
	for(unsigned char i = 0; i < K_LAST; i++)
	{
		m_bLastKeyState[i] = bKeyStates[i];
	}
}

void KeyboardManager::SendBeep()
{
	SoundManager::GetInstance().Play();
}
