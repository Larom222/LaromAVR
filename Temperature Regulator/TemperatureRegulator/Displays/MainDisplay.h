/* 
* MainDisplay.h
*
* Created: 2020-02-22 16:34:17
* Author: Gabriel
*/


#ifndef __MAINDISPLAY_H__
#define __MAINDISPLAY_H__

#include "DisplayWindow.h"
#include "../../../_Libraries/RTC_Manager/RTC_Manager.h"
#include "../../../_Libraries/LCD_Display/LCD_Display.h"
#include "../TemperatureController.h"

class MainDisplay
{
	enum E_STATE
	{
		S_IDLE = 0,
		S_REPAINT,
		S_REPAINT_FIRST_LINE,
		S_REPAINT_SECOND_LINE,
	};
	
	enum E_WRITE_STATE
	{
		WS_IDLE = 0,
		WS_WRITE_MIN_REGISTER,
		WS_WRITE_MIN,
		WS_WRITE_MAX_REGISTER,
		WS_WRITE_MAX,
		WS_WRITE_MODE_REGISTER,
		WS_WRITE_MODE,
	};
	
public:
	MainDisplay();
	~MainDisplay();
	
	//return true if processing finished
	void Finished();

	//process function
	void Process();

private:
	unsigned short m_usActualTemp;
	unsigned char m_ucMinTemp;
	unsigned char m_ucMaxTemp;
	unsigned char m_ucTempToSet;
	unsigned long m_ulLastBlink;
	bool m_bBlink;
	DateTime m_actualDate;
	bool m_eMode; //manual or auto
	bool m_bHeating;
	
	bool m_bMinEdit;
	bool m_bMaxEdit;
	
	E_STATE m_eState;
	E_WRITE_STATE m_eWriteState;
	char m_lineBuf[16];
};

#endif //__MAINDISPLAY_H__
