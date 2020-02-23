
#ifndef __PIN_CONFIG_H__
#define __PIN_CONFIG_H__

//**************************************************
//			Register select pins
//**************************************************
#if defined(PROJ_LCD_DISPLAY_TEST) || defined(PROJ_EEPROM_TEST) || defined(PROJ_RTC_TEST) || defined(PROJ_ANALOG_TEST) || defined(PROJ_TEMPERATURE_REGULATOR)

	#define F_CPU 8000000UL //8 MHz
	#define WRITE_BUFFER_SIZE 72 //2 lines(2*32) + 2 commands(2*2)
	
	#define RS_PORT PORTD
	#define RS_DDR DDRD
	#define RS_PIN 5
	
	#define RW_PORT PORTD
	#define RW_DDR DDRD
	#define RW_PIN 6
	
	#define E_PORT PORTD
	#define E_DDR DDRD
	#define E_PIN 7
	
//**************************************************
//				Data pins
//**************************************************
	
	#define D0_PORT PORTC
	#define D0_DDR DDRC
	#define D0_PIN 3
	
	#define D1_PORT PORTC
	#define D1_DDR DDRC
	#define D1_PIN 2
	
	#define D2_PORT PORTC
	#define D2_DDR DDRC
	#define D2_PIN 1
	
	#define D3_PORT PORTC
	#define D3_DDR DDRC
	#define D3_PIN 0
	
	#define D4_PORT PORTD
	#define D4_DDR DDRD
	#define D4_PIN 0
	
	#define D5_PORT PORTD
	#define D5_DDR DDRD
	#define D5_PIN 1
	
	#define D6_PORT PORTD
	#define D6_DDR DDRD
	#define D6_PIN 2
	
	#define D7_PORT PORTD
	#define D7_DDR DDRD
	#define D7_PIN 3
	
#endif

#endif //__PIN_CONFIG_H__