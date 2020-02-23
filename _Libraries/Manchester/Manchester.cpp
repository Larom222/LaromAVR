/*
* Manchester.cpp
*
* Created: 2019-11-30 11:37:41
* Author: Gabriel
*/


#include "Manchester.h"
#include "../../_Libraries/DebugTools/DebugTools.h"

// default constructor
Manchester::Manchester()
{
	m_bTxConfigured = false;
	m_bRxConfigured = false;
	
	m_bTxBufferEmpty = true;
	m_bWaitForStep = false;
	m_bWaitForShortStep = false;
	
	m_ucTxBufferPos = 0;
	m_ucTxCharPos = 0;
	m_ulLastClock = 0;
	m_ulLastInterrupt = 0;
	
	m_ucRxBitBufferStartPos = 0;
	m_ucRxBitBufferEndPos = 0;
	m_ucRxState = rxsWaitingForSynchro;
	
	m_bRxReadyToRead = false;
	m_ucLastValidMessageId = 0;
	m_ucLastValidMessage = 0;
	
	DEBUG_m_bState = 0;
} //Manchester

void Manchester::Debug_Clear()
{
	m_bTxConfigured = false;
	m_bRxConfigured = false;
	
	m_bTxBufferEmpty = true;
	m_bWaitForStep = false;
	m_bWaitForShortStep = false;
	
	m_ucTxBufferPos = 0;
	m_ucTxCharPos = 0;
	m_ulLastClock = 0;
	m_ulLastInterrupt = 0;
	
	m_ucRxBitBufferStartPos = 0;
	m_ucRxBitBufferEndPos = 0;
	m_ucRxState = rxsWaitingForSynchro;
	
	m_bRxReadyToRead = false;
	m_ucLastValidMessageId = 0;
	m_ucLastValidMessage = 0;
	
	DEBUG_m_bState = 0;
}

// default destructor
Manchester::~Manchester()
{
} //~Manchester

void Manchester::ConfigureTx()
{	
	//set output - RF
	DDRD |= (1 << 1);
	TX_PORT &= !(1 << TX_PIN);
	
	m_bTxConfigured = true;
}

void Manchester::ConfigureRx()
{	
	// EXT INT0 conf - any logical change
	MCUCR |= (1<<ISC00);
	GICR |= (1<<INT0);
		
	//INT0 as input
	DDRD &= ~(1 << RX_PIN);
	PORTD |= (1<<RX_PIN);
	sei();
	
	m_bRxConfigured = true;
}

//Set output pin high
void Manchester::SetOutputHigh()
{
	TX_PORT |= (1 << TX_PIN);
	//DebugTools::GetInstance().m_iDebugCounter++;
	DEBUG_m_bState = true;
}

//Set output pin low
void Manchester::SetOutputLow()
{		
	TX_PORT &= !(1 << TX_PIN);
	//DebugTools::GetInstance().m_iDebugCounter++;
	DEBUG_m_bState = false;
}

//Put message into tx buffer
bool Manchester::SendMessage(unsigned char _ucMessageId, unsigned char _ucMessageVal)
{
	if(!m_bTxBufferEmpty || !m_bTxConfigured)
		return false;
			
	bool ret = true;
		
	//get current time
	unsigned long ulCurrentTime = Time::GetInstance().GetUS();
	unsigned long ulTimeElapsed = ulCurrentTime - m_ulLastClock;
	
	//if max delay time exceeded, put fake frame into tx buffer
	if(ulCurrentTime < MAX_DELAY_TIME //first frame after start
		|| ulTimeElapsed >= m_ulLastClock + MAX_DELAY_TIME)
	{				
		m_ucMessageBuffer[0] = STX_CHAR;
		m_ucMessageBuffer[1] = FAKE_FRAME_ID;
		m_ucMessageBuffer[2] = FAKE_FRAME_VAL;
		m_ucMessageBuffer[3] = ETX_CHAR;	
		ret = false; //fake frame must be sent first	
	}
	else //otherwise put frame into tx buffer
	{
		m_ucMessageBuffer[0] = STX_CHAR;
		m_ucMessageBuffer[1] = _ucMessageId;
		m_ucMessageBuffer[2] = _ucMessageVal;
		m_ucMessageBuffer[3] = ETX_CHAR;
	}
		
	m_ulLastClock = ulCurrentTime;
	
	m_ucTxBufferPos = 0;
	m_ucTxCharPos = 0;
	m_bTxBufferEmpty = false;
	SetClockOutput();
	m_bWaitForStep = true;
		
	return ret;
}

