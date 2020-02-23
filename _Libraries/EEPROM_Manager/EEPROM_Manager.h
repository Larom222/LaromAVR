/* 
* EEPROM_Manager.h
*
* Created: 2020-02-09 17:10:41
* Author: Gabriel
*/


#ifndef __EEPROM_MANAGER_H__
#define __EEPROM_MANAGER_H__
	
#include <avr/io.h>
#include "../../_Libraries/TWI_Manager/TWI_Manager.h"
#include "../../_Libraries/Time/Time.h"

#define STOP_DELAY 5000

class EEPROMManager
{
	enum E_STATE
	{
		S_IDLE = 0,
		
		S_WRITE_START,
		S_WRITE_START_ACK,
		S_WRITE_PAGE_NO,
		S_WRITE_PAGE_NO_ACK,
		S_WRITE_ADDRESS,
		S_WRITE_ADDRESS_ACK,
		S_WRITE_DATA,
		S_WRITE_DATA_ACK,
		
		S_READ_START,
		S_READ_START_ACK,
		S_READ_PAGE_NO,
		S_READ_PAGE_NO_ACK,
		S_READ_ADDRESS,
		S_READ_ADDRESS_ACK,
		S_READ_START_2,
		S_READ_START_2_ACK,
		S_READ_ADDRESS_2,
		S_READ_ADDRESS_2_ACK,
		S_READ_DATA,
		S_READ_DATA_ACK,
		
		S_STOP,
	};
//variables
public:
protected:
private:


	bool m_bMutex;
	bool m_bConfigured;
	bool m_bDataReady;
	
	unsigned char m_ucPage;
	unsigned char m_ucAddress;
	unsigned char m_ucData;
	
	unsigned char m_lastReadData;
	
	E_STATE m_eState;
	
	unsigned long m_ulLastStopTime;
	
//functions
public:	
	static EEPROMManager& GetInstance()
	{
		// instance
		static EEPROMManager instance;
		
		return instance;
	}
	
	void Configure();
	
	// Register manager
	bool Register();
	
	// Unregister manager
	void Unregister();
	
	//Write byte
	bool WriteByte(unsigned char _ucPage, unsigned char _ucAddress, unsigned char _ucData);
	
	//Read byte
	bool ReadByte(unsigned char _ucPage, unsigned char _ucAddress);
	
	//Check if data successfully received and is ready to read
	bool ReadyToRead() {return m_bDataReady;};
		
	//Return last read byte
	unsigned char GetLastRead() {return m_lastReadData;};
		
	//Process
	void Process();
	
protected:
private:
	EEPROMManager();
	~EEPROMManager();
	EEPROMManager( const EEPROMManager &c );
	EEPROMManager& operator=( const EEPROMManager &c );
	
}; //EEPROMManager


#endif //__EEPROM_MANAGER_H__
