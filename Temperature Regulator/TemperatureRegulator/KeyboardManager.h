/* 
* KeyboardManager.h
*
* Created: 2020-02-22 19:35:39
* Author: Gabriel
*/


#ifndef __KEYBOARDMANAGER_H__
#define __KEYBOARDMANAGER_H__

#include <avr/io.h>

#define PRESSED_START_DELAY 1000000 //1s
#define PRESSED_TIME 100000 //100ms

class KeyboardManager
{
public:
	enum E_KEY_CODE
	{
		K_LEFT = 0,
		K_ESC,
		K_OK,
		K_RIGHT,
		K_LAST,
	};

private:
	bool m_bLastKeyState[4];
	E_KEY_CODE m_eKeyToProcess;
	unsigned long m_ulKeyPressedTime;
	bool m_bContinuePressed;
	
	bool ReadKeyState(unsigned char _ucKey);
	void SendBeep();
	
public:
	static KeyboardManager& GetInstance()
	{
		// instance
		static KeyboardManager instance;
	
		return instance;
	}

	void Configure();
	void Process();
	E_KEY_CODE GetKeyToProcess();

private:
	KeyboardManager();
	~KeyboardManager();
	KeyboardManager( const KeyboardManager &c );
	KeyboardManager& operator=( const KeyboardManager &c );
};

#endif //__KEYBOARDMANAGER_H__