void Manchester::SetClockOutput()
{	
	if((m_ucMessageBuffer[m_ucTxBufferPos] >> (7 - m_ucTxCharPos)) & 1)
	{
		SetOutputHigh();
	}
	else
	{
		SetOutputLow();
	}
}

void Manchester::SetStepOutput()
{
	if((m_ucMessageBuffer[m_ucTxBufferPos] >> (7 - m_ucTxCharPos)) & 1)
		SetOutputLow();
	else
		SetOutputHigh();
}

/// Process task
void Manchester::ProcessTx()
{
	//nothing to do
	if(m_bTxBufferEmpty)
		return;
		
	//get current time
	unsigned long ulCurrentTime = Time::GetInstance().GetUS();
		
	//process clock
	if(!m_bWaitForStep && ulCurrentTime > m_ulLastClock + CLOCK_TIME)
	{
		//make clock - go to next bit
		++m_ucTxCharPos;
		
		//check if byte pos should be incremented
		if(m_ucTxCharPos >= 8)
		{
			m_ucTxCharPos = 0;
			++m_ucTxBufferPos;
		}
		
		//check if end of message
		if(m_ucTxBufferPos >= 4)
		{
			m_ucTxBufferPos = 0;
			m_bTxBufferEmpty = true;
		}
		else
		{			
			SetClockOutput();
			m_bWaitForStep = true;
		}
						
		//store last clock time
		m_ulLastClock = ulCurrentTime;
	}
	//process step
	else if(m_bWaitForStep && ulCurrentTime > m_ulLastClock + STEP_TIME)
	{
		//make step - change output
		SetStepOutput();
		m_bWaitForStep = false;
	}
}

