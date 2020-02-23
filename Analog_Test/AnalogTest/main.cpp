/*
 * AnalogTest.cpp
 *
 * Created: 2020-01-19 14:31:23
 * Author : Gabriel
 */ 

#include <avr/io.h>
#include "../../_Libraries/LCD_Display/LCD_Display.h"
#include "../../_Libraries/DebugTools/DebugTools.h"

int ADCsingleREAD(unsigned char adctouse)
{
	int ADCval;

	ADMUX = adctouse;         // use #1 ADC
	ADMUX |= (1 << REFS0);    // use AVcc as the reference
	ADMUX &= ~(1 << ADLAR);   // clear for 10 bit resolution
	
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // 128 prescale for 8Mhz
	ADCSRA |= (1 << ADEN);    // Enable the ADC

	ADCSRA |= (1 << ADSC);    // Start the ADC conversion

	while(ADCSRA & (1 << ADSC));      // Thanks T, this line waits for the ADC to finish


	ADCval = ADCL;
	ADCval = (ADCH << 8) + ADCval;    // ADCH is read so ADC can be updated again

	return ADCval;
}

//int ADCvalue;

volatile uint8_t ADCvalue;    // Global variable, set to volatile if used with ISR

int main(void)
{	
	DisplayLCD::GetInstance().Initialize();
	
	//DisplayLCD::GetInstance().WriteText("HELLO WORLD", SECOND_LINE_ADDRESS + 5, 11);
	
	//led pin as output
	//DDRC |= (1 << 5);
	//PORTC &= ~(1 << 5);
	
	
	DDRC = 0;//Taking portC as input.		
	
	//DisplayLCD::GetInstance().Test2();
	unsigned char idx = 0;
    /* Replace with your application code */
    while (1) 
    {				
		//4294967296
		/*
		char buf[10];
		if(DisplayLCD::GetInstance().GetAvailableTextLength() > 10)
		{
			unsigned long currentTime = Time::GetInstance().GetUS();
			for(int i = 0; i < 10; i++)
			{				
				buf[9 - i] = (currentTime % 10) + 48;
				currentTime /= 10;
			}
			DisplayLCD::GetInstance().WriteText(buf, FIRST_LINE_ADDRESS, 10);
		}*/
				
		char buf[5];
		if(DisplayLCD::GetInstance().GetAvailableTextLength() > 6)
		{
			int read = ADCsingleREAD(idx);
			//int read = ADCvalue;
			for(int i = 0; i < 5; i++)
			{
				buf[4 - i] = (read % 10) + 48;
				read /= 10;
			}
			
			if(idx < 2)
				DisplayLCD::GetInstance().WriteText(buf, FIRST_LINE_ADDRESS + (10 * (idx % 2)), 5);
			else
				DisplayLCD::GetInstance().WriteText(buf, SECOND_LINE_ADDRESS + (10 * (idx % 2)), 5);
				
			if(++idx > 3)
				idx = 0;
		}
		
		DisplayLCD::GetInstance().Process();
    }
}