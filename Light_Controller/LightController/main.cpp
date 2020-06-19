/*
 * LightController.cpp
 *
 * Created: 2020-05-29 20:40:12
 * Author : Gabriel
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//#include "../../_Libraries/LCD_Display/LCD_Display.h"

enum E_RX_STATE
{
	RXS_STX = 0,
	RXS_ETX = 1,
};

unsigned char RxBuf[6];
unsigned char ReadBuf[4];
unsigned char RxBufPos = 0;
unsigned char RxState = RXS_STX;
unsigned char PWM = 0;
bool readyToRead = false;

unsigned char lightR = 0;
unsigned char lightG = 0;
unsigned char lightB = 0;

unsigned long long ms = 0;

bool haloState = false;
bool light1State = false;
bool light2State = false;
bool light3State = false;

#define ATMEGA8
void ConfigUART()
{
	cli();
	
#ifdef ATMEGA328P
	UBRR0H=0x00;
	UBRR0L=51;
	
	
	//---------------------------
	UCSR0B=0x00;
	UCSR0B|=0x80 | 0x10 | 0x08;     //Rx przerwania on, rx enable, tx enalbe
	//---------------------------
	UCSR0C=0x00;                    // tryb asynchroniczny, bez przystosci, 1 bit stopu
	UCSR0C|=0x04 | 0x02;            // 8 date bits
#else
#define BAUD_PRESCALE (((8000000 / (9600 * 16UL))) - 1) 
	UBRRH= BAUD_PRESCALE >> 8;
	UBRRL= BAUD_PRESCALE;

//UCSRA bez zmian - 0x00
	UCSRA = 0x00;
	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);                 //aktywne przerwanie od odbioru oraz zmiana trybu dzia³ania pinów D0 i D1
	UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);     //praca synchroniczna, brak kontroli parzystoœci, 1 bit stopu, 8 bitów danych

/*
	UCSRA=0x00;
	//---------------------------
	UCSRB=0x00;
	UCSRB|=0x80 | 0x10 | 0x08;     //Rx przerwania on, rx enable, tx enalbe
	//---------------------------
	UCSRC=0x00;                    // tryb asynchroniczny, bez przystosci, 1 bit stopu
	UCSRC|=0x04 | 0x02;            // 8 date bits*/
#endif
	
	sei();
}

void ConfigurePWM()
{
	cli();
	
	//TCCR1A |= (1 << WGM11);	// Set the Timer Mode to CTC
	//TIMSK1 |= (1 << OCIE1A);
	//TCCR1B |= (1 << CS10); // timer1 prescaler = 8
	//OCR1A = 0x90; //interrupt every 50us
	
	
	
	OCR1A = 0x0100;

	TCCR1B |= (1 << WGM12);
	// Mode 4, CTC on OCR1A
#ifdef ATMEGA328P
	TIMSK1 |= (1 << OCIE1A);
	//Set interrupt on compare match
#else
	TIMSK |= (1 << OCIE1A);
	//Set interrupt on compare match
#endif

	TCCR1B |= (1 << CS10);
	// set prescaler to 1024 and start the timer


	sei();
	// enable interrupts
	
	
	
	//sei();
};

int main(void)
{
	DDRB |= (1 << PB0); //PB0 as output
	DDRD |= (1 << PD6); //PD6 as output
	DDRD |= (1 << PD7); //PD7 as output
	DDRB |= (1 << PB1); //PB1 as output
	DDRB |= (1 << PB2); //PB2 as output
	DDRB |= (1 << PB3); //PB3 as output
	DDRD |= (1 << PD5); //PB3 as output
	
	PORTD &= ~(1 << PD7);
	PORTD &= ~(1 << PD6);
	PORTB &= ~(1 << PB0);
	PORTD &= ~(1 << PD5);
	
	PORTB &= ~(1 << PB1);
	PORTB &= ~(1 << PB2);
	PORTB &= ~(1 << PB3);
	
	ConfigUART();
	ConfigurePWM();
	
	//DisplayLCD::GetInstance().Initialize();
	
    /* Replace with your application code */
    while (true) 
    {				
		_delay_ms(1);			
		if(readyToRead)
		{
			readyToRead = false;
			unsigned char buf = 0;
			
			if(ReadBuf[0] == 'R' || ReadBuf[0] == 'G' || ReadBuf[0] == 'B')
			{				
				buf += ReadBuf[3] - 48;
				buf += (ReadBuf[2] - 48) * 10;
				buf += (ReadBuf[1] - 48) * 100;
			}
			
			switch(ReadBuf[0])
			{
				case 'R':
					if(buf > 255)
						break;
					lightR = buf;
					break;
				case 'G':
					if(buf > 255)
						break;
					lightG = buf;
					break;
				case 'B':
					if(buf > 255)
						break;
					lightB = buf;
					break;
				case '1':
					if(haloState)
					{
						haloState = false;
						PORTB &= ~(1 << PB0);
					}
					else
					{
						haloState = true;
						PORTB |= (1 << PB0);
					}
					break;
				case '2':
					if(light1State)
					{
						light1State = false;
						PORTD &= ~(1 << PD7);
					}
					else
					{
						light1State = true;
						PORTD |= (1 << PD7);
					}
					break;
				case '3':
					if(light2State)
					{
						light2State = false;
						PORTD &= ~(1 << PD6);
					}
					else
					{
						light2State = true;
						PORTD |= (1 << PD6);
					}
					break;
				case '4':
					if(light3State)
					{
						light3State = false;
						PORTD &= ~(1 << PD5);
					}
					else
					{
						light3State = true;
						PORTD |= (1 << PD5);
					}
					break;
			}
		}
		/*if(DisplayLCD::GetInstance().GetAvailableTextLength() > 6)
		{
			char buf[6];
			buf[5] = ms % 10 + 48;
			buf[4] = (ms / 10) % 10 + 48;
			buf[3] = (ms / 100) % 10 + 48;
			buf[2] = (ms / 1000) % 10 + 48;
			buf[1] = (ms / 10000) % 10 + 48;
			buf[0] = (ms / 100000) % 10 + 48;
			DisplayLCD::GetInstance().WriteText(buf, FIRST_LINE_ADDRESS, 6);
		}
		DisplayLCD::GetInstance().Process();*/
    }
}

ISR (TIMER1_COMPA_vect)
{
	++PWM;
	if(PWM > lightR || lightR == 0)
		PORTB &= ~(1 << PB1);
	else
		PORTB |= (1 << PB1);
		
	if(PWM > lightG || lightG == 0)
		PORTB &= ~(1 << PB2);
	else
		PORTB |= (1 << PB2);
		
	if(PWM > lightB || lightB == 0)
		PORTB &= ~(1 << PB3);
	else
		PORTB |= (1 << PB3);
	++ms;
}

#ifdef ATMEGA328P
ISR(USART_RX_vect)
#else
ISR(USART_RXC_vect)
#endif
{
	unsigned char data;
#ifdef ATMEGA328P
	data = UDR0;
#else
	data = UDR;
#endif
	
	if(RxState == RXS_STX)
	{		
		if(data == 2)
		{			
			RxBuf[0] = data;
			RxBufPos = 1;
			RxState = RXS_ETX;
		}		
	}
	else if(RxState == RXS_ETX)
	{
		RxBuf[RxBufPos] = data;
		RxBufPos++;
		if(data == 3)
		{
			RxState = RXS_STX;
			readyToRead = true;			
			for(int i = 0; i < 4; i++)
			{
				ReadBuf[i] = RxBuf[i + 1];
			}
		}
	}
}