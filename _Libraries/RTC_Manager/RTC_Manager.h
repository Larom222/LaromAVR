/* 
* RTC_Manager.h
*
* Created: 2020-02-09 17:10:41
* Author: Gabriel
*/


#ifndef __RTC_MANAGER_H__
#define __RTC_MANAGER_H__
	
#include <avr/io.h>
#include "../../_Libraries/TWI_Manager/TWI_Manager.h"
#include "../../_Libraries/Time/Time.h"

#define MIN_DAY 1
#define MAX_DAY 31
#define MIN_MONTH 1
#define MAX_MONTH 12
#define MIN_YEAR 0
#define MAX_YEAR 99
#define MIN_HOUR 0
#define MAX_HOUR 23
#define MIN_MINUTE 0
#define MAX_MINUTE 59
#define MIN_DAY_OF_WEEK 1
#define MAX_DAY_OF_WEEK 7

struct DateTime
{
	unsigned char m_year;
	unsigned char m_month;
	unsigned char m_day;
	unsigned char m_hour;
	unsigned char m_minute;
	unsigned char m_second;
	unsigned char m_dayOfWeek;
	
	DateTime(){
		m_year = 0;
		m_month = 0;
		m_day = 0;
		m_hour = 0;
		m_minute = 0;
		m_second = 0;
		m_dayOfWeek = 0;
	};
	
	bool operator!=(DateTime _dateTime);
	
	void DateToString(char *_strBuf);
	void TimeToString(char *_strBuf);
	void DayOfWeekToString(char *_strBuf);
	static void DayOfWeekToString(unsigned char _ucDayOfWeek, char *_strBuf);
};

class RTCManager
{
	enum E_STATE
	{
		S_IDLE = 0,
		
		S_WRITE_START,
		S_WRITE_START_ACK,
		S_WRITE_DEVICE_ADDRESS,
		S_WRITE_DEVICE_ADDRESS_ACK,
		S_WRITE_ADDRESS,
		S_WRITE_ADDRESS_ACK,
		S_WRITE_DATA,
		S_WRITE_DATA_ACK,
		
		S_READ_START,
		S_READ_START_ACK,
		S_READ_DEVICE_ADDRESS,
		S_READ_DEVICE_ADDRESS_ACK,
		S_READ_ADDRESS,
		S_READ_ADDRESS_ACK,
		S_READ_START_2,
		S_READ_START_2_ACK,
		S_READ_ADDRESS_2,
		S_READ_ADDRESS_2_ACK,
		S_READ_DATA,
		S_READ_DATA_ACK,
		
		S_STOP,
		S_WRITE_STOP,
	};
//variables
public:
protected:
private:


	bool m_bMutex;
	bool m_bConfigured;
	bool m_bDataReady;
	
	DateTime m_dateTime;
	
	unsigned char m_ucDataPart;
		
	E_STATE m_eState;
		
//functions
public:	
	static RTCManager& GetInstance()
	{
		// instance
		static RTCManager instance;
		
		return instance;
	}
	
	void Configure();
	
	// Register manager
	bool Register();
	
	// Unregister manager
	void Unregister();
	
	//Write byte
	bool WriteDateTime(DateTime _dateTime);
	
	//Read byte
	bool ReadDateTime();
	
	//Check if data successfully received and is ready to read
	bool ReadyToRead() {return m_bDataReady;};
		
	//Return last read byte
	DateTime GetLastDateTime() {return m_dateTime;};
		
	//Process
	void Process();
	
protected:
private:
	RTCManager();
	~RTCManager();
	RTCManager( const RTCManager &c );
	RTCManager& operator=( const RTCManager &c );
	
}; //RTCManager


#endif //__RTC_MANAGER_H__
