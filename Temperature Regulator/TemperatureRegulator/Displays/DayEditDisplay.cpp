/* 
* DayEditDisplay.cpp
*
* Created: 2020-02-23 16:31:56
* Author: Gabriel
*/


#include "DayEditDisplay.h"
#include "../../../_Libraries/LCD_Display/LCD_Display.h"
#include "../../../_Libraries/RTC_Manager/RTC_Manager.h"
#include "../../../_Libraries/EEPROM_Manager/EEPROM_Manager.h"
#include "../Config.h"
#include "../KeyboardManager.h"
#include "../TemperatureController.h"

// default constructor
DayEditDisplay::DayEditDisplay()
{	
	m_ucDayOfWeek = MIN_DAY_OF_WEEK;
	m_bFinished = true;
	m_ucMinTemp = DEFAULT_MIN_TEMP;
	m_ucMaxTemp = DEFAULT_MAX_TEMP;
	m_ulLastBlink = 0;
	m_eState = S_INIT_DISPLAY;
	m_eEditingPart = EP_MIN;
	m_eSaveStep = ST_CLEAR_DISPLAY;
	m_ucSaveHour = 0;
} //DayEditDisplay

// default destructor
DayEditDisplay::~DayEditDisplay()
{
} //~DayEditDisplay

void DayEditDisplay::Show(unsigned char _ucDayOfWeek)
{
	m_ucDayOfWeek = _ucDayOfWeek;
	m_bFinished = false;
	m_ucMinTemp = DEFAULT_MIN_TEMP;
	m_ucMaxTemp = DEFAULT_MAX_TEMP;
	m_ulLastBlink = 0;
	m_bBlink = false;
	m_eState = S_INIT_DISPLAY;
	m_eEditingPart = EP_MIN;
	m_eSaveStep = ST_CLEAR_DISPLAY;
	m_ucSaveHour = 0;
}

bool DayEditDisplay::Finished()
{
	return m_bFinished;
}

