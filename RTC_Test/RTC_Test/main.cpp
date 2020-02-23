/*
 * RTC_Test.cpp
 *
 * Created: 2020-02-16 13:28:39
 * Author : Gabriel
 */ 

#include <avr/io.h>
#include "../../_Libraries/LCD_Display/LCD_Display.h"
#include "../../_Libraries/RTC_Manager/RTC_Manager.h"


int main(void)
{
	
	DisplayLCD::GetInstance().Initialize();
	RTCManager::GetInstance().Configure();
	Time::GetInstance().Configure();
	
	int state = 0;
	DateTime dateTime;
	long long lastRead = Time::GetInstance().GetUS();
	
	/*
	DateTime dateTimeToWrite;
	dateTimeToWrite.m_day = 16;
	dateTimeToWrite.m_month = 2;
	dateTimeToWrite.m_year = 20;
	dateTimeToWrite.m_hour = 18;
	dateTimeToWrite.m_minute = 8;
	dateTimeToWrite.m_second = 20;
	dateTimeToWrite.m_dayOfWeek = 1;
	
	RTCManager::GetInstance().Register();
	RTCManager::GetInstance().WriteDateTime(dateTimeToWrite);
	RTCManager::GetInstance().Unregister();
	*/
	
	
    /* Replace with your application code */
    while (1) 
    {
		switch(state)
		{
			case 0:
				if(RTCManager::GetInstance().Register())
					++state;
				break;
			case 1:				
				if(RTCManager::GetInstance().ReadDateTime())
					++state;
				break;
			case 2:
				if(RTCManager::GetInstance().ReadyToRead())
				{
					dateTime = RTCManager::GetInstance().GetLastDateTime();
					++state;
				}
				break;
			case 3:
				RTCManager::GetInstance().Unregister();
				++state;
				break;				
		}
		
		if(DisplayLCD::GetInstance().GetAvailableTextLength() > 25)
		{		
			char date[8];
			char time[8];
			char day[2];
			
			date[0] = dateTime.m_day / 10 + 48;
			date[1] = dateTime.m_day % 10 + 48;			
			date[2] = '.';
			
			date[3] = dateTime.m_month / 10 + 48;
			date[4] = dateTime.m_month % 10 + 48;
			date[5] = '.';
			
			date[6] = dateTime.m_year / 10 + 48;
			date[7] = dateTime.m_year % 10 + 48;
			
			time[0] = dateTime.m_hour / 10 + 48;
			time[1] = dateTime.m_hour % 10 + 48;
			time[2] = ':';
			
			time[3] = dateTime.m_minute / 10 + 48;
			time[4] = dateTime.m_minute % 10 + 48;
			time[5] = ':';
			
			time[6] = dateTime.m_second / 10 + 48;
			time[7] = dateTime.m_second % 10 + 48;
			
			switch(dateTime.m_dayOfWeek)
			{
				case 1:
					day[0] = 'N';
					day[1] = 'D';
					break;
				case 2:
					day[0] = 'P';
					day[1] = 'N';
					break;
				case 3:
					day[0] = 'W';
					day[1] = 'T';
					break;
				case 4:
					day[0] = 'S';
					day[1] = 'R';
					break;
				case 5:
					day[0] = 'C';
					day[1] = 'Z';
					break;
				case 6:
					day[0] = 'P';
					day[1] = 'T';
					break;
				case 7:
					day[0] = 'S';
					day[1] = 'O';
					break;
			}
			
			char s[1];
			s[0] = state + 48;
			DisplayLCD::GetInstance().WriteText(date, FIRST_LINE_ADDRESS, 8);
			DisplayLCD::GetInstance().WriteText(time, SECOND_LINE_ADDRESS, 8);
			DisplayLCD::GetInstance().WriteText(day, FIRST_LINE_ADDRESS + 10, 2);
			DisplayLCD::GetInstance().WriteText(s, SECOND_LINE_ADDRESS + 10, 1);
			
			/*
			if(DebugTools::GetInstance().m_bError)
				DisplayLCD::GetInstance().WriteText("ERR", SECOND_LINE_ADDRESS + 5, 3);
			else
				DisplayLCD::GetInstance().WriteText("OK ", SECOND_LINE_ADDRESS + 5, 3);
			*/	
		}
		
		if(state > 3)
		{
			long long currentTime = Time::GetInstance().GetUS();
			if(currentTime - lastRead > 1000000)
			{
				state = 0;		
				lastRead = currentTime;
			}
		}
		
		DisplayLCD::GetInstance().Process();
		RTCManager::GetInstance().Process();
    }
}

