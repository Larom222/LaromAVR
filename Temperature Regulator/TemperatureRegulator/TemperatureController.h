/* 
* TemperatureController.h
*
* Created: 2020-02-21 18:19:26
* Author: Gabriel
*/


#ifndef __TEMPERATURECONTROLLER_H__
#define __TEMPERATURECONTROLLER_H__

#include "../../_Libraries/RTC_Manager/RTC_Manager.h"
#include "../../_Libraries/EEPROM_Manager/EEPROM_Manager.h"
#include "Config.h"

class TemperatureController
{	
public:
	enum E_MODE
	{
		M_MANUAL = 0,
		M_AUTO = 1,
	};

private:
	enum E_STATE
	{
		S_IDLE = 0,
		S_READ_MODE,
		S_READ_MODE_RES,
		S_READ_MIN_TEMP,
		S_READ_MIN_TEMP_RES,
		S_READ_MAX_TEMP,
		S_READ_MAX_TEMP_RES,
		S_READ_DATE_TIME,
		S_READ_DATE_TIME_RES,
		S_READ_TEMP,
		S_READ_TEMP_RES,
	};
	
	
//variables
public:
protected:
private:

	unsigned short m_usActualTemp;
	unsigned char m_ucMinTemp;
	unsigned char m_ucMaxTemp;
	DateTime m_actualDate;
	E_MODE m_eMode; //manual or auto
	bool m_bHeating;
	E_STATE m_eState;
	unsigned long m_ulLastTimeRefresh;
	unsigned long m_ulLastTempRefresh;
	bool m_bInitialized; //if false, the whole loop will be processed
	
//functions
public:
	static TemperatureController& GetInstance()
	{
		// instance
		static TemperatureController instance;
	
		return instance;
	}
	
	void SetMinTemp(unsigned char _ucMinTemp) {m_ucMinTemp = _ucMinTemp;};
	void SetMaxTemp(unsigned char _ucMaxTemp) {m_ucMaxTemp = _ucMaxTemp;};
	void SetMode(E_MODE _eMode);

	DateTime GetDateTime() {return m_actualDate;}
	E_MODE GetMode() {return m_eMode;}
	unsigned char GetMinTemp() {return m_ucMinTemp;}
	unsigned char GetMaxTemp() {return m_ucMaxTemp;}
	unsigned short GetActualTemp() {return m_usActualTemp;}
	bool GetHeating() {return m_bHeating;}
	void RefreshMinMax() {m_eState = S_READ_MIN_TEMP;}
	void RefreshDateTime() {m_eState = S_READ_DATE_TIME;}
	
	void Process();

protected:
private:

	TemperatureController();
	~TemperatureController();
	TemperatureController( const TemperatureController &c );
	TemperatureController& operator=( const TemperatureController &c );

}; //TemperatureController

#endif //__TEMPERATURECONTROLLER_H__
