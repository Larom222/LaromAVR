/*
* EEPROM_Manager.cpp
*
* Created: 2020-02-09 17:10:41
* Author: Gabriel
*/


#include "EEPROM_Manager.h"
#include "../../_Libraries/DebugTools/DebugTools.h"

// default constructor
EEPROMManager::EEPROMManager()
{
	m_bMutex = false;
	m_bConfigured = false;
	m_bDataReady = false;
	
	m_ucPage = 0;
	m_ucAddress = 0;
	m_ucData = 0;
	
	m_ulLastStopTime = 0;
	
	m_eState = S_IDLE;
} //EEPROMManager

// default destructor
EEPROMManager::~EEPROMManager()
{
} //~EEPROMManager

// Register manager
bool EEPROMManager::Register()
{
	if(!m_bConfigured)
		return false;
		
	if(m_bMutex)
		return false;
		
	if(Time::GetInstance().GetUS() - m_ulLastStopTime < STOP_DELAY)
		return false;
		
	m_bMutex = true;
	return true;
}

// Unregister manager
void EEPROMManager::Unregister()
{
	m_bMutex = false;
}

void EEPROMManager::Configure()
{
	TWIManager::GetInstance().Configure();
	m_bConfigured = true;
}

//Write byte
bool EEPROMManager::WriteByte(unsigned char _ucPage, unsigned char _ucAddress, unsigned char _ucData)
{
	if(!m_bConfigured || !m_bMutex || m_eState != S_IDLE)
		return false;
		
	if(Time::GetInstance().GetUS() - m_ulLastStopTime < STOP_DELAY)
		return false;
	
	if(TWIManager::GetInstance().Register())	
	{
		m_ucPage = (_ucPage << 1);
		m_ucAddress = _ucAddress;
		m_ucData = _ucData;
		m_eState = S_WRITE_START;
		return true;
	}
	
	return false;
}

//Read byte
bool EEPROMManager::ReadByte(unsigned char _ucPage, unsigned char _ucAddress)
{
	if(!m_bConfigured || !m_bMutex || m_eState != S_IDLE)
	return false;
	
	if(Time::GetInstance().GetUS() - m_ulLastStopTime < STOP_DELAY)
		return false;	
		
	if(TWIManager::GetInstance().Register())
	{		
		m_ucPage = (_ucPage << 1);
		m_ucAddress = _ucAddress;
		m_ucData = 0;
		m_eState = S_READ_START;
		m_bDataReady = false;
		return true;
	}
	
	return false;
}

//Process
void EEPROMManager::Process()
{
	if(!m_bConfigured)
		return;
	
	switch(m_eState)
	{
		//send start sequence
		case S_WRITE_START:
			if(TWIManager::GetInstance().SendStart())
				m_eState = S_WRITE_START_ACK;
			break;
		
		//wait for TWI
		case S_WRITE_START_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x08)
				m_eState = S_WRITE_PAGE_NO;
			else 
			{
				if(!DebugTools::GetInstance().m_bError)
				{
					DebugTools::GetInstance().m_bError = true;
					DebugTools::GetInstance().m_iDebugCounter = 100 + ucState;
				}
				m_eState = S_IDLE;
			}
			break;
		}
		
		//send device address sequence
		case S_WRITE_PAGE_NO:		
			if(TWIManager::GetInstance().SendData(0xA0 + m_ucPage))
				m_eState = S_WRITE_PAGE_NO_ACK;
			break;
			
		case S_WRITE_PAGE_NO_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x18)
				m_eState = S_WRITE_ADDRESS;
			else
			{
				if(!DebugTools::GetInstance().m_bError)
				{
					DebugTools::GetInstance().m_bError = true;
					DebugTools::GetInstance().m_iDebugCounter = 200 + ucState;					
				}
				m_eState = S_IDLE;
			}
			break;
		}
		
		//send memory address
		case S_WRITE_ADDRESS:
			if(TWIManager::GetInstance().SendData(m_ucAddress))
			{
				m_eState = S_WRITE_ADDRESS_ACK;				
			}
			break;
			
		case S_WRITE_ADDRESS_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x28)
				m_eState = S_WRITE_DATA;
			else
			{
				if(!DebugTools::GetInstance().m_bError)
				{
					DebugTools::GetInstance().m_bError = true;
					DebugTools::GetInstance().m_iDebugCounter = 300 + ucState;
				}
				m_eState = S_IDLE;
			}
			break;
		}
		
		//send data
		case S_WRITE_DATA:
			if(TWIManager::GetInstance().SendData(m_ucData))
			{
				m_eState = S_WRITE_DATA_ACK;				
			}
			break;
			
		case S_WRITE_DATA_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x28)
				m_eState = S_STOP;
			else
			{
				if(!DebugTools::GetInstance().m_bError)
				{
					DebugTools::GetInstance().m_bError = true;
					DebugTools::GetInstance().m_iDebugCounter = 400 + ucState;
				}
				m_eState = S_IDLE;				
			}
			break;
		}
		
		//send stop
		case S_STOP:
			if(TWIManager::GetInstance().SendStop())
			{
				m_eState = S_IDLE;
				TWIManager::GetInstance().Unregister();
				m_ulLastStopTime = Time::GetInstance().GetUS();
			}			
			break;
			
			
		//send start sequence
		case S_READ_START:
			if(TWIManager::GetInstance().SendStart())
				m_eState = S_READ_START_ACK;
			break;
			
		//wait for TWI
		case S_READ_START_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x08)
				m_eState = S_READ_PAGE_NO;
			else
				m_eState = S_IDLE;
			break;
		}
		
		//send device address sequence
		case S_READ_PAGE_NO:
			if(TWIManager::GetInstance().SendData(0xA0 + m_ucPage))
				m_eState = S_READ_PAGE_NO_ACK;
			break;
			
		case S_READ_PAGE_NO_ACK:
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
			if(TWIManager::GetInstance().SendData(m_ucAddress))
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
			if(TWIManager::GetInstance().SendStart())
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
			if(TWIManager::GetInstance().SendData(0xA1 + m_ucPage))
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
		//now, in MR mode, get data byte. We don't set TWEA, so no ACK is sent afterwards:
		case S_READ_DATA:
			if(TWIManager::GetInstance().SendData(0))
				m_eState = S_READ_DATA_ACK;
			break;
		
		case S_READ_DATA_ACK:
		{
			unsigned char ucState = TWIManager::GetInstance().GetLastState();
			if(ucState == 0) //TWI busy
				break;
			else if(ucState == 0x58)
			{
				m_lastReadData = TWIManager::GetInstance().ReadData();
				m_bDataReady = true;
				m_eState = S_STOP;
			}
			else
				m_eState = S_IDLE;
			break;
		}
		case S_IDLE:
			break;
	}
}