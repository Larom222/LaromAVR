/* 
* MainDisplay.cpp
*
* Created: 2020-02-22 16:34:17
* Author: Gabriel
*/


#include "MainDisplay.h"
#include "../KeyboardManager.h"
#include "../../../_Libraries/Time/Time.h"
#include "../Config.h"

// default constructor
MainDisplay::MainDisplay()
{
	m_usActualTemp = 0;
	m_ucMinTemp = 0;
	m_ucMaxTemp = 0;
	m_ucTempToSet = 0;
	m_eMode = 0;
	m_bHeating = false;
	m_eState = S_REPAINT;
	m_bMinEdit = false;
	m_bMaxEdit = false;
	m_ulLastBlink = 0;
	m_bBlink = false;
	m_eWriteState = WS_IDLE;
} //MainDisplay

// default destructor
MainDisplay::~MainDisplay()
{
} //~MainDisplay

void MainDisplay::Process()
{
	//Keyboard process
	KeyboardManager::E_KEY_CODE eKeyToProcess = KeyboardManager::GetInstance().GetKeyToProcess();
	if(eKeyToProcess != KeyboardManager::K_LAST)
	{
		switch(eKeyToProcess)
		{
			case KeyboardManager::K_LEFT:
				if(m_bMinEdit)
				{
					if(m_ucTempToSet > MIN_TEMP)
						--m_ucTempToSet;
				}
				else if(m_bMaxEdit)
				{
					if(m_ucTempToSet > m_ucMinTemp + 1)
						--m_ucTempToSet;
				}
				else if(m_eMode == TemperatureController::M_MANUAL)
				{
					m_bMinEdit = true;
					m_ucTempToSet = m_ucMinTemp;
					m_bBlink = true;					
					m_ulLastBlink = Time::GetInstance().GetUS();
				}
				m_eState = S_REPAINT;
				break;

			case KeyboardManager::K_ESC:
				if(m_bMinEdit)
				{
					m_bMinEdit = false;
					m_eState = S_REPAINT;
				}
				else if(m_bMaxEdit)
				{
					m_bMaxEdit = false;
					m_eState = S_REPAINT;
				}
				else
				{
					m_eWriteState = WS_WRITE_MODE_REGISTER;
				}
				break;

			case KeyboardManager::K_OK:
				if(m_bMinEdit)
				{					
					m_eWriteState = WS_WRITE_MIN_REGISTER;
					m_bMinEdit = false;					
				}
				else if(m_bMaxEdit)
				{
					m_eWriteState = WS_WRITE_MAX_REGISTER;
					m_bMaxEdit = false;
				}
				else
				{
					//TODO enter menu
					//m_eState = S_IN_MENU;
				}
				break;

			case KeyboardManager::K_RIGHT:
				if(m_bMinEdit)
				{
					if(m_ucTempToSet < m_ucMaxTemp - 1)
						++m_ucTempToSet;
				}
				else if(m_bMaxEdit)
				{
					if(m_ucTempToSet < MAX_TEMP)
						++m_ucTempToSet;
				}
				else if(m_eMode == TemperatureController::M_MANUAL)
				{
					m_bMaxEdit = true;
					m_ucTempToSet = m_ucMaxTemp;
					m_bBlink = true;
					m_ulLastBlink = Time::GetInstance().GetUS();
				}
				m_eState = S_REPAINT;
				break;
			default:
				break;
		}
	}

	//GUI loop
	switch(m_eState)
	{
		case S_REPAINT:
			m_usActualTemp = TemperatureController::GetInstance().GetActualTemp();
			m_ucMinTemp = TemperatureController::GetInstance().GetMinTemp();
			m_ucMaxTemp = TemperatureController::GetInstance().GetMaxTemp();
			m_actualDate = TemperatureController::GetInstance().GetDateTime();
			m_eMode = TemperatureController::GetInstance().GetMode();
			m_bHeating = TemperatureController::GetInstance().GetHeating();
			m_eState = S_REPAINT_FIRST_LINE;
			break;
		case S_REPAINT_FIRST_LINE:		
			if(DisplayLCD::GetInstance().GetAvailableTextLength() > 17)
			{
				m_actualDate.DateToString(m_lineBuf);
				m_actualDate.TimeToString(m_lineBuf + 11);
				
				//blink time separator
				if(m_actualDate.m_second % 2)
					m_lineBuf[13] = ' ';

				//spaces
				m_lineBuf[8] = m_lineBuf[10] = ' ';

				//mode
				if(m_eMode == TemperatureController::M_AUTO)
					m_lineBuf[9] = 'A';
				else
					m_lineBuf[9] = 'M';
				
				DisplayLCD::GetInstance().WriteText(m_lineBuf, FIRST_LINE_ADDRESS, 16);
				
				m_eState = S_REPAINT_SECOND_LINE;
			}
			break;	
		case S_REPAINT_SECOND_LINE:
			if(DisplayLCD::GetInstance().GetAvailableTextLength() > 17)
			{
				m_actualDate.DayOfWeekToString(m_lineBuf);
				
				//spaces
				m_lineBuf[2] = m_lineBuf[4] = m_lineBuf[5] = ' ';
				
				//heating
				if(m_bHeating)
					m_lineBuf[3] = 'G';
				else
					m_lineBuf[3] = ' ';
				
				//min, current and max temp
				if(m_bMinEdit)
				{
					if(m_bBlink)
					{
						m_lineBuf[6] = m_lineBuf[7] = ' ';
					}
					else
					{
						m_lineBuf[6] = m_ucTempToSet / 10 + 48;
						m_lineBuf[7] = m_ucTempToSet % 10 + 48;
					}
				}
				else
				{					
					m_lineBuf[6] = m_ucMinTemp / 10 + 48;
					m_lineBuf[7] = m_ucMinTemp % 10 + 48;
				}
				m_lineBuf[8] = m_lineBuf[13] = '<';
				m_lineBuf[9] = m_usActualTemp / 1000 + 48;
				m_lineBuf[10] = (m_usActualTemp / 100) % 10 + 48;
				m_lineBuf[11] = '.';
				m_lineBuf[12] = (m_usActualTemp / 10) % 10 + 48;
				if(m_bMaxEdit)
				{
					if(m_bBlink)
					{
						m_lineBuf[14] = m_lineBuf[15] = ' ';
					}
					else
					{
						m_lineBuf[14] = m_ucTempToSet / 10 + 48;
						m_lineBuf[15] = m_ucTempToSet % 10 + 48;
					}
				}
				else
				{
					m_lineBuf[14] = m_ucMaxTemp / 10 + 48;
					m_lineBuf[15] = m_ucMaxTemp % 10 + 48;
				}
				
				DisplayLCD::GetInstance().WriteText(m_lineBuf, SECOND_LINE_ADDRESS, 16);
				
				m_eState = S_IDLE;
			}
			break;
		case S_IDLE:
				//repaint any change
				if(m_usActualTemp != TemperatureController::GetInstance().GetActualTemp()
					|| m_ucMinTemp != TemperatureController::GetInstance().GetMinTemp()
					|| m_ucMaxTemp != TemperatureController::GetInstance().GetMaxTemp()
					|| m_actualDate != TemperatureController::GetInstance().GetDateTime()
					|| m_eMode != TemperatureController::GetInstance().GetMode()
					|| m_bHeating != TemperatureController::GetInstance().GetHeating())
					{						
						m_eState = S_REPAINT;
						break;
					}
					
				//repaint blink
				unsigned long ulCurrentTime = Time::GetInstance().GetUS();
				if(ulCurrentTime - m_ulLastBlink > BLINK_TIME)
				{
					m_bBlink = !m_bBlink;
					m_ulLastBlink = ulCurrentTime;
					m_eState = S_REPAINT;
					break;
				}
			break;
	}
	
	//EEPROM write loop
	switch(m_eWriteState)
	{
		case WS_WRITE_MIN_REGISTER:
			if(EEPROMManager::GetInstance().Register())
			{
				m_eWriteState = WS_WRITE_MIN;
			}
			break;
		case WS_WRITE_MIN:
			if(EEPROMManager::GetInstance().WriteByte(EEPROM_MANUAL_MODE_MIN_PAGE, EEPROM_MANUAL_MODE_MIN_ADDRESS, m_ucTempToSet))
			{
				m_eWriteState = WS_IDLE;
				EEPROMManager::GetInstance().Unregister();				
				TemperatureController::GetInstance().SetMinTemp(m_ucTempToSet);
			}
			break;
		case WS_WRITE_MAX_REGISTER:
			if(EEPROMManager::GetInstance().Register())
			{
				m_eWriteState = WS_WRITE_MAX;
			}
			break;
		case WS_WRITE_MAX:
			if(EEPROMManager::GetInstance().WriteByte(EEPROM_MANUAL_MODE_MAX_PAGE, EEPROM_MANUAL_MODE_MAX_ADDRESS, m_ucTempToSet))
			{
				m_eWriteState = WS_IDLE;
				EEPROMManager::GetInstance().Unregister();
				TemperatureController::GetInstance().SetMaxTemp(m_ucTempToSet);
			}
			break;
		case WS_WRITE_MODE_REGISTER:
			if(EEPROMManager::GetInstance().Register())
			{
				m_eWriteState = WS_WRITE_MODE;
			}
			break;
		case WS_WRITE_MODE:
			if(EEPROMManager::GetInstance().WriteByte(EEPROM_MODE_PAGE, EEPROM_MODE_ADDRESS, !m_eMode))
			{
				m_eWriteState = WS_IDLE;
				EEPROMManager::GetInstance().Unregister();
				TemperatureController::GetInstance().SetMode((TemperatureController::E_MODE)!m_eMode);
			}
			break;
		case WS_IDLE:
			break;
	}
}