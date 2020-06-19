
#ifndef __TIME_PIN_CONFIG_H__
#define __TIME_PIN_CONFIG_H__

//**************************************************
//			Register select
//**************************************************
#define COMPARATOR 0
#define INTERRUPT 1

#if defined(PROJ_TEMPERATURE_REGULATOR) || defined(PROJ_HC_06_TEST) || defined(PROJ_LIGHT_CONTROLLER)

#define TIMER0_MODE COMPARATOR

#else

#define TIMER0_MODE INTERRUPT
	
#endif

#endif //__TIME_PIN_CONFIG_H__