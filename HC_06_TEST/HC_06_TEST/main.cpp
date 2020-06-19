/*
 * HC_06_TEST.cpp
 *
 * Created: 2020-05-29 15:29:44
 * Author : Gabriel
 */ 

#include <avr/io.h>

#include "../../_Libraries/LCD_Display/LCD_Display.h"




bool test = false;
bool sent = true;
unsigned char count = 0;

unsigned char buf[4];
unsigned char pos = 0;

int main(void)
{
	DDRB |= (1 << PB0); //PB0 as outpu
	
	DisplayLCD::GetInstance().Initialize();
	
	//DisplayLCD::GetInstance().WriteText("TEST", FIRST_LINE_ADDRESS, 5);
	
	UBRR0H=0x00;
	UBRR0L=51;
	
	UCSR0A=0x00;
	//---------------------------
	UCSR0B=0x00;
	UCSR0B|=0x80 | 0x10 | 0x08;     //Rx przerwania on, rx enable, tx enalbe
	//---------------------------
	UCSR0C=0x00;                    // tryb asynchroniczny, bez przystosci, 1 bit stopu
	UCSR0C|=0x04 | 0x02;            // 8 date bits

	sei();

	/* Enable receiver and transmitter */
	//UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	
	/* Set frame format: 8data, 1stop bit */
	//UCSR0C = (0<<USBS0)|(3<<UCSZ00)|(0<<UPM00)|(0<<UPM01);
	
	int recv = 0;
	int pos = 0;
	
    /* Replace with your application code */
	
    //DisplayLCD::GetInstance().WriteText("TEST", FIRST_LINE_ADDRESS + pos, 4);
	
    while (1) 
    {
		if(DisplayLCD::GetInstance().GetAvailableTextLength() > 30)
		{
			for(int i = 0; i < 4; i++)
			{
				char text2[4];
				text2[3] = ' ';
				text2[2] = (buf[i] % 10) + 48;
				text2[1] = ((buf[i] % 100) / 10) + 48;
				text2[0] = (buf[i] / 100) + 48;
				DisplayLCD::GetInstance().WriteText(text2, FIRST_LINE_ADDRESS + i * 4, 4);
			}
			
			char text[4];
			text[3] = ' ';
			text[2] = (count % 10) + 48;
			text[1] = ((count % 100) / 10) + 48;
			text[0] = (count / 100) + 48;
			
			DisplayLCD::GetInstance().WriteText(text, SECOND_LINE_ADDRESS + 5, 4);
			
			if(!test)
			DisplayLCD::GetInstance().WriteText("WAIT", SECOND_LINE_ADDRESS, 4);
			else
			DisplayLCD::GetInstance().WriteText("OK  ", SECOND_LINE_ADDRESS, 4);
		}
		
		if(!sent)
		{			
			if(buf[0] == 49)
			{
				PORTB |= (1 << PB0); //PD1 set to 1 (enable relay)
				
				while (!(UCSR0A & (1<<UDRE0)))
					;
				UDR0='S';
				while (!(UCSR0A & (1<<UDRE0)))
					;
				UDR0=49;
			}
			else
			{
				PORTB &= ~(1 << PB0); //PD1 set to 1 (enable relay)
				
				while (!(UCSR0A & (1<<UDRE0)))
				;
				UDR0='S';
				
				while (!(UCSR0A & (1<<UDRE0)))
				;
				UDR0=48;
			}
			
			sent = true;
		}
		
		DisplayLCD::GetInstance().Process();
    }
}

ISR(USART_RX_vect)
{	
	test = true;
	sent = false;
	count++;	
	
	uint8_t recive_char;
	recive_char = UDR0;
	buf[pos++] = recive_char;
	
	if(pos >= 4)
		pos = 0;
		
	while(!(UCSR0A & 0x20));
		//UDR0=recive_char;
}

