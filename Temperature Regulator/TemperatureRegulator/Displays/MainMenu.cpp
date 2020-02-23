/* 
* MainMenu.cpp
*
* Created: 2020-02-23 11:58:38
* Author: Gabriel
*/


#include "MainMenu.h"
#include "../KeyboardManager.h"
#include "../../../_Libraries/LCD_Display/LCD_Display.h"

// default constructor
MainMenu::MainMenu()
{
	m_bFinished = true;
	m_eState = S_IDLE;
} //MainMenu

// default destructor
MainMenu::~MainMenu()
{
} //~MainMenu


void MainMenu::Show()
{
	m_bFinished = false;
	m_eState = S_REPAINT_FIRST_LINE;
	m_eMenuPos = M_DAY_EDIT;
}

bool MainMenu::Finished()
{
	return m_bFinished;
}

void MainMenu::Process()
{
	//Keyboard process
	if(m_eState != S_SHOW_SUBMENU)
	{
		KeyboardManager::E_KEY_CODE eKeyToProcess = KeyboardManager::GetInstance().GetKeyToProcess();
		if(eKeyToProcess != KeyboardManager::K_LAST)
		{
			switch(eKeyToProcess)
			{
				case KeyboardManager::K_LEFT:
					if(m_eMenuPos)
					{
						--m_eMenuPos;
						m_eState = S_REPAINT_FIRST_LINE;
					}
					break;

				case KeyboardManager::K_ESC:
					m_bFinished = true;
					break;

				case KeyboardManager::K_OK:
					m_eState = S_SHOW_SUBMENU;
					switch(m_eMenuPos)
					{
						case M_DATE_TIME_EDIT:
							m_dateTimeEditDisplay.Show();
							break;
						case M_DAY_EDIT:
							m_daySelectDisplay.Show(DaySelectDisplay::EM_DAY);
							break;
						case M_HOUR_EDIT:
							m_daySelectDisplay.Show(DaySelectDisplay::EM_HOUR);
							break;
					}
					break;

				case KeyboardManager::K_RIGHT:
					if(m_eMenuPos < 2)
					{
						++m_eMenuPos;
						m_eState = S_REPAINT_FIRST_LINE;
					}
					break;
				default:
					break;
			}
		}
	}

	//GUI loop
	switch(m_eState)
	{
		case S_REPAINT_FIRST_LINE:
		case S_REPAINT_SECOND_LINE:
		{			
			if(DisplayLCD::GetInstance().GetAvailableTextLength() > 21)
			{
				unsigned char ucAddress;
				char *strText;
				
				if(m_eState == S_REPAINT_FIRST_LINE)
				{
					ucAddress = FIRST_LINE_ADDRESS;
					strText = FIRST_LINE_MAIN_MENU_TEXT;
					m_eState = S_REPAINT_SECOND_LINE;
				}
				else
				{
					ucAddress = SECOND_LINE_ADDRESS;
					strText = SECOND_LINE_MAIN_MENU_TEXT;
					m_eState = S_IDLE;
				}
				
				if(m_eMenuPos == 0)
				{
					DisplayLCD::GetInstance().WriteText(EMPTY_MAIN_MENU_TEXT, ucAddress, 4);
				}
				else
				{
					DisplayLCD::GetInstance().WriteText(strText + (m_eMenuPos - 1) * 4, ucAddress, 4);
				}
				
				DisplayLCD::GetInstance().WriteText(strText + m_eMenuPos * 4, ucAddress + 6, 4);
				
				if(m_eMenuPos >= 2)
				{
					DisplayLCD::GetInstance().WriteText(EMPTY_MAIN_MENU_TEXT, ucAddress + 12, 4);
				}
				else
				{
					DisplayLCD::GetInstance().WriteText(strText + (m_eMenuPos + 1) * 4, ucAddress + 12, 4);
				}
				DisplayLCD::GetInstance().WriteText(" >", ucAddress + 4, 2);
				DisplayLCD::GetInstance().WriteText("< ", ucAddress + 10, 2);
			}
			break;
		}
		case S_SHOW_SUBMENU:
			if(!m_daySelectDisplay.Finished())
				m_daySelectDisplay.Process();
			else if(!m_dateTimeEditDisplay.Finished())
				m_dateTimeEditDisplay.Process();
			else
				m_eState = S_REPAINT_FIRST_LINE;
		case S_IDLE:
			break;
	}
}