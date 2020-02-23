/*
 * BlinkTest.cpp
 *
 * Created: 2019-11-01 11:17:09
 * Author : Gabriel
 */ 

#define F_CPU 8000000UL //8 MHz

#include <avr/io.h>
#include "../../_Libraries/Time/Time.h"
#include "../../_Libraries/Manchester/Manchester.h"

unsigned char message = 0;


unsigned long displayTime = 0;
unsigned long currentTime = 0;
bool state;

int main(void)
{
	Time::GetInstance().Configure();
	Manchester::GetInstance().ConfigureTx();
			
    /* Replace with your application code */
    while (1) 
    {		
		if(Manchester::GetInstance().SendMessage(1, message))
			message++;
		
		Manchester::GetInstance().ProcessTx();
		
		
		/*
		currentTime = Time::GetInstance().GetUS();
				
		if(currentTime >= displayTime + 1000000) //refresh display every 2ms
		{
			displayTime = currentTime;
			if(state)
			{
				TX_PORT |= (1 << TX_PIN);
				state = false;
			}
			else
			{
				TX_PORT &= !(1 << TX_PIN);
				state = true;
			}			
		}
		*/		
    }
}


