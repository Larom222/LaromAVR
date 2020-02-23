/*
* TWI_Manager.cpp
*
* Created: 2020-02-09 17:10:41
* Author: Gabriel
*/


#include "TWI_Manager.h"

// default constructor
TWIManager::TWIManager()
{
	m_bMutex = false;
	m_bConfigured = false;
} //TWIManager

// default destructor
TWIManager::~TWIManager()
{
} //~TWIManager

// Register manager
bool TWIManager::Register()
{
	if(!m_bConfigured)
		return false;
		
	if(m_bMutex)
		return false;
		
	m_bMutex = true;
	return true;
}

// Unregister manager
void TWIManager::Unregister()
{
	m_bMutex = false;
}

void TWIManager::Configure()
{
	TWBR = 32;
	m_bConfigured = true;
}

//Send START
bool TWIManager::SendStart()
{
	if(!m_bConfigured || !m_bMutex)
		return false;
		
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	return true;
}

//Send Data
bool TWIManager::SendData(unsigned char _ucData, bool _bSendACK)
{
	if(!m_bConfigured || !m_bMutex)
	return false;
	
	TWDR = _ucData;
	if(_bSendACK)
		TWCR = (1<<TWINT)|(1<<TWEN|(1<<TWEA));
	else
		TWCR = (1<<TWINT)|(1<<TWEN);
	return true;
}

//Send STOP
bool TWIManager::SendStop()
{
	if(!m_bConfigured || !m_bMutex)
	return false;
	
	TWCR = ((1<<TWINT)|(1<<TWEN)|(1<<TWSTO));
	return true;
}

//Get TWI state
unsigned char TWIManager::GetLastState()
{	
	//busy
	if(!(TWCR & (1<<TWINT)))
		return 0;
		
	return TWSR & 0xF8;
}

//Read data
unsigned char TWIManager::ReadData()
{
	return TWDR;
}