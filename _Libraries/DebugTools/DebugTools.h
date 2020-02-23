/*
* DebugTools.h
*
* Created: 2019-12-01 17:10:41
* Author: Gabriel
*/


#ifndef __DEBUG_TOOLS_H__
#define __DEBUG_TOOLS_H__

#include <avr/io.h>
//#include <util/delay.h>
#include <avr/interrupt.h>

ISR (TIMER0_OVF_vect);

class DebugTools
{	
	//variables
	public:
	bool m_bError;
	bool m_bProcessing;
	int m_iDebugCounter;
	int m_iDebugCounter2;
	
	protected:
	private:
	
	//functions
	public:
	static DebugTools& GetInstance()
	{
		// instance
		static DebugTools instance;
		
		return instance;
	}
	
	protected:
	private:
	DebugTools();
	~DebugTools();
	DebugTools( const DebugTools &c );
	DebugTools& operator=( const DebugTools &c );
	

}; //DebugTools

#endif //__DEBUG_TOOLS_H__