/// Process Rx task
void Manchester::ProcessRx()
{	
	if(GetRxBufferSize() < 8)
		return;	
	
	switch(m_ucRxState)
	{
		case rxsWaitingForSTX:
		{			
			// clear message buffer
			m_ucMessageBuffer[0] = 0;
				
			// write 8 bits from bit buffer to message buffer
			CopyBitBuffer(0);
			
			/*DebugTools::GetInstance().m_bError = true;
			DebugTools::GetInstance().m_iDebugCounter = m_ucMessageBuffer[0];*/
			
			// check if message is STX
			if(m_ucMessageBuffer[0] == STX_CHAR)
			{
				// STX found, wait for message id
				m_ucRxState = rxsWaitingForMessageId;
				IncreaseBufferStartPos(8);
			}
			else
			{
				// increase start index and wait for next bit
				IncreaseBufferStartPos(1);
			}	
		}
		break;
		
		case rxsWaitingForMessageId:
		{			
			// clear message buffer
			m_ucMessageBuffer[1] = 0;
				
			// write 8 bits from bit buffer to message buffer
			CopyBitBuffer(1);
			
			/*DebugTools::GetInstance().m_bError = true;
			DebugTools::GetInstance().m_iDebugCounter = m_ucMessageBuffer[1];
			DebugTools::GetInstance().m_iDebugCounter += 1000;*/
				
			m_ucRxState = rxsWaitingForMessage;
			IncreaseBufferStartPos(8);		
		}
		break;
		
		case rxsWaitingForMessage:
		{
			// clear message buffer
			m_ucMessageBuffer[2] = 0;
				
			// write 8 bits from bit buffer to message buffer
			CopyBitBuffer(2);
			
			/*DebugTools::GetInstance().m_bError = true;
			DebugTools::GetInstance().m_iDebugCounter = m_ucMessageBuffer[2];
			DebugTools::GetInstance().m_iDebugCounter += 2000;*/
				
			m_ucRxState = rxsWaitingForETX;
			IncreaseBufferStartPos(8);
		}
		break;
		
		case rxsWaitingForETX:
		{
			// clear message buffer
			m_ucMessageBuffer[3] = 0;
				
			// write 8 bits from bit buffer to message buffer
			CopyBitBuffer(3);	
			
			/*DebugTools::GetInstance().m_bError = true;
			DebugTools::GetInstance().m_iDebugCounter = m_ucMessageBuffer[3];
			DebugTools::GetInstance().m_iDebugCounter += 3000;	*/

			// check if message is ETX
			if(m_ucMessageBuffer[3] == ETX_CHAR)
			{
				m_ucLastValidMessageId = m_ucMessageBuffer[1];
				m_ucLastValidMessage = m_ucMessageBuffer[2];
				m_bRxReadyToRead = true;
				
				// frame valid. Go to start, wait for STX
				m_ucRxState = rxsWaitingForSTX;
				IncreaseBufferStartPos(8);
			}
			else
			{				
				bool bSTXFound = false;
				for(int i = 1; i < 24; i++)
				{
					//move whole buffer
					m_ucMessageBuffer[0] = m_ucMessageBuffer[0] << 1;
					m_ucMessageBuffer[0] |= m_ucMessageBuffer[1] >> 7;
					
					m_ucMessageBuffer[1] = m_ucMessageBuffer[1] << 1;
					m_ucMessageBuffer[1] |= m_ucMessageBuffer[2] >> 7;
					
					m_ucMessageBuffer[2] = m_ucMessageBuffer[2] << 1;
					m_ucMessageBuffer[2] |= m_ucMessageBuffer[3] >> 7;
					
					m_ucMessageBuffer[3] = m_ucMessageBuffer[3] << 1;
					
					//check for STX char
					if(m_ucMessageBuffer[0] == STX_CHAR)
					{
						//update status
						if(i < 8)
							m_ucRxState = rxsWaitingForETX;
						else if(i < 16)
							m_ucRxState = rxsWaitingForMessage;
						else if(i < 24)
							m_ucRxState = rxsWaitingForMessageId;
							
						//update buffer position
						IncreaseBufferStartPos(i % 8);
						bSTXFound = true;
						break;
					}
				}
				
				//STX not found in the whole buffer.
				//Back to first status and increase buffer position
				if(!bSTXFound)
				{
					m_ucRxState = rxsWaitingForSTX;
					IncreaseBufferStartPos(1);
					break;
				}
				
				// frame invalid. Search for STX char in buffer
				/*if(m_ucMessageBuffer[1] == STX_CHAR)
				{
					m_ucMessageBuffer[0] = m_ucMessageBuffer[1];
					m_ucMessageBuffer[1] = m_ucMessageBuffer[2];
					m_ucMessageBuffer[2] = m_ucMessageBuffer[3];
					m_ucRxState = rxsWaitingForETX;
				}
				else if(m_ucMessageBuffer[2] == STX_CHAR)
				{
					m_ucMessageBuffer[0] = m_ucMessageBuffer[2];
					m_ucMessageBuffer[1] = m_ucMessageBuffer[3];
					m_ucRxState = rxsWaitingForMessage;
				}
				else if(m_ucMessageBuffer[3] == STX_CHAR)
				{
					m_ucMessageBuffer[0] = m_ucMessageBuffer[3];
					m_ucRxState = rxsWaitingForMessageId;
				}
				else
				{					
					//Start analyse from the beginning
					m_ucRxState = rxsWaitingForSTX;
					IncreaseBufferStartPos(1);
					break;
				}*/
			}	
		}
		break;
	}	
}

bool Manchester::ReadMessage(unsigned char &_ucMessageId, unsigned char &_ucMessage)
{
	if(m_bRxReadyToRead)
	{
		_ucMessageId = m_ucLastValidMessageId;
		_ucMessage = m_ucLastValidMessage;
		m_bRxReadyToRead = false;
		return true;
	}
	
	return false;
}

void Manchester::IncreaseBufferStartPos(unsigned char _ucIncreaseValue)
{
	m_ucRxBitBufferStartPos += _ucIncreaseValue;
	if(m_ucRxBitBufferStartPos >= RX_BUF_SIZE)
		m_ucRxBitBufferStartPos -= RX_BUF_SIZE;
}

void Manchester::CopyBitBuffer(unsigned char _ucBufferIndex)
{
	m_ucMessageBuffer[_ucBufferIndex] = 0;
	for(unsigned int i = 0; i < 8; i++)
	{
		unsigned int index = (i + m_ucRxBitBufferStartPos) % RX_BUF_SIZE;		
		m_ucMessageBuffer[_ucBufferIndex] |= m_bRxBitBuffer[index] << (7 - i);
	}
}

unsigned char Manchester::GetRxBufferSize()
{	
	if(m_ucRxBitBufferEndPos < m_ucRxBitBufferStartPos)
	{
		return RX_BUF_SIZE - (m_ucRxBitBufferStartPos - m_ucRxBitBufferEndPos);
	}
	
	return m_ucRxBitBufferEndPos - m_ucRxBitBufferStartPos;	
}

