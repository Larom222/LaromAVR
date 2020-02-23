/* 
* DaySelectDisplay.cpp
*
* Created: 2020-02-23 13:56:57
* Author: Gabriel
*/


#include "DaySelectDisplay.h"
#include "../../../_Libraries/LCD_Display/LCD_Display.h"
#include "../KeyboardManager.h"
#include "DayEditDisplay.h"

// default constructor
DaySelectDisplay::DaySelectDisplay()
{
	m_bFinished = true;
	m_eState = S_IDLE;
	m_eEditMode = EM_DAY;
} //DaySelectDisplay

// default destructor
DaySelectDisplay::~DaySelectDisplay()
{
} //~DaySelectDisplay

void DaySelectDisplay::Show(E_EDIT_MODE _eEditMode)
{
	m_bFinished = false;
	m_eState = S_REPAINT_FIRST_LINE;
	m_eMenuPos = M_PN;
	m_eEditMode = _eEditMode;
}

bool DaySelectDisplay::Finished()
{
	return m_bFinished;
}


void DaySelectDisplay::Process()
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
					if(m_eEditMode == EM_DAY)
					{
						m_dayEditDisplay.Show(m_eMenuPos + 1); //day of week enum starts from 1
						m_eState = S_SHOW_SUBMENU;
					}
					else
					{
						m_hourEditDisplay.Show(m_eMenuPos + 1); //day of week enum starts from 1
						m_eState = S_SHOW_SUBMENU;
					}
					break;

				case KeyboardManager::K_RIGHT:
					if(m_eMenuPos < M_SO)
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
			if(DisplayLCD::GetInstance().GetAvailableTextLength() > 23)
			{				
				if(m_eMenuPos == 0)
				{
					DisplayLCD::GetInstance().WriteText(EMPTY_MENU_TEXT, FIRST_LINE_ADDRESS + 1, 2);
				}
				else
				{
					DisplayLCD::GetInstance().WriteText(FIRST_LINE_MENU_TEXT + (m_eMenuPos - 1) * 2, FIRST_LINE_ADDRESS + 1, 2);
				}
				
				DisplayLCD::GetInstance().WriteText(FIRST_LINE_MENU_TEXT + m_eMenuPos * 2, FIRST_LINE_ADDRESS + 7, 2);
				
				if(m_eMenuPos >= 6)
				{
					DisplayLCD::GetInstance().WriteText(EMPTY_MENU_TEXT, FIRST_LINE_ADDRESS + 13, 2);
				}
				else
				{
					DisplayLCD::GetInstance().WriteText(FIRST_LINE_MENU_TEXT + (m_eMenuPos + 1) * 2, FIRST_LINE_ADDRESS + 13, 2);
				}
				DisplayLCD::GetInstance().WriteText(" ", FIRST_LINE_ADDRESS, 1);
				DisplayLCD::GetInstance().WriteText("  > ", FIRST_LINE_ADDRESS + 3, 4);
				DisplayLCD::GetInstance().WriteText(" <  ", FIRST_LINE_ADDRESS + 9, 4);
				DisplayLCD::GetInstance().WriteText(" ", FIRST_LINE_ADDRESS + 15, 1);
				m_eState = S_REPAINT_SECOND_LINE;
			}
			break;
		case S_REPAINT_SECOND_LINE:
		{
			if(DisplayLCD::GetInstance().GetAvailableTextLength() > 17)
			{				
				DisplayLCD::GetInstance().WriteText("     >    <      ", SECOND_LINE_ADDRESS, 16);
				m_eState = S_IDLE;
			}
			break;
		}
		case S_SHOW_SUBMENU:
			if(!m_dayEditDisplay.Finished())
				m_dayEditDisplay.Process();
			else if(!m_hourEditDisplay.Finished())
				m_hourEditDisplay.Process();
			else
				m_eState = S_REPAINT_FIRST_LINE;
			break;
		case S_IDLE:
			break;
	}
}