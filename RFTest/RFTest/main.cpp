/*
 * rf_sender.c
 *
 * Created: 20.02.2016 11:46:21
 * Author : Christian Wagner
 */

#define F_CPU 1000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//#define FOSC F_CPU
#define FOSC 1100000
#define BAUD 9600UL
#define MYUBRR ((FOSC/(16*BAUD))-1)


void ConfigBaudRate()
{	
	/* Set baud rate */
	UBRRH = (unsigned char)(MYUBRR>>8);
	UBRRL = (unsigned char)MYUBRR;
	
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN);
		
	/* Set frame format: 1stop bit */
	UCSRC = (1<<URSEL) | (0<<USBS) | (1<<UCSZ0) | (1<<UCSZ1) | (0<<UCSZ2) | (0<<UPM0);
	
	//UCSRC |= (1<<URSEL) | (0<<USBS);
	
	//8data
	//UCSRC |= (1<<URSEL) | (1<<UCSZ0);
	//UCSRC |= (1<<URSEL) | (1<<UCSZ1);
	//UCSRC |= (1<<URSEL) | (0<<UCSZ2);
	
	// ustawienie bitów parzystoœci - no parity
	//UCSRC |= (1<<URSEL) | 0<<UPM0;
	
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) )
		;
	
	/* Put data into buffer, sends the data */
	UDR = data;
	
}

int main(void)
{
	//ConfigBaudRate();
	
	//USART_Transmit(10);
	//_delay_ms(1000);
	
	DDRD |= (1<<1);
	
	while(1)
	{
		PORTD |= (1<<1);
		_delay_ms(5);
		PORTD &= ~(1<<1);
		_delay_ms(10);
	}
}
