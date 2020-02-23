/* 
* DaySelectDisplay.h
*
* Created: 2020-02-23 13:56:57
* Author: Gabriel
*/


#ifndef __DAYSELECTDISPLAY_H__
#define __DAYSELECTDISPLAY_H__

#include "DayEditDisplay.h"
#include "HourEditDisplay.h"

static char FIRST_LINE_MENU_TEXT[2 * 7 + 1] = "NDPNWTSRCZPTSO";
static char EMPTY_MENU_TEXT[2 + 1] = "  ";

class DaySelectDisplay
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
		M_ND = 0,
		M_PN,
		M_WT,
		M_SR,
		M_CZ,
		M_PT,
		M_SO,
	};

public:	
	enum E_EDIT_MODE
	{
		EM_DAY = 0,
		EM_HOUR,	
	};
	
//variables
public:
protected:
private:
	bool m_bFinished;
	unsigned char m_eMenuPos;

	E_STATE m_eState;
	E_EDIT_MODE m_eEditMode;
	
	DayEditDisplay m_dayEditDisplay;
	HourEditDisplay m_hourEditDisplay;

//functions
public:
	DaySelectDisplay();
	~DaySelectDisplay();
	
	void Show(E_EDIT_MODE _eEditMode);
	
	bool Finished();
	
	void Process();
	
protected:
private:
	DaySelectDisplay( const DaySelectDisplay &c );
	DaySelectDisplay& operator=( const DaySelectDisplay &c );

}; //DaySelectDisplay

#endif //__DAYSELECTDISPLAY_H__
