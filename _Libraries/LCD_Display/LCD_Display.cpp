/*
* LCD_Display.h
*
* Created: 2020-01-19 21:06:28
* Author: Gabriel
*/

#include "LCD_Display.h"
#include "../../_Libraries/DebugTools/DebugTools.h"

// default constructor
DisplayLCD::DisplayLCD()
{
	m_ucBufferStartPos = 0;
	m_ucBufferStartPos = 0;
	m_ulLastTime = 0;
	m_bWaitingForFunction = 0;
	m_bWaitingForClock = 0;
	m_bConfigured = false;
}

// default destructor
DisplayLCD::~DisplayLCD()
{
}

// Initialize LCD
//default display configuration after reset:
//DL = 1; 8-bit interface data
//N = 0; 1-line display
//F = 0; 5 × 8 dot character font
//D = 0; Display off
//C = 0; Cursor off
//B = 0; Blinking off
//I/D = 1; Increment by 1
//S = 0; No shift
void DisplayLCD::Initialize()
{
	if(m_bConfigured)
		return;
		
	//wait for display initialization (busy flag is high)
	_delay_ms(20);
	
	ConfigPorts();
	ConfigLCD();
	Time::GetInstance().Configure();
	
	m_ulLastTime = Time::GetInstance().GetUS();
	m_bConfigured = true;
}

//Configure ports
void DisplayLCD::ConfigPorts()
{
	//RS as output
	RS_DDR |= (1 << RS_PIN);
	
	//RW as output
	RW_DDR |= (1 << RW_PIN);
	
	//E as output
	E_DDR |= (1 << E_PIN);
	
	//D0 as output
	D0_DDR |= (1 << D0_PIN);
	
	//D1 as output
	D1_DDR |= (1 << D1_PIN);
	
	//D2 as output
	D2_DDR |= (1 << D2_PIN);
	
	//D3 as output
	D3_DDR |= (1 << D3_PIN);
	
	//D4 as output
	D4_DDR |= (1 << D4_PIN);
	
	//D5 as output
	D5_DDR |= (1 << D5_PIN);
	
	//D6 as output
	D6_DDR |= (1 << D6_PIN);
	
	//D7 as output
	D7_DDR |= (1 << D7_PIN);
}

