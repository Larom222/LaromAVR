/*
* RTC_Manager.cpp
*
* Created: 2020-02-09 17:10:41
* Author: Gabriel
*/


#include "RTC_Manager.h"
#include "../../_Libraries/DebugTools/DebugTools.h"

// default constructor
RTCManager::RTCManager()
{
	m_bMutex = false;
	m_bConfigured = false;
	m_bDataReady = false;
	
	m_ucDataPart = 0;
		
	m_eState = S_IDLE;
} //RTCManager

// default destructor
RTCManager::~RTCManager()
{
} //~RTCManager

// Register manager
bool RTCManager::Register()
{
	if(!m_bConfigured)
		return false;
		
	if(m_bMutex)
		return false;
				
	m_bMutex = true;
	return true;
}

// Unregister manager
void RTCManager::Unregister()
{
	m_bMutex = false;
}

void RTCManager::Configure()
{
	TWIManager::GetInstance().Configure();
	m_bConfigured = true;
}

//Write byte
bool RTCManager::WriteDateTime(DateTime _dateTime)
{
	if(!m_bConfigured || !m_bMutex || m_eState != S_IDLE)
		return false;		
	
	if(TWIManager::GetInstance().Register())	
	{
		m_dateTime = _dateTime;
		m_eState = S_WRITE_START;
		m_ucDataPart = 0;
		return true;
	}
	
	return false;
}

//Read byte
bool RTCManager::ReadDateTime()
{
	if(!m_bConfigured || !m_bMutex || m_eState != S_IDLE)
	return false;
		
	if(TWIManager::GetInstance().Register())
	{
		m_eState = S_READ_START;
		m_bDataReady = false;
		m_ucDataPart = 0;
		return true;
	}
	
	return false;
}

//Process
void RTCManager::Process()
{
	if(!m_bConfigured)
		return;
	
	switch(m_eState)
	{
		//send start sequence
		case S_WRITE_START:
			TWIManager::GetInstance().SendStart();
			m_eState = S_WRITE_START_ACK;
			break;
		
		//wait for TWI
		case S_WRITE_START_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x08)
				m_eState = S_WRITE_DEVICE_ADDRESS;
			else 
				m_eState = S_IDLE;
			break;
		}
		
		//send device address sequence
		case S_WRITE_DEVICE_ADDRESS:
			TWIManager::GetInstance().SendData(0xD0); //D0 for write, D1 for read
			m_eState = S_WRITE_DEVICE_ADDRESS_ACK;
			break;
			
		case S_WRITE_DEVICE_ADDRESS_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x18)
				m_eState = S_WRITE_ADDRESS;
			else
				m_eState = S_IDLE;
			break;
		}
		
		//send memory address
		case S_WRITE_ADDRESS:
			TWIManager::GetInstance().SendData(0); //start from address 0 - seconds
			m_eState = S_WRITE_ADDRESS_ACK;
			break;
			
		case S_WRITE_ADDRESS_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x28)
				m_eState = S_WRITE_DATA;
			else
				m_eState = S_IDLE;
			break;
		}
		
		//send data
		case S_WRITE_DATA:
			switch(m_ucDataPart)
			{
				case 0:
					TWIManager::GetInstance().SendData((m_dateTime.m_second % 10) | (m_dateTime.m_second / 10 << 4));
				break;
				case 1:
					TWIManager::GetInstance().SendData((m_dateTime.m_minute % 10) | (m_dateTime.m_minute / 10 << 4));
				break;
				case 2:
					TWIManager::GetInstance().SendData((m_dateTime.m_hour % 10) | (m_dateTime.m_hour / 10 << 4));
				break;
				case 3:
					TWIManager::GetInstance().SendData(m_dateTime.m_dayOfWeek);
				break;
				case 4:
					TWIManager::GetInstance().SendData((m_dateTime.m_day % 10) | (m_dateTime.m_day / 10 << 4));
				break;
				case 5:
					TWIManager::GetInstance().SendData((m_dateTime.m_month % 10) | (m_dateTime.m_month / 10 << 4));
				break;
				case 6:
					TWIManager::GetInstance().SendData((m_dateTime.m_year % 10) | (m_dateTime.m_year / 10 << 4));
				break;
			}
			m_eState = S_WRITE_DATA_ACK;
			break;
			
		case S_WRITE_DATA_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x28)
			{
				if(m_ucDataPart >= 6)
				{
					m_eState = S_STOP;
				}
				else
				{
					++m_ucDataPart;
					m_eState = S_WRITE_DATA;
				}
			}
			else
				m_eState = S_IDLE;
			break;
		}
		
		//send stop
		case S_STOP:
			TWIManager::GetInstance().SendStop();
			m_eState = S_IDLE;
			TWIManager::GetInstance().Unregister();
			break;
			
			
		//send start sequence
		case S_READ_START:
			TWIManager::GetInstance().SendStart();
			m_eState = S_READ_START_ACK;
			break;
			
		//wait for TWI
		case S_READ_START_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x08)
				m_eState = S_READ_DEVICE_ADDRESS;
			else
				m_eState = S_IDLE;
			break;
		}
		
		//send device address sequence
		case S_READ_DEVICE_ADDRESS:
			TWIManager::GetInstance().SendData(0xD0); //D0 for write, D1 for read
			m_eState = S_READ_DEVICE_ADDRESS_ACK;
			break;
			
		case S_READ_DEVICE_ADDRESS_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x18)
				m_eState = S_READ_ADDRESS;
			else
				m_eState = S_IDLE;
			break;
		}
		
		//send memory address
		case S_READ_ADDRESS:
			TWIManager::GetInstance().SendData(0);
			m_eState = S_READ_ADDRESS_ACK;
			break;
		
		case S_READ_ADDRESS_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x28)
				m_eState = S_READ_START_2;
			else
				m_eState = S_IDLE;
			break;
		}
		
		//repeat start sequence
		case S_READ_START_2:
			TWIManager::GetInstance().SendStart();
			m_eState = S_READ_START_2_ACK;
			break;
			
		//wait for TWI
		case S_READ_START_2_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x10)
				m_eState = S_READ_ADDRESS_2;
			else
				m_eState = S_IDLE;
			break;
		}
		
		//send slave address, read bit = 1; MR mode!
		case S_READ_ADDRESS_2:
			TWIManager::GetInstance().SendData(0xD1); //D0 for write, D1 for read
			m_eState = S_READ_ADDRESS_2_ACK;
			break;
		
		case S_READ_ADDRESS_2_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x40)
				m_eState = S_READ_DATA;
			else
				m_eState = S_IDLE;
			break;
		}
		
		//send read command
		case S_READ_DATA:
			if(m_ucDataPart < 6)
				TWIManager::GetInstance().SendData(0, true);
			else
				TWIManager::GetInstance().SendData(0, false);
			m_eState = S_READ_DATA_ACK;
			break;
		
		case S_READ_DATA_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x58 || ucState == 0x50)
			{
				switch(m_ucDataPart)
				{
					case 0:
						m_dateTime.m_second = (TWIManager::GetInstance().ReadData() >> 4) * 10 + (TWIManager::GetInstance().ReadData() & 15);
						break;
					case 1:
						m_dateTime.m_minute = (TWIManager::GetInstance().ReadData() >> 4) * 10 + (TWIManager::GetInstance().ReadData() & 15);
						break;
					case 2:
						m_dateTime.m_hour = (TWIManager::GetInstance().ReadData() >> 4) * 10 + (TWIManager::GetInstance().ReadData() & 15);
						break;
					case 3:
						m_dateTime.m_dayOfWeek = TWIManager::GetInstance().ReadData();
						break;
					case 4:
						m_dateTime.m_day = (TWIManager::GetInstance().ReadData() >> 4) * 10 + (TWIManager::GetInstance().ReadData() & 15);
						break;
					case 5:
						m_dateTime.m_month = (TWIManager::GetInstance().ReadData() >> 4) * 10 + (TWIManager::GetInstance().ReadData() & 15);
						break;
					case 6:
						m_dateTime.m_year = (TWIManager::GetInstance().ReadData() >> 4) * 10 + (TWIManager::GetInstance().ReadData() & 15);
						break;					
				}
				
				if(m_ucDataPart < 6)
				{
					++m_ucDataPart;
					m_eState = S_READ_DATA;
				}
				else 
				{
					m_eState = S_STOP;
					m_bDataReady = true;
				}
			}
			else
				m_eState = S_IDLE;
			break;
		}
		case S_IDLE:
			break;
	}
}

