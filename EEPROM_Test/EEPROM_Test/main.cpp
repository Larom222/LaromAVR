/*
 * EEPROM_Test.cpp
 *
 * Created: 2020-02-09 14:59:32
 * Author : Gabriel
 */ 

#include <avr/io.h>
#include "../../_Libraries/LCD_Display/LCD_Display.h"
#include "../../_Libraries/EEPROM_Manager/EEPROM_Manager.h"
#include "../../_Libraries/DebugTools/DebugTools.h"

bool startError = false;
bool start2Error = false;
bool repeatedStartError = false;
bool slaError = false;
bool sla2Error = false;
bool addressError = false;
bool address2Error = false;
bool dataError = false;
bool data2Error = false;

void Write()
{	
	//send start
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	//Wait for TWINT Flag set. This indicates that the START condition has been transmitted
	while (!(TWCR & (1<<TWINT)));
	
	if ((TWSR & 0xF8) != 0x08)
	{
		startError = true;
	}
	
	//Load SLA_W into TWDR Register. Clear TWINT bit in TWCR to start transmission of address
	TWDR = 0xA0;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x18)
	{
		slaError = true;
	}
	
	//send EEPROM memory address
	TWDR = 2;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x28)
	{
		addressError = true;
	}
	
	//send EEPROM data
	TWDR = 0x27;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x28)
	{
		dataError = true;
	}
	
	//send STOP
	TWCR = ((1<<TWINT)|(1<<TWEN)|(1<<TWSTO));
}

void Read()
{
	//send start
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	//Wait for TWINT Flag set. This indicates that the START condition has been transmitted
	while (!(TWCR & (1<<TWINT)));
	
	if ((TWSR & 0xF8) != 0x08)
	{
		start2Error = true;
	}
	
	//Load SLA_W into TWDR Register. Clear TWINT bit in TWCR to start transmission of address
	TWDR = 0xA0;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x18)
	{
		sla2Error = true;
	}
	
	//send EEPROM memory address
	TWDR = 2;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x28)
	{
		addressError = true;
	}
	
	//repeat start
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	//Wait for TWINT Flag set. This indicates that the START condition has been transmitted
	while (!(TWCR & (1<<TWINT)));
	
	if ((TWSR & 0xF8) != 0x10)
	{
		repeatedStartError = true;
	}
	
	//send slave address, read bit = 1; MR mode!
	TWDR = 0xA1;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x40)
	{
		address2Error = true;
	}
	
	//now, in MR mode, get data byte. We don't set TWEA, so no ACK is sent afterwards:
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if (TWDR != 0x27)
	{
		data2Error = true;
	}
		
	//send STOP
	TWCR = ((1<<TWINT)|(1<<TWEN)|(1<<TWSTO));
}

