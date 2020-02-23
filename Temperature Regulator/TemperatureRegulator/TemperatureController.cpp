/* 
* TemperatureController.cpp
*
* Created: 2020-02-21 18:19:26
* Author: Gabriel
*/


#include "TemperatureController.h"
#include "Thermistor.h"

// default constructor
TemperatureController::TemperatureController()
{
	Thermistor::GetInstance().Configure();
	RTCManager::GetInstance().Configure();
		
	m_eState = S_READ_MODE;
	m_bHeating = false;
	m_ulLastTimeRefresh = 0;
	m_ulLastTempRefresh = 0;
	m_bInitialized = false;
} //TemperatureController

// default destructor
TemperatureController::~TemperatureController()
{
} //~TemperatureController

void TemperatureController::Process()
{
	switch(m_eState)
	{
		case S_READ_MODE:
			if(EEPROMManager::GetInstance().Register())
			{
				if(EEPROMManager::GetInstance().ReadByte(EEPROM_MODE_PAGE, EEPROM_MODE_ADDRESS))
					m_eState = S_READ_MODE_RES;
				else
					EEPROMManager::GetInstance().Unregister();
			}		
			break;

		case S_READ_MODE_RES:
			if(EEPROMManager::GetInstance().ReadyToRead())
			{
				if(EEPROMManager::GetInstance().GetLastRead())
					m_eMode = M_AUTO;
				else
					m_eMode = M_MANUAL;
				EEPROMManager::GetInstance().Unregister();
				m_eState = S_READ_DATE_TIME;
			}
			break;
		case S_READ_DATE_TIME:
			if(RTCManager::GetInstance().Register())
			{
				if(RTCManager::GetInstance().ReadDateTime())
					m_eState = S_READ_DATE_TIME_RES;
				else
					EEPROMManager::GetInstance().Unregister();
			}			
			break;
		case S_READ_DATE_TIME_RES:
			if(RTCManager::GetInstance().ReadyToRead())
			{
				m_actualDate = RTCManager::GetInstance().GetLastDateTime();
				RTCManager::GetInstance().Unregister();
				m_eState = S_READ_TEMP;
			}
			break;
			
		case S_READ_TEMP:
			if(Thermistor::GetInstance().ReadTemperature())
				m_eState = S_READ_TEMP_RES;
			break;
		case S_READ_TEMP_RES:
			if(Thermistor::GetInstance().ReadyToRead())
			{
				m_usActualTemp = Thermistor::GetInstance().GetLastTemperature();
				if(m_bInitialized)
					m_eState = S_IDLE;
				else
					m_eState = S_READ_MIN_TEMP;
			}
			break;
		case S_READ_MIN_TEMP:
		{
			if(EEPROMManager::GetInstance().Register())
			{
				unsigned char ucPage, ucAddress;
				if(m_eMode == M_AUTO)
				{
					ucPage = EEPROM_AUTO_MODE_MIN_PAGE;
					ucAddress = 24 * m_actualDate.m_dayOfWeek + m_actualDate.m_hour;
				}
				else
				{
					ucPage = EEPROM_MANUAL_MODE_MIN_PAGE;
					ucAddress = EEPROM_MANUAL_MODE_MIN_ADDRESS;
				}
				if(EEPROMManager::GetInstance().ReadByte(ucPage, ucAddress))
					m_eState = S_READ_MIN_TEMP_RES;
				else
					EEPROMManager::GetInstance().Unregister();
			}
			break;
		}
		case S_READ_MIN_TEMP_RES:
			if(EEPROMManager::GetInstance().ReadyToRead())
			{
				m_ucMinTemp = EEPROMManager::GetInstance().GetLastRead();
				EEPROMManager::GetInstance().Unregister();
				m_eState = S_READ_MAX_TEMP;
			}
			break;
		case S_READ_MAX_TEMP:
		{
			if(EEPROMManager::GetInstance().Register())
			{
				unsigned char ucPage, ucAddress;
				if(m_eMode == M_AUTO)
				{
					ucPage = EEPROM_AUTO_MODE_MAX_PAGE;
					ucAddress = 24 * m_actualDate.m_dayOfWeek + m_actualDate.m_hour;
				}
				else
				{
					ucPage = EEPROM_MANUAL_MODE_MAX_PAGE;
					ucAddress = EEPROM_MANUAL_MODE_MAX_ADDRESS;
				}
				if(EEPROMManager::GetInstance().ReadByte(ucPage, ucAddress))
					m_eState = S_READ_MAX_TEMP_RES;
				else
					EEPROMManager::GetInstance().Unregister();
			}
			break;
		}
		case S_READ_MAX_TEMP_RES:
			if(EEPROMManager::GetInstance().ReadyToRead())
			{
				m_ucMaxTemp = EEPROMManager::GetInstance().GetLastRead();
				EEPROMManager::GetInstance().Unregister();
				m_eState = S_IDLE;
			}
			break;
		case S_IDLE:
			unsigned long ulCurrentTime = Time::GetInstance().GetUS();
			if(ulCurrentTime - m_ulLastTimeRefresh > TIME_REFRESH_DELAY)
			{
				m_eState = S_READ_DATE_TIME; //after initialization the state will back to IDLE after time and temperature read
				m_ulLastTimeRefresh = m_ulLastTempRefresh = ulCurrentTime;
			}
			else if(ulCurrentTime - m_ulLastTempRefresh > TEMP_REFRESH_DELAY)
			{
				m_eState = S_READ_TEMP; //after initialization the state will back to IDLE after temperature read
				m_ulLastTempRefresh = ulCurrentTime;
			}
			break;
	}

	if(m_bHeating && m_usActualTemp / 100 >= m_ucMaxTemp)
	{
		m_bHeating = false;
		//TODO disable relay
	}
	else if(m_usActualTemp / 100 < m_ucMinTemp)
	{
		m_bHeating = true;
		//TODO enable relay
	}
}

void TemperatureController::SetMode(E_MODE _eMode)
{
	m_eMode = _eMode;
	m_eState = S_READ_MIN_TEMP;
}