//Configure LCD
void DisplayLCD::ConfigLCD()
{
	//unused pins set to 0
	D0_PORT &= ~(1 << D0_PIN);
	D1_PORT &= ~(1 << D1_PIN);
	D2_PORT &= ~(1 << D2_PIN);
	D3_PORT &= ~(1 << D3_PIN);
	
	//function set - change to 4 bit operation
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT |= (1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	//it is processed as 8 bit operation so only one write is needed
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(5);
		
		
	//function set
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT |= (1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(5);
	
	
	//display on/off control
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT |= (1 << D5_PIN);
	D6_PORT |= (1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	
	//Entry mode set
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT |= (1 << D5_PIN);
	D6_PORT |= (1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
}

void DisplayLCD::IncreaseBufferStartPos(unsigned char _ucIncreaseValue)
{
	m_ucBufferStartPos += _ucIncreaseValue;
	if(m_ucBufferStartPos >= WRITE_BUFFER_SIZE)
	m_ucBufferStartPos -= WRITE_BUFFER_SIZE;
}

void DisplayLCD::IncrementBufferEndPos()
{
	++m_ucBufferEndPos;
	if(m_ucBufferEndPos >= WRITE_BUFFER_SIZE)
		m_ucBufferEndPos = 0;	
}

//Return free space in buffer
unsigned char DisplayLCD::GetBufferSize()
{
	if(m_ucBufferEndPos >= m_ucBufferStartPos)
	{
		return WRITE_BUFFER_SIZE - (m_ucBufferEndPos - m_ucBufferStartPos);
	}
	
	return m_ucBufferStartPos - m_ucBufferEndPos;
}

/// Get available space in buffer for new text
unsigned char DisplayLCD::GetAvailableTextLength()
{
	return (GetBufferSize() / 2) - 1; //4bytes mode - 2 writes for every char. -1 for cursor move command
}

bool DisplayLCD::WriteText(const char *_strText, unsigned char _ucPosition, unsigned char _ucTextLen)
{
	if(!m_bConfigured)
		return false;
		
	if( (_ucPosition > LINE_LENGTH && _ucPosition < SECOND_LINE_ADDRESS)
		|| (_ucPosition > SECOND_LINE_ADDRESS + LINE_LENGTH) )
		return false;
		
	if( (_ucTextLen * 2) + 2 > GetBufferSize()) //4bytes mode - 2 writes for every char
		return false;
		
	MoveCursor(_ucPosition);
	
	for(int i = 0; i < _ucTextLen; i++)
	{
		PutChar(_strText[i]);		
	}
	
	return true;
}

void DisplayLCD::PutChar(unsigned char _ucChar)
{
	//if(_ucChar < ' ' || _ucChar > '}')
	//	return;
		
	m_bWriteBuffer[m_ucBufferEndPos][0] = 1; //RS
	m_bWriteBuffer[m_ucBufferEndPos][1] = 1 & (_ucChar >> 7); //D7
	m_bWriteBuffer[m_ucBufferEndPos][2] = 1 & (_ucChar >> 6); //D6
	m_bWriteBuffer[m_ucBufferEndPos][3] = 1 & (_ucChar >> 5); //D5
	m_bWriteBuffer[m_ucBufferEndPos][4] = 1 & (_ucChar >> 4); //D4
	IncrementBufferEndPos();
	
	m_bWriteBuffer[m_ucBufferEndPos][0] = 1; //RS
	m_bWriteBuffer[m_ucBufferEndPos][1] = 1 & (_ucChar >> 3); //D3
	m_bWriteBuffer[m_ucBufferEndPos][2] = 1 & (_ucChar >> 2); //D2
	m_bWriteBuffer[m_ucBufferEndPos][3] = 1 & (_ucChar >> 1); //D1
	m_bWriteBuffer[m_ucBufferEndPos][4] = 1 & _ucChar; //D0
	IncrementBufferEndPos();
}

void DisplayLCD::MoveCursor(unsigned char _ucPosition)
{
	m_bWriteBuffer[m_ucBufferEndPos][0] = 0; //RS
	m_bWriteBuffer[m_ucBufferEndPos][1] = 1; //D7
	m_bWriteBuffer[m_ucBufferEndPos][2] = 1 & (_ucPosition >> 6); //D6
	m_bWriteBuffer[m_ucBufferEndPos][3] = 1 & (_ucPosition >> 5); //D5
	m_bWriteBuffer[m_ucBufferEndPos][4] = 1 & (_ucPosition >> 4); //D4
	IncrementBufferEndPos();
	
	m_bWriteBuffer[m_ucBufferEndPos][0] = 0; //RS
	m_bWriteBuffer[m_ucBufferEndPos][1] = 1 & (_ucPosition >> 3); //D3
	m_bWriteBuffer[m_ucBufferEndPos][2] = 1 & (_ucPosition >> 2); //D2
	m_bWriteBuffer[m_ucBufferEndPos][3] = 1 & (_ucPosition >> 1); //D1
	m_bWriteBuffer[m_ucBufferEndPos][4] = 1 & _ucPosition; //D0
	IncrementBufferEndPos();
}

void DisplayLCD::Process()
{
	unsigned long currentTime = Time::GetInstance().GetUS();
	
	//wait to make a clock step
	if(m_bWaitingForClock)
	{
		if(currentTime - m_ulLastTime < CLOCK_TIME)
			return;
		else
		{
			m_bWaitingForClock = false;
			E_PORT &= ~(1 << E_PIN);
			m_bWaitingForFunction = true;
			return;
		}
	}
	
	//wait for LCD functioning
	if(m_bWaitingForFunction)
	{
		if(currentTime - m_ulLastTime < FUNCTION_TIME)
			return;
		else
		{
			m_bWaitingForFunction = false;
		}
	}
		
	//write next buffer
	if(m_bWriteBuffer[m_ucBufferStartPos][0])
		RS_PORT |= (1 << RS_PIN);
	else
		RS_PORT &= ~(1 << RS_PIN);
		
	if(m_bWriteBuffer[m_ucBufferStartPos][1])
		D7_PORT |= (1 << D7_PIN);
	else
		D7_PORT &= ~(1 << D7_PIN);
		
	if(m_bWriteBuffer[m_ucBufferStartPos][2])
		D6_PORT |= (1 << D6_PIN);
	else
		D6_PORT &= ~(1 << D6_PIN);
		
	if(m_bWriteBuffer[m_ucBufferStartPos][3])
		D5_PORT |= (1 << D5_PIN);
	else
		D5_PORT &= ~(1 << D5_PIN);
		
	if(m_bWriteBuffer[m_ucBufferStartPos][4])
		D4_PORT |= (1 << D4_PIN);
	else
		D4_PORT &= ~(1 << D4_PIN);
	
	//start clock
	E_PORT |= (1 << E_PIN);
	m_bWaitingForClock = true;
	IncreaseBufferStartPos(1);	
}
/*
void DisplayLCD::Test()
{
	//function set	
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);
	
	D2_PORT &= ~(1 << D2_PIN);
	D3_PORT |= (1 << D3_PIN);
	D4_PORT |= (1 << D4_PIN);
	D5_PORT |= (1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	
	//display on/off control
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D0_PORT &= ~(1 << D0_PIN);
	D1_PORT |= (1 << D1_PIN);
	D2_PORT |= (1 << D2_PIN);
	D3_PORT |= (1 << D3_PIN);
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	
	
	//Entry mode set
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D0_PORT &= ~(1 << D0_PIN);
	D1_PORT |= (1 << D1_PIN);
	D2_PORT |= (1 << D2_PIN);
	D3_PORT &= ~(1 << D3_PIN);
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	
	
	//H write
	RS_PORT |= (1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D0_PORT &= ~(1 << D0_PIN);
	D1_PORT &= ~(1 << D1_PIN);
	D2_PORT &= ~(1 << D2_PIN);
	D3_PORT |= (1 << D3_PIN);
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT |= (1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	
	//Set cursor to second line
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D0_PORT &= ~(1 << D0_PIN);
	D1_PORT &= ~(1 << D1_PIN);
	D2_PORT &= ~(1 << D2_PIN);
	D3_PORT &= ~(1 << D3_PIN);
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT |= (1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	
	//cos write
	RS_PORT |= (1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D0_PORT &= ~(1 << D0_PIN);
	D1_PORT &= ~(1 << D1_PIN);
	D2_PORT &= ~(1 << D2_PIN);
	D3_PORT |= (1 << D3_PIN);
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT |= (1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
}

void DisplayLCD::Test4()
{
	//unused pins set to 0
	D0_PORT &= ~(1 << D0_PIN);
	D1_PORT &= ~(1 << D1_PIN);
	D2_PORT &= ~(1 << D2_PIN);
	D3_PORT &= ~(1 << D3_PIN);
	
	//function set - change to 4 bit operation
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT |= (1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	//it is processed as 8 bit operation so only one write is needed
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(5);
	
	
	//function set
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT |= (1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(5);
	
	//display on/off control
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);	
	_delay_ms(1);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT |= (1 << D5_PIN);
	D6_PORT |= (1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	
	//Entry mode set
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT |= (1 << D5_PIN);
	D6_PORT |= (1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	
	//H write
	RS_PORT |= (1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT |= (1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= (1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	//Set cursor to second line
	RS_PORT &= ~(1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT |= (1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
		
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT &= ~(1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	//cos write
	RS_PORT |= (1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT |= (1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	//cos write
	RS_PORT |= (1 << RS_PIN);
	RW_PORT &= ~(1 << RW_PIN);

	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT |= (1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
	
	D4_PORT &= ~(1 << D4_PIN);
	D5_PORT &= ~(1 << D5_PIN);
	D6_PORT &= ~(1 << D6_PIN);
	D7_PORT |= (1 << D7_PIN);
	
	E_PORT |= (1 << E_PIN);
	_delay_ms(1);
	E_PORT &= ~(1 << E_PIN);
	_delay_ms(1);
}*/