int main(void)
{
	DisplayLCD::GetInstance().Initialize();
	EEPROMManager::GetInstance().Configure();
		
	/*
	Write();
	_delay_ms(3);
	Read();
	
	char buf[12];
	
	buf[0] = startError ? '1' : '0';
	buf[1] = start2Error ? '1' : '0';
	buf[2] = repeatedStartError ? '1' : '0';
	buf[3] = slaError ? '1' : '0';
	buf[4] = sla2Error ? '1' : '0';
	buf[5] = addressError ? '1' : '0';
	buf[6] = address2Error ? '1' : '0';
	buf[7] = dataError ? '1' : '0';
	buf[8] = data2Error ? '1' : '0';
	buf[9] = ' ';
	buf[10] = 'O';
	buf[11] = 'K';
	*/
	
	int state = 0;
	int read = 0;
	
	/*
	EEPROMManager::GetInstance().Register();
	
	EEPROMManager::GetInstance().WriteByte(0, 2, 22);
	EEPROMManager::GetInstance().ReadByte(0, 2);
	while(!EEPROMManager::GetInstance().ReadyToRead());
	int res = EEPROMManager::GetInstance().GetLastRead();
	
	EEPROMManager::GetInstance().Unregister();
	*/
	
    /* Replace with your application code */
    while (1) 
    {	
		
		switch(state)
		{
			case 0:
			state = 6;
			/*
				if(EEPROMManager::GetInstance().Register())
					++state;
			*/
				break;
			case 1:				
				if(EEPROMManager::GetInstance().WriteByte(0, 1, 11))
					++state;
				break;
			case 2:
				if(EEPROMManager::GetInstance().WriteByte(0, 2, 22))
					++state;
				break;
			case 3:
				if(EEPROMManager::GetInstance().WriteByte(0, 3, 33))
					++state;
				break;
			case 4:
				if(EEPROMManager::GetInstance().WriteByte(0, 4, 44))
					++state;
				break;
			case 5:
				EEPROMManager::GetInstance().Unregister();
				++state;
				break;
				
				
			case 6:
				if(EEPROMManager::GetInstance().Register())
					++state;
				break;
			case 7:
				if(EEPROMManager::GetInstance().ReadByte(0, 1))
					++state;
				break;
			case 8:
				if(EEPROMManager::GetInstance().ReadyToRead())
				{
					read += EEPROMManager::GetInstance().GetLastRead();
					++state;
				}
				break;
			case 9:
				if(EEPROMManager::GetInstance().ReadByte(0, 2))
					++state;
				break;				
			case 10:
				if(EEPROMManager::GetInstance().ReadyToRead())
				{
					read += EEPROMManager::GetInstance().GetLastRead();
					++state;
				}
				break;
			case 11:
				if(EEPROMManager::GetInstance().ReadByte(0, 3))
					++state;
				break;
			case 12:
				if(EEPROMManager::GetInstance().ReadyToRead())
				{
					read += EEPROMManager::GetInstance().GetLastRead();
					++state;
				}
				break;
			case 13:
				if(EEPROMManager::GetInstance().ReadByte(0, 4))
					++state;
				break;
			case 14:
				if(EEPROMManager::GetInstance().ReadyToRead())
				{
					read += EEPROMManager::GetInstance().GetLastRead();
					++state;
				}
				break;
			case 15:
				EEPROMManager::GetInstance().Unregister();
				break;
		}
		
		if(DisplayLCD::GetInstance().GetAvailableTextLength() > 16)
		{
			if(read == 0)
				DisplayLCD::GetInstance().WriteText("CZEKAM", FIRST_LINE_ADDRESS, 6);
			else if(read == 110)
				DisplayLCD::GetInstance().WriteText("  OK  ", FIRST_LINE_ADDRESS, 6);
			else
				DisplayLCD::GetInstance().WriteText("ERROR ", FIRST_LINE_ADDRESS, 6);
				
			char s[1];
			s[0] = state + 48;
			DisplayLCD::GetInstance().WriteText(s, SECOND_LINE_ADDRESS, 1);
			
			
			if(DebugTools::GetInstance().m_bError)
				DisplayLCD::GetInstance().WriteText("ERR", SECOND_LINE_ADDRESS + 5, 3);
			else
				DisplayLCD::GetInstance().WriteText("OK ", SECOND_LINE_ADDRESS + 5, 3);
			
			
			char err[3];
			int iErr = read;
			err[2] = (iErr % 10) + 48;
			iErr /= 10;
			err[1] = (iErr % 10) + 48;
			iErr /= 10;
			err[0] = (iErr % 10) + 48;
			
			DisplayLCD::GetInstance().WriteText(err, SECOND_LINE_ADDRESS + 10, 3);
		}
		
		/*
		if(DisplayLCD::GetInstance().GetAvailableTextLength() > 13)
		{			
			DisplayLCD::GetInstance().WriteText(buf, FIRST_LINE_ADDRESS, 12);
		}
		*/
		
		DisplayLCD::GetInstance().Process();
		EEPROMManager::GetInstance().Process();
    }
}

