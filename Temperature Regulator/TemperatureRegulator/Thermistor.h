/* 
* Thermistor.h
*
* Created: 2020-02-21 18:26:40
* Author: Gabriel
*/


#ifndef __THERMISTOR_H__
#define __THERMISTOR_H__

#include <avr/io.h>

#define ADC_CHANNEL 3

class Thermistor
{
//variables
public:
protected:
private:
	bool m_bConfigured;
	
//functions
public:
	static Thermistor& GetInstance()
	{
		// instance
		static Thermistor instance;
		
		return instance;
	}
	
	void Configure();
	bool ReadTemperature();	
	bool ReadyToRead();
	unsigned short GetLastTemperature();
	
protected:
private:
	Thermistor();
	~Thermistor();
	Thermistor( const Thermistor &c );
	Thermistor& operator=( const Thermistor &c );

}; //Thermistor

#endif //__THERMISTOR_H__
