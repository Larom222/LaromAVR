/*
 * TemperatureRegulator.cpp
 *
 * Created: 2020-02-21 17:58:35
 * Author : Gabriel
 */ 

#include <avr/io.h>

#include "../../_Libraries/LCD_Display/LCD_Display.h"
#include "../../_Libraries/EEPROM_Manager/EEPROM_Manager.h"
#include "../../_Libraries/RTC_Manager/RTC_Manager.h"
#include "../../_Libraries/DebugTools/DebugTools.h"
#include "../../_Libraries/Sound_Manager/SoundManager.h"
#include "TemperatureController.h"
#include "Thermistor.h"
#include "Displays/MainDisplay.h"
#include "KeyboardManager.h"
	
int main(void)
{
	DDRB |= (1 << PB0); //PB0 as output
	
	Time::GetInstance().Configure();
	DisplayLCD::GetInstance().Initialize();
	RTCManager::GetInstance().Configure();
	EEPROMManager::GetInstance().Configure();
	Thermistor::GetInstance().Configure();
	KeyboardManager::GetInstance().Configure();
	SoundManager::GetInstance().Configure();

	MainDisplay mainDisplay;
	
	/*	
	EEPROMManager::GetInstance().Register();
	EEPROMManager::GetInstance().WriteByte(0, 3, 26);
	*/
	
    while (1) 
    {			
		DisplayLCD::GetInstance().Process();
		RTCManager::GetInstance().Process();		
		EEPROMManager::GetInstance().Process();
		TemperatureController::GetInstance().Process();
		mainDisplay.Process();
		KeyboardManager::GetInstance().Process();
		SoundManager::GetInstance().Process();
		
		/*if(DebugTools::GetInstance().m_bProcessing)
		{
			SoundManager::GetInstance().Play();
			//DebugTools::GetInstance().m_bError = false;	
		}*/
    }
}
