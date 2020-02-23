/* 
* Display4x7Seg.h
*
* Created: 2019-11-07 21:06:28
* Author: Gabriel
*/


#ifndef __DISPLAY4X7SEG_H__
#define __DISPLAY4X7SEG_H__

#define F_CPU 8000000UL //8 MHz

#include "pin_config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// display time [ms] of single digit
#define DISPLAY_SHOW_MS 1

#define DISPLAY_TEST 0
#define USE_TIMER 0

#if DISPLAY_TEST == 1
	int testCounter = 1;
	int testOverflow = 0;
#endif

class Display4x7Seg
{
//variables
public:
protected:
private:

//functions
public:	
	// get display instance
	static Display4x7Seg& GetInstance()
	{		
		// instance
		static Display4x7Seg instance;
		
		return instance;
	}
	
	// show number on display
	void ShowNumber(unsigned int number);
	
	// set number to show in timer mode
	void SetNumber(unsigned int number);
	
	// set timer0 and interrupt
	void SetTimer();
	
	// show next digit
	void ShowNextDigit();
	
#if DISPLAY_TEST == 1
	void TestMux();
	void TestNumber();
#endif

protected:
private:
	Display4x7Seg();
	~Display4x7Seg();
	Display4x7Seg( const Display4x7Seg &c );
	Display4x7Seg& operator=( const Display4x7Seg &c );
	
	// number to display
	int mNumber;
	
	// 1 of 4 digit to display
	char mNextDigit;

	// Config ports for MUX and display
	void ConfigPorts();
	
	// show single digit
	void ShowDigit(unsigned char digit, unsigned char display);

}; //Display4x7Seg

#endif //__DISPLAY4X7SEG_H__
