/* 
* TWI_Manager.h
*
* Created: 2020-02-09 17:10:41
* Author: Gabriel
*/


#ifndef __TWI_MANAGER_H__
#define __TWI_MANAGER_H__
	
#include <avr/io.h>

class TWIManager
{
//variables
public:
protected:
private:
	bool m_bMutex;
	bool m_bConfigured;
	
//functions
public:	
	static TWIManager& GetInstance()
	{
		// instance
		static TWIManager instance;
		
		return instance;
	}
	
	void Configure();
	
	// Register manager
	bool Register();
	
	// Unregister manager
	void Unregister();
	
	//Get TWI state
	unsigned char GetLastState();
	
	//Send START
	bool SendStart();
	
	//Send Data
	bool SendData(unsigned char _ucData, bool _bSendACK = false);
	
	//Send STOP
	bool SendStop();
	
	//Read data
	unsigned char ReadData();
	
protected:
private:
	TWIManager();
	~TWIManager();
	TWIManager( const TWIManager &c );
	TWIManager& operator=( const TWIManager &c );
	
}; //TWIManager


#endif //__TWI_MANAGER_H__