void Manchester::ReadRxBit()
{	
	bool pinState = RX_PORT & (1 << RX_PIN);
		
	m_bRxBitBuffer[m_ucRxBitBufferEndPos] = !pinState;
	++m_ucRxBitBufferEndPos;
	if(m_ucRxBitBufferEndPos >= RX_BUF_SIZE)
		m_ucRxBitBufferEndPos = 0;
}

void Manchester::ClearRxBuffer()
{			
	m_ucRxBitBufferEndPos = 0;
	m_ucRxBitBufferStartPos = 0;
}

void Manchester::MakeStep()
{					
	DebugTools::GetInstance().m_bProcessing = true;
	
	unsigned long ulCurrentTime = Time::GetInstance().GetUS();
		
	// Wait for first interrupt
	if(m_ucRxState == rxsWaitingForSynchro)
	{		
		m_ulLastInterrupt = ulCurrentTime;
		m_ucRxState = rxsWaitingFor2T;
		
		DebugTools::GetInstance().m_bProcessing = false;
		return;
	}
	
	unsigned long ulTimeElapsed = ulCurrentTime - m_ulLastInterrupt;
	m_ulLastInterrupt = ulCurrentTime;
			
			
			
								
	//if(ulTimeElapsed < MIN_STEP_DELAY_TIME)
	//if(DebugTools::GetInstance().m_bError == false)
	{
		//DebugTools::GetInstance().m_bError = true;
		//DebugTools::GetInstance().m_iDebugCounter = ulTimeElapsed;
	}
	
	//if max step time exceeded or step too fast, reset the Rx buffer	
	if(ulTimeElapsed > STEP_TIME + MAX_STEP_DELAY_TIME || ulTimeElapsed < MIN_STEP_DELAY_TIME)
	{					
		ClearRxBuffer();
		m_ucRxState = rxsWaitingFor2T;
		DebugTools::GetInstance().m_bProcessing = false;
		return;
	}
	
	//wait for long step (2T) to get synchronized with clock
	if(m_ucRxState == rxsWaitingFor2T)
	{
		//long step detected
		if(ulTimeElapsed > CLOCK_TIME)
		{			
			//synchonized succesfully
			m_ucRxState = rxsWaitingForSTX;
			m_bWaitForShortStep = false;
			DebugTools::GetInstance().m_bProcessing = false;
			return;
		}	
		else
		{
			//if short step, wait for another one
			DebugTools::GetInstance().m_bProcessing = false;
			return;
		}			
	}
		
	//read bit
	if(m_ucRxState > rxsWaitingFor2T)
	{				
		//start of clock with state change. Wait for short step
		if((m_bWaitForShortStep == false) && (ulTimeElapsed <= MAX_STEP_DELAY_TIME))
		{
			m_bWaitForShortStep = true;
			DebugTools::GetInstance().m_bProcessing = false;
			return;
		}
		
		//full step detected. Read bit
		if((m_bWaitForShortStep == false) && (ulTimeElapsed >= CLOCK_TIME))
		{
			Manchester::GetInstance().ReadRxBit();
			DebugTools::GetInstance().m_bProcessing = false;
			return;
		}
		
		//full step detected. Read bit
		if((m_bWaitForShortStep == true) && (ulTimeElapsed <= MAX_STEP_DELAY_TIME))
		{			
			Manchester::GetInstance().ReadRxBit();
			m_bWaitForShortStep = false;
			DebugTools::GetInstance().m_bProcessing = false;
			return;
		}
		
		//there should be a state change in the middle of the clock. Invalid state detected
		if((m_bWaitForShortStep == true) && (ulTimeElapsed > MAX_STEP_DELAY_TIME))
		{			
			ClearRxBuffer();
			m_ucRxState = rxsWaitingFor2T;
			DebugTools::GetInstance().m_bProcessing = false;
			return;
		}
	}
	
	DebugTools::GetInstance().m_bProcessing = false;
}

ISR(INT0_vect)
{	
	/*
	if(DebugTools::GetInstance().m_bProcessing == true)
	{
		DebugTools::GetInstance().m_bError = true;
		DebugTools::GetInstance().m_iDebugCounter++;
	}*/
	
	
	Manchester::GetInstance().MakeStep();
}
