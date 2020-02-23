/* 
* DayEditDisplay.h
*
* Created: 2020-02-23 16:31:56
* Author: Gabriel
*/


#ifndef __DAYEDITDISPLAY_H__
#define __DAYEDITDISPLAY_H__


class DayEditDisplay
{
	enum E_STATE
	{
		S_IDLE = 0,
		S_INIT_DISPLAY,
		S_REPAINT_LABEL,
		S_REPAINT_TEMP,
		S_SAVE_TEMPERATURE,
	};
	
	enum E_EDITING_PART
	{
		EP_MIN = 0,
		EP_MAX,
	};
	
	enum E_SAVE_STEP
	{
		ST_CLEAR_DISPLAY = 0,
		ST_WRITE_WAIT_MESSAGE,
		ST_WRITE_MIN_TEMP,
		ST_WRITE_MAX_TEMP,	
	};
	
//variables
public:
protected:
private:
	unsigned char m_ucDayOfWeek;
	bool m_bFinished;
	unsigned char m_ucMinTemp;
	unsigned char m_ucMaxTemp;
	unsigned long m_ulLastBlink;
	bool m_bBlink;
	E_STATE m_eState;
	E_EDITING_PART m_eEditingPart;
	E_SAVE_STEP m_eSaveStep;
	unsigned char m_ucSaveHour;
	
//functions
public:
	DayEditDisplay();
	~DayEditDisplay();
	
	void Show(unsigned char _ucDayOfWeek);
	
	bool Finished();
	
	void Process();
	
protected:
private:
	DayEditDisplay( const DayEditDisplay &c );
	DayEditDisplay& operator=( const DayEditDisplay &c );

	void IncreaseActualPart();

	void DecreaseActualPart();

}; //DayEditDisplay

#endif //__DAYEDITDISPLAY_H__