void DateTime::DateToString(char *_strBuf)
{
	_strBuf[0] = m_day / 10 + 48;
	_strBuf[1] = m_day % 10 + 48;
	_strBuf[2] = '.';
	_strBuf[3] = m_month / 10 + 48;
	_strBuf[4] = m_month % 10 + 48;
	_strBuf[5] = '.';
	_strBuf[6] = m_year / 10 + 48;
	_strBuf[7] = m_year % 10 + 48;
}

void DateTime::TimeToString(char *_strBuf)
{
	_strBuf[0] = m_hour / 10 + 48;
	_strBuf[1] = m_hour % 10 + 48;
	_strBuf[2] = ':';
	_strBuf[3] = m_minute / 10 + 48;
	_strBuf[4] = m_minute % 10 + 48;
}

void DateTime::DayOfWeekToString(unsigned char _ucDayOfWeek, char *_strBuf)
{	
	switch(_ucDayOfWeek)
	{
		case 1:
			_strBuf[0] = 'N';
			_strBuf[1] = 'D';
			break;
		case 2:
			_strBuf[0] = 'P';
			_strBuf[1] = 'N';
			break;
		case 3:
			_strBuf[0] = 'W';
			_strBuf[1] = 'T';
			break;
		case 4:
			_strBuf[0] = 'S';
			_strBuf[1] = 'R';
			break;
		case 5:
			_strBuf[0] = 'C';
			_strBuf[1] = 'Z';
			break;
		case 6:
			_strBuf[0] = 'P';
			_strBuf[1] = 'T';
			break;
		case 7:
			_strBuf[0] = 'S';
			_strBuf[1] = 'O';
			break;
		default:
			_strBuf[0] = '-';
			_strBuf[1] = '-';
			break;
	}
}

void DateTime::DayOfWeekToString(char *_strBuf)
{
	DayOfWeekToString(m_dayOfWeek, _strBuf);
}

bool DateTime::operator!=(DateTime _dateTime)
{
	bool ret = false;
	
	if(m_second != _dateTime.m_second)
		ret = true;
	else if(m_minute != _dateTime.m_minute)
		ret = true;
	else if(m_hour != _dateTime.m_hour)
		ret = true;
	else if(m_day != _dateTime.m_day)
		ret = true;
	else if(m_month != _dateTime.m_month)
		ret = true;
	else if(m_year != _dateTime.m_year)
		ret = true;
	else if(m_dayOfWeek != _dateTime.m_dayOfWeek)
		ret = true;
		
	return ret;
}