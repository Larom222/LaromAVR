/*
* Manchester.h
*
* Created: 2019-11-30 11:37:41
* Author: Gabriel
*/


#ifndef __MANCHESTER_H__
#define __MANCHESTER_H__
	
#include <avr/io.h>
#include "../../_Libraries/Time/Time.h"

//PD1 as output - RF
#define TX_PORT PORTD
#define TX_PIN 1

#define RX_PORT PIND
#define RX_PIN 2

#define RX_BUF_SIZE 16
#define BAUD_RATE 1000

#define CLOCK_TIME 2000UL //* 1000 // 1ms = 1000 us
#define STEP_TIME 1000UL //* 1000// 500 us
#define MIN_STEP_DELAY_TIME 850UL //* 1000 // 2ms
#define MAX_STEP_DELAY_TIME 1500UL //* 1000 // 2ms
#define MAX_DELAY_TIME 20000UL //* 1000 // 2ms

/*
#define CLOCK_TIME 1000000UL //* 1000 // 1ms = 1000 us
#define STEP_TIME 500000UL //* 1000// 500 us
#define MAX_STEP_DELAY_TIME 750000UL //* 1000 // 2ms
#define MAX_DELAY_TIME 10000000UL //* 1000 // 2ms
*/


#define FAKE_FRAME_ID 0
#define FAKE_FRAME_VAL 0

#define STX_CHAR 2
#define ETX_CHAR 3



class Manchester
{
	//friend void INT0_vect(void);
	
	enum RX_STATE
	{
		rxsWaitingForSynchro = 0,
		rxsWaitingFor2T = 1,
		rxsWaitingForSTX = 2,
		rxsWaitingForMessageId = 3,
		rxsWaitingForMessage = 4,
		rxsWaitingForETX = 5,
	};
	
public:
protected:
private:
	bool m_bTxConfigured;
	bool m_bRxConfigured;
	bool m_bTxBufferEmpty;
	
	bool m_bRxReadyToRead;
	unsigned char m_ucLastValidMessageId;
	unsigned char m_ucLastValidMessage;
	
	bool m_bWaitForStep;
	bool m_bWaitForShortStep;
	
	unsigned char m_ucTxBufferPos;
	unsigned char m_ucTxCharPos;
	
	unsigned long m_ulLastClock;
	unsigned long m_ulLastInterrupt;
	bool m_bTxBitBuffer[8];
	bool m_bRxBitBuffer[RX_BUF_SIZE];
	unsigned char m_ucMessageBuffer[4];
	
	/// start position of Rx bit buffer
	unsigned char m_ucRxBitBufferStartPos;
	/// end position of Rx bit buffer (next bit will be written to this index)
	unsigned char m_ucRxBitBufferEndPos;
	
	unsigned char m_ucRxState;
	
	bool DEBUG_m_bState;

//functions
public:
	static Manchester& GetInstance()
	{
		// instance
		static Manchester instance;
		
		return instance;
	}
	
	void ConfigureTx();
	void ConfigureRx();
	
	/// Process transmitting
	void ProcessTx();
	
	/// Process receiving
	void ProcessRx();
	
	bool IsRxBufferEmpty() {return m_bTxBufferEmpty;}
	
	bool SendMessage(unsigned char _ucMessageId, unsigned char _ucMessageVal);
	bool ReadMessage(unsigned char &_ucMessageId, unsigned char &_ucMessage);
	
	bool Debug_GetState() { return DEBUG_m_bState;}
	void Debug_Clear();
	
	
	//Interrupt
	void MakeStep();
protected:
private:
	Manchester();
	~Manchester();
	Manchester( const Manchester &c );
	Manchester& operator=( const Manchester &c );	
	
	void SetOutputHigh();
	void SetOutputLow();
	void SetClockOutput();
	void SetStepOutput();
	
	unsigned char GetRxBufferSize();
	void CopyBitBuffer(unsigned char _ucBufferIndex);
	void IncreaseBufferStartPos(unsigned char _ucIncreaseValue);
	void ReadRxBit();
	void ClearRxBuffer();
};

#endif //__MANCHESTER_H__