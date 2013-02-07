/**\file
timer0 driver header.*/
#ifndef __TIMER0H_H__
#define __TIMER0H_H__

#include <stdint.h>
#include <avr/interrupt.h>

#if defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)	/*********************/
#elif defined(__AVR_AT90USB162__) || defined(__AVR_ATmega8U2__) || defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega32U2__) /*********************/
#else /*********************/
	#error "Wrong Target!\n"
#endif

/** Timer 0 PWM modes.
TOP is always defined by OCRA.*/
//@{
#define T0_WGM_CTC() \
	TCCR0A &= ~_BV(WGM00);\
	TCCR0A |=  _BV(WGM01);\
	TCCR0B &= ~_BV(WGM02)
#define T0_WGM_PWM_PHASE() \
	TCCR0A |=  _BV(WGM00);\
	TCCR0A &= ~_BV(WGM01);\
	TCCR0B &= ~_BV(WGM02);
#define T0_WGM_PWM_FAST() \
	TCCR0A |=  _BV(WGM01);\
	TCCR0A |=  _BV(WGM02);\
	TCCR0B |=  _BV(WGM02)
#define T0_PWMA_OFF() (TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0)))
//@}

/** \name Prescaler macros for timer 0
\code
Macro        CSn2   CSn1   CSn0    Description
PRESC0_OFF      0      0      0     No clock source. (Timer/Counter stopped)
PRESC0_1        0      0      1     clk_io/1 (No prescaling
PRESC0_8        0      1      0     clk_io/8 (From prescaler)
PRESC0_64       0      1      1     clk_io/64 (From prescaler)
PRESC0_256      1      0      0     clk_io/256 (From prescaler)
PRESC0_1024     1      0      1     clk_io/1024 (From prescaler)
PRESC0_EXTF     1      1      0     External clock source on Tn pin. Clock on falling edge
PRESC0_EXTR     1      1      1     External clock source on Tn pin. Clock on rising edge
\endcode*/
//@{
#define PRESC0_OFF	0x0
#define PRESC0_1	0x1
#define PRESC0_8	0x2
#define PRESC0_64	0x3
#define PRESC0_256	0x4
#define PRESC0_1024	0x5
#define PRESC0_EXTF	0x6
#define PRESC0_EXTR	0x7
//@}

/** Configuration of timer 0 with prescaler PRESCNUM.
\code
\c PRESC has to be one of: \c PRESC0_OFF, \c PRESC0_1, \c PRESC0_8, \c PRESC0_64, \c PRESC0_256, \c PRESC0_1024,\n
\c PRESC0_EXTF (external clock; falling edge), \c PRESC0_EXTR (external clock; rising edge)
\endcode */
#define T0_PRESC(PRESC) TCCR0B |= (0x7 & PRESC);TCCR0B &= (~0x7 | PRESC)

/** defines the 3 modes timer 0 can be configured to.
	- TIMER_ONESHOT Timer stops after the first Period
	- TIMER_PERIODIC Timer runs periodically
	- TIMER_STOP Timer stops immediately (needs to be last in the enum!)
 */
enum timer0mode { TIMER_ONESHOT, TIMER_PERIODIC, TIMER_STOP };

/** \name timer config values for 1ms resolution
@{*/
#define	PRESC0_SHORT	PRESC0_64
#define	PRESC0_LONG	PRESC0_1024
#define	OCR0_SHORT	250
#define	OCR0_LONG	250
#define	DUR0_LONG	16
#define	DUR0_MAX	65.535f
#define	RES0	0.001f
//@}

void configure_timer0(uint8_t mode, uint16_t duration, void (*cb)(void));

#endif
