/* 
* Thermistor.cpp
*
* Created: 2020-02-21 18:26:40
* Author: Gabriel
*/


#include "Thermistor.h"
#include <math.h>

// default constructor
Thermistor::Thermistor()
{	
	m_bConfigured = false;
} //Thermistor

// default destructor
Thermistor::~Thermistor()
{
} //~Thermistor

void Thermistor::Configure()
{	
	DDRC &= ~(1 << ADC_CHANNEL);
	
	ADMUX = ADC_CHANNEL;
	ADMUX |= (1 << REFS0);    // use AVcc as the reference
	ADMUX &= ~(1 << ADLAR);   // clear for 10 bit resolution
	
	ADCSRA = 0x87;		/* Enable ADC, fr/128  */
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // 128 prescale for 8Mhz
	ADCSRA |= (1 << ADEN);    // Enable the ADC
	m_bConfigured = true;
}

bool Thermistor::ReadTemperature()
{
	if(!m_bConfigured)
		return false;

	ADCSRA |= (1 << ADSC);    // Start the ADC conversion
	return true;
}

bool Thermistor::ReadyToRead()
{
	if(!m_bConfigured)
		return false;
	return !(ADCSRA & (1 << ADSC));
}

unsigned short Thermistor::GetLastTemperature()
{
	int ADCval;	
	
	ADCval = ADCL;
	ADCval = (ADCH << 8) + ADCval;    // ADCH is read so ADC can be updated again
	
	long R = ((10230000 / ADCval) - 10000);/* calculate the resistance */
	double Thermister = log(R);	/* calculate natural log of resistance */
	/* Steinhart-Hart Thermistor Equation: */
	/*  Temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)		*/
	/*  where A = 0.001129148, B = 0.000234125 and C = 8.76741*10^-8  */
	Thermister = 1 / (0.001129148 + (0.000234125 * Thermister) + (0.0000000876741 * Thermister * Thermister * Thermister));
	Thermister = Thermister - 273.15;/* convert kelvin to °C */
	
	return Thermister * 100.0;
}