/* 
* DateTimeEditDisplay.h
*
* Created: 2020-02-23 13:56:32
* Author: Gabriel
*/


#ifndef __DATETIMEEDITDISPLAY_H__
#define __DATETIMEEDITDISPLAY_H__

#include "../../../_Libraries/Time/Time.h"
#include "../../../_Libraries/RTC_Manager/RTC_Manager.h"
#include "../Config.h"

class DateTimeEditDisplay
{
	enum E_STATE
	{
		S_IDLE = 0,
		S_INIT_DISPLAY,
		S_REPAINT,
		S_SAVE_DATE_TIME,
	};
	
	enum E_EDITING_PART
	{
		EP_DAY = 0,
		EP_MONTH,
		EP_YEAR,
		EP_DAY_OF_WEEK,
		EP_HOUR,
		EP_MINUTE,
	};
	
//variables
public:
protected:
private:
	bool m_bFinished;
	unsigned char m_eEditingPart;
	unsigned long m_ulLastBlink;
	bool m_bBlink;
	DateTime m_dateTime;

	E_STATE m_eState;

//functions
public:
	DateTimeEditDisplay();
	~DateTimeEditDisplay();
	
	void Show();
	
	bool Finished();
	
	void Process();
protected:
private:
	DateTimeEditDisplay( const DateTimeEditDisplay &c );
	DateTimeEditDisplay& operator=( const DateTimeEditDisplay &c );
	
	
	void IncreaseActualPart();

	void DecreaseActualPart();

}; //DateTimeEditDisplay

#endif //__DATETIMEEDITDISPLAY_H__
