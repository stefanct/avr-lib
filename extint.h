/**\file
Macros to easily utilize external interrupts.*/
#ifndef __EXTINT_H__
#define __EXTINT_H__

#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include "macros.h"

#define EXTINT_TRIGGER_LOW		0
#define EXTINT_TRIGGER_EDGE		1
#define EXTINT_TRIGGER_FALLING	2
#define EXTINT_TRIGGER_RISING	3

#define EXTINT_ENABLE(PIN, EXTINT_TRIGGER) \
	CLB(EIMSK, PIN);\
	if(PIN<4)\
		EICRA |= EXTINT_TRIGGER<<(2*PIN);\
	else\
		EICRB |= EXTINT_TRIGGER<<(2*PIN - 8);\
	SEB(EIFR, PIN); /* clear spurious interrupt*/\
	SEB(EIMSK, PIN);

#define EXTINT_DISABLE(PIN) \
	CLB(EIMSK, PIN);

#endif // __EXTINT_H__
