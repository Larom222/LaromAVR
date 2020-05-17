/* 
* DateTimeEditDisplay.cpp
*
* Created: 2020-02-23 13:56:32
* Author: Gabriel
*/


#include "DateTimeEditDisplay.h"
#include "../../../_Libraries/LCD_Display/LCD_Display.h"
#include "../TemperatureController.h"
#include "../KeyboardManager.h"
#include "../../../_Libraries/DebugTools/DebugTools.h"

// default constructor
DateTimeEditDisplay::DateTimeEditDisplay()
{
	m_bFinished = true;
	m_eState = S_IDLE;
	m_eEditingPart = EP_DAY;
	m_ulLastBlink = 0;
	m_bBlink = false;
} //DateTimeEditDisplay

// default destructor
DateTimeEditDisplay::~DateTimeEditDisplay()
{
} //~DateTimeEditDisplay

void DateTimeEditDisplay::Show()
{
	m_bFinished = false;
	m_eState = S_INIT_DISPLAY;
	m_eEditingPart = EP_DAY;
	m_ulLastBlink = 0;
	m_bBlink = false;
	m_dateTime = TemperatureController::GetInstance().GetDateTime();
	m_dateTime.m_second = 0;
}

bool DateTimeEditDisplay::Finished()
{
	return m_bFinished;
}

void DateTimeEditDisplay::Process()
{
	//Keyboard process
	KeyboardManager::E_KEY_CODE eKeyToProcess = KeyboardManager::GetInstance().GetKeyToProcess();
	if(eKeyToProcess != KeyboardManager::K_LAST)
	{
		switch(eKeyToProcess)
		{
			case KeyboardManager::K_LEFT:
				DecreaseActualPart();
				m_eState = S_REPAINT;
				break;

			case KeyboardManager::K_ESC:
				m_bFinished = true;
				break;

			case KeyboardManager::K_OK:
				if(m_eEditingPart < EP_MINUTE)
					++m_eEditingPart;
				else
				{
					m_eState = S_SAVE_DATE_TIME;
				}
				break;

			case KeyboardManager::K_RIGHT:
				IncreaseActualPart();
				m_eState = S_REPAINT;
				break;
			default:
				break;
		}
	}

	//GUI loop
	switch(m_eState)
	{
		case S_INIT_DISPLAY:
			if(DisplayLCD::GetInstance().ClearDisplay())
				m_eState = S_REPAINT;
			break;
		case S_REPAINT:
			if(DisplayLCD::GetInstance().GetAvailableTextLength() > 18)
			{
				char strFirstLine[11];
				char strSecondLine[5];
				m_dateTime.DateToString(strFirstLine);
				m_dateTime.DayOfWeekToString(strFirstLine + 9);
				m_dateTime.TimeToString(strSecondLine);
				strFirstLine[8] = ' ';
				
				if(m_bBlink)
				{
					if(m_eEditingPart < EP_HOUR)
					{						
						strFirstLine[m_eEditingPart * 3] = ' ';
						strFirstLine[m_eEditingPart * 3 + 1] = ' ';
					}
					else
					{
						strSecondLine[(m_eEditingPart - EP_HOUR) * 3] = ' ';
						strSecondLine[(m_eEditingPart - EP_HOUR) * 3 + 1] = ' ';
					}
				}
				DisplayLCD::GetInstance().WriteText(strFirstLine, FIRST_LINE_ADDRESS, 11);
				DisplayLCD::GetInstance().WriteText(strSecondLine, SECOND_LINE_ADDRESS, 5);
				m_eState = S_IDLE;
			}
			break;
		case S_SAVE_DATE_TIME:
			if(RTCManager::GetInstance().Register())
			{
				if(RTCManager::GetInstance().WriteDateTime(m_dateTime))
				{
					m_eState = S_IDLE;
					m_bFinished = true;
					TemperatureController::GetInstance().RefreshDateTime();
				}
				RTCManager::GetInstance().Unregister();
			}
		case S_IDLE:
			unsigned long currentTime = Time::GetInstance().GetUS();
			if(currentTime - m_ulLastBlink > BLINK_TIME)
			{
				m_ulLastBlink = currentTime;
				m_bBlink = !m_bBlink;
				m_eState = S_REPAINT;
			}
			break;
	}
}

void DateTimeEditDisplay::IncreaseActualPart()
{
	switch(m_eEditingPart)
	{
		case EP_DAY:
			if(m_dateTime.m_day < MAX_DAY)
				++m_dateTime.m_day;
			else
				m_dateTime.m_day = MIN_DAY;
			break;
		case EP_MONTH:
			if(m_dateTime.m_month < MAX_MONTH)
				++m_dateTime.m_month;
			else
				m_dateTime.m_month = MIN_MONTH;
			break;
		case EP_YEAR:
			if(m_dateTime.m_year < MAX_YEAR)
				++m_dateTime.m_year;
			else
				m_dateTime.m_year = MIN_YEAR;
			break;
		case EP_DAY_OF_WEEK:
			if(m_dateTime.m_dayOfWeek < MAX_DAY_OF_WEEK)
				++m_dateTime.m_dayOfWeek;
			else
				m_dateTime.m_dayOfWeek = MIN_DAY_OF_WEEK;
			break;
		case EP_HOUR:
			if(m_dateTime.m_hour < MAX_HOUR)
				++m_dateTime.m_hour;
			else
				m_dateTime.m_hour = MIN_HOUR;
			break;
		case EP_MINUTE:
			if(m_dateTime.m_minute < MAX_MINUTE)
				++m_dateTime.m_minute;
			else
				m_dateTime.m_minute = MIN_MINUTE;
			break;
	}
}

void DateTimeEditDisplay::DecreaseActualPart()
{
	switch(m_eEditingPart)
	{
		case EP_DAY:
			if(m_dateTime.m_day > MIN_DAY)
				--m_dateTime.m_day;
			else
				m_dateTime.m_day = MAX_DAY;
			break;
		case EP_MONTH:
			if(m_dateTime.m_month > MIN_MONTH)
				--m_dateTime.m_month;
			else
				m_dateTime.m_month = MAX_MONTH;
			break;
		case EP_YEAR:
			if(m_dateTime.m_year  > MIN_YEAR)
				--m_dateTime.m_year;
			else
				m_dateTime.m_year = MAX_YEAR;
			break;
		case EP_DAY_OF_WEEK:
			if(m_dateTime.m_dayOfWeek  > MIN_DAY_OF_WEEK)
				--m_dateTime.m_dayOfWeek;
			else
				m_dateTime.m_dayOfWeek = MAX_DAY_OF_WEEK;
			break;
		case EP_HOUR:
			if(m_dateTime.m_hour  > MIN_HOUR)
				--m_dateTime.m_hour;
			else
				m_dateTime.m_hour = MAX_HOUR;
			break;
		case EP_MINUTE:
			if(m_dateTime.m_minute  > MIN_MINUTE)
				--m_dateTime.m_minute;
			else
				m_dateTime.m_minute = MAX_MINUTE;
			break;
	}
}