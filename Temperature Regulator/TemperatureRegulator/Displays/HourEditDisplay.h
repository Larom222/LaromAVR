/* 
* HourEditDisplay.h
*
* Created: 2020-02-23 17:49:46
* Author: Gabriel
*/


#ifndef __HOUREDITDISPLAY_H__
#define __HOUREDITDISPLAY_H__


class HourEditDisplay
{
	enum E_STATE
	{
		S_IDLE = 0,
		S_INIT_DISPLAY,
		S_READ_MIN_TEMP,
		S_READ_MIN_TEMP_RES,
		S_READ_MAX_TEMP,
		S_READ_MAX_TEMP_RES,
		S_REPAINT,
		S_SAVE_MIN_TEMP,
		S_SAVE_MAX_TEMP,
	};
	
	enum E_EDITING_PART
	{
		EP_MIN = 0,
		EP_MAX,
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
	unsigned char m_ucSelectedHour;
	bool m_bEditMode;

//functions
public:
	HourEditDisplay();
	~HourEditDisplay();
	
	void Show(unsigned char _ucDayOfWeek);
	
	bool Finished();
	
	void Process();
protected:
private:
	HourEditDisplay( const HourEditDisplay &c );
	HourEditDisplay& operator=( const HourEditDisplay &c );

	void IncreaseActualPart();

	void DecreaseActualPart();

}; //HourEditDisplay

#endif //__HOUREDITDISPLAY_H__
