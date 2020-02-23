/* 
* MainMenu.h
*
* Created: 2020-02-23 11:58:38
* Author: Gabriel
*/


#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include "DaySelectDisplay.h"
#include "DateTimeEditDisplay.h"

static char FIRST_LINE_MAIN_MENU_TEXT[4 * 3 + 1] = "DATACALATRYB";
static char SECOND_LINE_MAIN_MENU_TEXT[4 * 3 + 1] = "CZASDOBAGODZ";
static char EMPTY_MAIN_MENU_TEXT[4 + 1] = "    ";

class MainMenu
{	
	enum E_STATE
	{
		S_IDLE = 0,
		S_REPAINT_FIRST_LINE,
		S_REPAINT_SECOND_LINE,
		S_SHOW_SUBMENU,
	};
	
	enum E_MENU_POS
	{
		M_DATE_TIME_EDIT = 0,
		M_DAY_EDIT,
		M_HOUR_EDIT,
	};
	
//variables
public:
protected:
private:

	bool m_bFinished;
	unsigned char m_eMenuPos;

	E_STATE m_eState;
	
	DaySelectDisplay m_daySelectDisplay;
	DateTimeEditDisplay m_dateTimeEditDisplay;

//functions
public:
	MainMenu();
	~MainMenu();
	
	void Show();
	
	bool Finished();
	
	void Process();
protected:
private:
		
	MainMenu( const MainMenu &c );
	MainMenu& operator=( const MainMenu &c );

}; //MainMenu

#endif //__MAINMENU_H__
