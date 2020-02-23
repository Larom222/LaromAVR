/* 
* HourEditDisplay.cpp
*
* Created: 2020-02-23 17:49:46
* Author: Gabriel
*/


#include "HourEditDisplay.h"
#include "../../../_Libraries/LCD_Display/LCD_Display.h"
#include "../../../_Libraries/RTC_Manager/RTC_Manager.h"
#include "../../../_Libraries/EEPROM_Manager/EEPROM_Manager.h"
#include "../Config.h"
#include "../KeyboardManager.h"
#include "../TemperatureController.h"

// default constructor
HourEditDisplay::HourEditDisplay()
{
	m_ucDayOfWeek = MIN_DAY_OF_WEEK;
	m_bFinished = true;
	m_ucMinTemp = DEFAULT_MIN_TEMP;
	m_ucMaxTemp = DEFAULT_MAX_TEMP;
	m_ulLastBlink = 0;
	m_eState = S_INIT_DISPLAY;
	m_eEditingPart = EP_MIN;
	m_ucSelectedHour = 12;
	m_bEditMode = false;
} //HourEditDisplay

// default destructor
HourEditDisplay::~HourEditDisplay()
{
} //~HourEditDisplay

void HourEditDisplay::Show(unsigned char _ucDayOfWeek)
{
	m_ucDayOfWeek = _ucDayOfWeek;
	m_bFinished = false;
	m_ucMinTemp = DEFAULT_MIN_TEMP;
	m_ucMaxTemp = DEFAULT_MAX_TEMP;
	m_ulLastBlink = 0;
	m_bBlink = false;
	m_eState = S_INIT_DISPLAY;
	m_eEditingPart = EP_MIN;
	m_ucSelectedHour = 12;
	m_bEditMode = false;
}

bool HourEditDisplay::Finished()
{
	return m_bFinished;
}