void DayEditDisplay::Process()
{
	//Keyboard process
	if(m_eState != S_SAVE_TEMPERATURE)
	{
		KeyboardManager::E_KEY_CODE eKeyToProcess = KeyboardManager::GetInstance().GetKeyToProcess();
		if(eKeyToProcess != KeyboardManager::K_LAST)
		{
			switch(eKeyToProcess)
			{
				case KeyboardManager::K_LEFT:
					DecreaseActualPart();
					m_eState = S_REPAINT_TEMP;
					break;

				case KeyboardManager::K_ESC:
					m_bFinished = true;
					break;

				case KeyboardManager::K_OK:
					if(m_eEditingPart == EP_MIN)
						m_eEditingPart = EP_MAX;
					else
					{
						m_eState = S_SAVE_TEMPERATURE;
						m_eSaveStep = ST_CLEAR_DISPLAY;
						m_ucSaveHour = 0;
					}
					break;

				case KeyboardManager::K_RIGHT:
					IncreaseActualPart();
					m_eState = S_REPAINT_LABEL;
					break;
				default:
					break;
			}
		}
	}

	//GUI loop
	switch(m_eState)
	{
		case S_INIT_DISPLAY:
			if(DisplayLCD::GetInstance().ClearDisplay())
				m_eState = S_REPAINT_LABEL;
			break;
		case S_REPAINT_LABEL:
			if(DisplayLCD::GetInstance().GetAvailableTextLength() > 11)
			{
				char strDayOfWeek[2];
				DateTime::DayOfWeekToString(m_ucDayOfWeek, strDayOfWeek);
				DisplayLCD::GetInstance().WriteText("MIN", FIRST_LINE_ADDRESS, 3);
				DisplayLCD::GetInstance().WriteText("MAX", FIRST_LINE_ADDRESS + 13, 3);
				DisplayLCD::GetInstance().WriteText(strDayOfWeek, FIRST_LINE_ADDRESS + 7, 2);
				m_eState = S_REPAINT_TEMP;
			}
			break;
		case S_REPAINT_TEMP:
			{
				char num[2];
				num[0] = m_ucMinTemp / 10 + 48;
				num[1] = m_ucMinTemp % 10 + 48;
				
				if(m_eEditingPart == EP_MIN && m_bBlink)
					DisplayLCD::GetInstance().WriteText("  ", SECOND_LINE_ADDRESS, 2);
				else				
					DisplayLCD::GetInstance().WriteText(num, SECOND_LINE_ADDRESS, 2);
				
				num[0] = m_ucMaxTemp / 10 + 48;
				num[1] = m_ucMaxTemp % 10 + 48;
				
				if(m_eEditingPart == EP_MAX && m_bBlink)
					DisplayLCD::GetInstance().WriteText("  ", SECOND_LINE_ADDRESS + 14, 2);
				else
					DisplayLCD::GetInstance().WriteText(num, SECOND_LINE_ADDRESS + 14, 2);
					
				m_eState = S_IDLE;
				break;
			}
		case S_SAVE_TEMPERATURE:
			switch(m_eSaveStep)
			{
				case ST_CLEAR_DISPLAY:
					if(DisplayLCD::GetInstance().ClearDisplay())
					{
						m_eSaveStep = ST_WRITE_WAIT_MESSAGE;
						break;
					}
					break;
				case ST_WRITE_WAIT_MESSAGE:
					if(DisplayLCD::GetInstance().GetAvailableTextLength() > 11)
					{
						DisplayLCD::GetInstance().WriteText("TRWA ZAPIS", FIRST_LINE_ADDRESS, 10);
						m_eSaveStep = ST_WRITE_MIN_TEMP;
					}
					break;
				case ST_WRITE_MIN_TEMP:
					if(EEPROMManager::GetInstance().Register())
					{
						if(EEPROMManager::GetInstance().WriteByte(EEPROM_AUTO_MODE_MIN_PAGE, m_ucDayOfWeek * 24 + m_ucSaveHour, m_ucMinTemp))
						{
							m_eSaveStep = ST_WRITE_MAX_TEMP;
						}
						EEPROMManager::GetInstance().Unregister();				
					}
					break;
				case ST_WRITE_MAX_TEMP:
					if(EEPROMManager::GetInstance().Register())
					{
						if(EEPROMManager::GetInstance().WriteByte(EEPROM_AUTO_MODE_MAX_PAGE, m_ucDayOfWeek * 24 + m_ucSaveHour, m_ucMaxTemp))
						{
							if(m_ucSaveHour < MAX_HOUR)
							{
								++m_ucSaveHour;
								m_eSaveStep = ST_WRITE_MIN_TEMP;
							}
							else
							{								
								m_bFinished = true;
								TemperatureController::GetInstance().RefreshMinMax();
								m_eState = S_IDLE;
							}
						}
						EEPROMManager::GetInstance().Unregister();
					}
					break;
			}
			break;
		case S_IDLE:
			unsigned long currentTime = Time::GetInstance().GetUS();
			if(currentTime - m_ulLastBlink > BLINK_TIME)
			{
				m_ulLastBlink = currentTime;
				m_bBlink = !m_bBlink;
				m_eState = S_REPAINT_TEMP;
			}
			break;
	}
}


void DayEditDisplay::DecreaseActualPart()
{
	if(m_eEditingPart == EP_MIN && m_ucMinTemp > MIN_TEMP)
		--m_ucMinTemp;
		
	if(m_eEditingPart == EP_MAX && m_ucMaxTemp > MIN_TEMP && m_ucMaxTemp > m_ucMinTemp + 1)
		--m_ucMaxTemp;
		
	m_eState = S_REPAINT_TEMP;
}

void DayEditDisplay::IncreaseActualPart()
{
	if(m_eEditingPart == EP_MIN && m_ucMinTemp < MAX_TEMP - 1)
	{
		++m_ucMinTemp;
		if(m_ucMinTemp >= m_ucMaxTemp)
			m_ucMaxTemp = m_ucMinTemp + 1;
	}
	
	if(m_eEditingPart == EP_MAX && m_ucMaxTemp < MAX_TEMP)
		++m_ucMaxTemp;
		
	m_eState = S_REPAINT_TEMP;
}