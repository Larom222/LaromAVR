/* 
* LCD_Display.h
*
* Created: 2020-01-19 21:06:28
* Author: Gabriel
*/


#ifndef __LCD_DISPLAY_H__
#define __LCD_DISPLAY_H__

#include "pin_config.h"
#include "../../_Libraries/Time/Time.h"
#include <avr/io.h>
#include <util/delay.h>

#define FIRST_LINE_ADDRESS 0x00
#define SECOND_LINE_ADDRESS 0x40
#define LINE_LENGTH 16
#define FUNCTION_TIME 41 //40us
#define CLOCK_TIME 10 //10us

class DisplayLCD
{
//variables
public:
protected:
private:

//functions
public:	
	// get display instance
	static DisplayLCD& GetInstance()
	{		
		/// Instance
		static DisplayLCD instance;
		
		return instance;
	}
	
	/*void Test();
	void Test2();
	void Test4();*/
	
	/// Get available space in buffer for new text
	unsigned char GetAvailableTextLength();
	
	/// Initialize LCD
	void Initialize();
	
	void Process();
	
	bool WriteText(const char *_strText, unsigned char _ucPosition, unsigned char _ucTextLen);
	
	bool ClearDisplay();
	
protected:
private:
	DisplayLCD();
	~DisplayLCD();
	DisplayLCD( const DisplayLCD &c );
	DisplayLCD& operator=( const DisplayLCD &c );
	
	/// Write buffer
	bool m_bWriteBuffer[WRITE_BUFFER_SIZE][5];
	
	/// Waiting for LCD function flag
	bool m_bWaitingForFunction;
	
	/// Waiting for clock step flag
	bool m_bWaitingForClock;
	
	/// Configured flag
	bool m_bConfigured;
	
	/// Last operation time
	unsigned long m_ulLastTime;
	
	/// Start position of buffer
	unsigned char m_ucBufferStartPos;
	/// End position buffer (next bit will be written to this index)
	unsigned char m_ucBufferEndPos;
		
	/// Move cursor to position
	void MoveCursor(unsigned char _ucPosition);
	
	/// Put single char to buffer
	void PutChar(unsigned char _ucChar);
	
	void IncreaseBufferStartPos(unsigned char _ucIncreaseValue);
	void IncrementBufferEndPos();
	
	/// Return free space in buffer
	unsigned char GetBufferSize();
	
	void ConfigPorts();
	void ConfigLCD();

}; //DisplayLCD

#endif //__LCD_DISPLAY_H__