void HourEditDisplay::Process()
{
	//Keyboard process
	if(m_eState == S_IDLE || m_eState == S_REPAINT)
	{
		KeyboardManager::E_KEY_CODE eKeyToProcess = KeyboardManager::GetInstance().GetKeyToProcess();
		if(eKeyToProcess != KeyboardManager::K_LAST)
		{
			switch(eKeyToProcess)
			{
				case KeyboardManager::K_LEFT:
					DecreaseActualPart();
					break;

				case KeyboardManager::K_ESC:
					if(m_bEditMode)
					{
						m_bEditMode = false;
						m_eState = S_READ_MIN_TEMP; //reload temperature
					}
					else
					{
						m_bFinished = true;
					}
					break;

				case KeyboardManager::K_OK:
					if(!m_bEditMode)
					{
						m_bEditMode = true;
						m_eEditingPart = EP_MIN;
					}
					else
					{
						if(m_eEditingPart == EP_MIN)
							m_eEditingPart = EP_MAX;
						else
						{
							m_eState = S_SAVE_MIN_TEMP;
							m_bEditMode = false;
						}
					}
					break;

				case KeyboardManager::K_RIGHT:
					IncreaseActualPart();
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
				m_eState = S_READ_MIN_TEMP;
			break;
		case S_READ_MIN_TEMP:
			if(EEPROMManager::GetInstance().Register())
			{
				if(EEPROMManager::GetInstance().ReadByte(EEPROM_AUTO_MODE_MIN_PAGE, m_ucDayOfWeek * 24 + m_ucSelectedHour))
				{
					m_eState = S_READ_MIN_TEMP_RES;
				}
				else
					EEPROMManager::GetInstance().Unregister();
			}
			break;
		case S_READ_MIN_TEMP_RES:
			if(EEPROMManager::GetInstance().ReadyToRead())
			{
				m_ucMinTemp = EEPROMManager::GetInstance().GetLastRead();
				m_eState = S_READ_MAX_TEMP;
				EEPROMManager::GetInstance().Unregister();
			}
			break;
		case S_READ_MAX_TEMP:
			if(EEPROMManager::GetInstance().Register())
			{
				if(EEPROMManager::GetInstance().ReadByte(EEPROM_AUTO_MODE_MAX_PAGE, m_ucDayOfWeek * 24 + m_ucSelectedHour))
				{
					m_eState = S_READ_MAX_TEMP_RES;
				}
				else
					EEPROMManager::GetInstance().Unregister();
			}
			break;
		case S_READ_MAX_TEMP_RES:
			if(EEPROMManager::GetInstance().ReadyToRead())
			{
				m_ucMaxTemp = EEPROMManager::GetInstance().GetLastRead();
				m_eState = S_REPAINT;
				EEPROMManager::GetInstance().Unregister();
			}
			break;
			
		case S_REPAINT:
			if(DisplayLCD::GetInstance().GetAvailableTextLength() > 32)
			{
				char strDayOfWeek[3];
				if(m_bEditMode)
					strDayOfWeek[0] = ' ';
				else
					strDayOfWeek[0] = '<';
				DateTime::DayOfWeekToString(m_ucDayOfWeek, strDayOfWeek + 1);
				DisplayLCD::GetInstance().WriteText(strDayOfWeek, FIRST_LINE_ADDRESS, 3);
				
				DateTime tmpTime;
				char strTime[5];
				tmpTime.m_hour = m_ucSelectedHour;
				tmpTime.m_minute = MIN_MINUTE;
				tmpTime.TimeToString(strTime);
				DisplayLCD::GetInstance().WriteText(strTime, FIRST_LINE_ADDRESS + 4, 5);
				
				tmpTime.m_minute = MAX_MINUTE;
				tmpTime.TimeToString(strTime);
				DisplayLCD::GetInstance().WriteText(strTime, FIRST_LINE_ADDRESS + 10, 5);
				DisplayLCD::GetInstance().WriteText("-", FIRST_LINE_ADDRESS + 9, 1);
				
				char temp[2];
				temp[0] = m_ucMinTemp / 10 + 48;
				temp[1] = m_ucMinTemp % 10 + 48;
				if(m_bEditMode && (m_eEditingPart == EP_MIN) && m_bBlink)
					DisplayLCD::GetInstance().WriteText("  ", SECOND_LINE_ADDRESS + 5, 2);
				else
					DisplayLCD::GetInstance().WriteText(temp, SECOND_LINE_ADDRESS + 5, 2);
				
				temp[0] = m_ucMaxTemp / 10 + 48;
				temp[1] = m_ucMaxTemp % 10 + 48;
				if(m_bEditMode && (m_eEditingPart == EP_MAX) && m_bBlink)
					DisplayLCD::GetInstance().WriteText("  ", SECOND_LINE_ADDRESS + 8, 2);
				else
					DisplayLCD::GetInstance().WriteText(temp, SECOND_LINE_ADDRESS + 8, 2);
				DisplayLCD::GetInstance().WriteText("-", SECOND_LINE_ADDRESS + 7, 1);
				
				if(m_bEditMode)
				{					
					DisplayLCD::GetInstance().WriteText(" ", SECOND_LINE_ADDRESS, 1);
					DisplayLCD::GetInstance().WriteText(" ", SECOND_LINE_ADDRESS + 15, 1);
					DisplayLCD::GetInstance().WriteText(" ", FIRST_LINE_ADDRESS + 15, 1);
				}
				else
				{					
					DisplayLCD::GetInstance().WriteText("<", SECOND_LINE_ADDRESS, 1);
					DisplayLCD::GetInstance().WriteText(">", SECOND_LINE_ADDRESS + 15, 1);
					DisplayLCD::GetInstance().WriteText(">", FIRST_LINE_ADDRESS + 15, 1);
				}
				m_eState = S_IDLE;
			}
			break;		
		case S_SAVE_MIN_TEMP:
			if(EEPROMManager::GetInstance().Register())
			{
				if(EEPROMManager::GetInstance().WriteByte(EEPROM_AUTO_MODE_MIN_PAGE, m_ucDayOfWeek * 24 + m_ucSelectedHour, m_ucMinTemp))
				{
					m_eState = S_SAVE_MAX_TEMP;
				}
				EEPROMManager::GetInstance().Unregister();
			}
			break;
		case S_SAVE_MAX_TEMP:
			if(EEPROMManager::GetInstance().Register())
			{
				if(EEPROMManager::GetInstance().WriteByte(EEPROM_AUTO_MODE_MAX_PAGE, m_ucDayOfWeek * 24 + m_ucSelectedHour, m_ucMaxTemp))
				{
					m_eState = S_READ_MIN_TEMP;
					TemperatureController::GetInstance().RefreshMinMax();
				}
				EEPROMManager::GetInstance().Unregister();
			}
			break;
		case S_IDLE:
			unsigned long currentTime = Time::GetInstance().GetUS();
			if(m_bEditMode && currentTime - m_ulLastBlink > BLINK_TIME)
			{
				m_ulLastBlink = currentTime;
				m_bBlink = !m_bBlink;
				m_eState = S_REPAINT;
			}
			break;
	}
}


void HourEditDisplay::DecreaseActualPart()
{
	if(m_bEditMode)
	{
		if(m_eEditingPart == EP_MIN && m_ucMinTemp > MIN_TEMP)
			--m_ucMinTemp;
		
		if(m_eEditingPart == EP_MAX && m_ucMaxTemp > MIN_TEMP && m_ucMaxTemp > m_ucMinTemp + 1)
			--m_ucMaxTemp;
		
		m_eState = S_REPAINT;
	}
	else if(m_ucSelectedHour > MIN_HOUR)
	{
		--m_ucSelectedHour;
		m_eState = S_READ_MIN_TEMP;
	}
}

void HourEditDisplay::IncreaseActualPart()
{
	if(m_bEditMode)
	{
		if(m_eEditingPart == EP_MIN && m_ucMinTemp < MAX_TEMP - 1)
		{
			++m_ucMinTemp;
			if(m_ucMinTemp >= m_ucMaxTemp)
			m_ucMaxTemp = m_ucMinTemp + 1;
		}
		
		if(m_eEditingPart == EP_MAX && m_ucMaxTemp < MAX_TEMP)
			++m_ucMaxTemp;
		m_eState = S_REPAINT;
	}
	else if(m_ucSelectedHour < MAX_HOUR)
	{
		++m_ucSelectedHour;
		m_eState = S_READ_MIN_TEMP;
	}
}