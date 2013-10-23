/**\file
timer drivers header.*/
#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <stdint.h>
#include <avr/interrupt.h>

#if !defined DOXYGEN && defined AVRLIB_TIMERPREFS
		#include "timer_prefs.h"
#else
/** \name example timer config values for 1ms resolution up to >15min intervals
@{*/
/* timer0 */
#define PRESC0_SHORT    PRESC_64
#define PRESC0_LONG     PRESC_1024
#define OCR0_SHORT      250
#define OCR0_LONG       250
#define DUR0_LONG       16
#define DUR0_LONG_WIDTH 16
#define DUR0_PARAM_WIDTH 16
/* for documentation only:
created by './timers.py -r 0.001 -f 16000000 -tw 8 -lw 16 -t 0' */
#define DUR0_MAX        1048.8150000000f /* [s] */
#define F_TIMER0        16000000 /* [Hz] */
#define RES0            0.001f /* [s] */

/* timer1 */
#define PRESC1_SHORT    PRESC_64
#define PRESC1_LONG     PRESC_1024
#define OCR1_SHORT      250
#define OCR1_LONG       250
#define DUR1_LONG       16
#define DUR1_LONG_WIDTH 16
#define DUR1_PARAM_WIDTH 16
/* for documentation only:
created by './timers.py -r 0.001 -f 16000000 -tw 8 -lw 16 -t 1 -v' */
#define DUR1_MAX        1048.8150000000f /* [s] */
#define F_TIMER1        16000000 /* [Hz] */
#define RES1            0.001f /* [s] */
//@}
#endif

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
	TCCR0B |=  _BV(WGM02);
#define T0_WGM_PWM_FAST() \
	TCCR0A |=  _BV(WGM01);\
	TCCR0A |=  _BV(WGM02);\
	TCCR0B |=  _BV(WGM02)
//@}

/** Timer 1 PWM modes.
TOP is always defined by OCRA.*/
//@{
#define T1_WGM_CTC() \
	TCCR1A &= ~_BV(WGM10);\
	TCCR1A &= ~_BV(WGM11);\
	TCCR1B |=  _BV(WGM12);\
	TCCR1B &= ~_BV(WGM13)
#define T1_WGM_PWM_PHASE() \
	TCCR1A |=  _BV(WGM10);\
	TCCR1A |=  _BV(WGM11);\
	TCCR1B &= ~_BV(WGM12);\
	TCCR1B |=  _BV(WGM13);
#define T1_WGM_PWM_FAST() \
	TCCR1A |=  _BV(WGM11);\
	TCCR1A |=  _BV(WGM12);\
	TCCR1B |=  _BV(WGM12);\
	TCCR1B |=  _BV(WGM13)
//@}

#define _PWMA_PINS0 \
_BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0)
#define _PWMA_PINS1 \
_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0) | _BV(COM1C1) | _BV(COM1C0)

#define PWMA_OFF(TIMER) (TCCR ## TIMER ## A &= ~(_PWMA_PINS ## TIMER))

/** \name Prescaler macros for the timers
\code
Macro        CSn2   CSn1   CSn0    Description
PRESC_OFF      0      0      0     No clock source. (Timer/Counter stopped)
PRESC_1        0      0      1     clk_io/1 (No prescaling
PRESC_8        0      1      0     clk_io/8 (From prescaler)
PRESC_64       0      1      1     clk_io/64 (From prescaler)
PRESC_256      1      0      0     clk_io/256 (From prescaler)
PRESC_1024     1      0      1     clk_io/1024 (From prescaler)
PRESC_EXTF     1      1      0     External clock source on Tn pin. Clock on falling edge
PRESC_EXTR     1      1      1     External clock source on Tn pin. Clock on rising edge
\endcode*/
//@{
#define PRESC_OFF	0x0
#define PRESC_1		0x1
#define PRESC_8		0x2
#define PRESC_64	0x3
#define PRESC_256	0x4
#define PRESC_1024	0x5
#define PRESC_EXTF	0x6
#define PRESC_EXTR	0x7
//@}

/** Configuration of timer 1 with prescaler PRESCNUM.
\code
\c PRESC has to be one of: \c PRESC_OFF, \c PRESC_1, \c PRESC_8, \c PRESC_64, \c PRESC_256, \c PRESC_1024,\n
\c PRESC_EXTF (external clock; falling edge), \c PRESC_EXTR (external clock; rising edge)
\endcode */
#define SET_TPRESC(NUM, PRESC) TCCR ## NUM ## B |= (0x7 & PRESC); TCCR ## NUM ## B &= (~0x7 | PRESC)

#define __BITS_TYPE(W) uint ## W ## _t
#define _BITS_TYPE(W) __BITS_TYPE(W)
#define CNT_LONG_TYPE(TIMERNUM) _BITS_TYPE(DUR ## TIMERNUM ## _LONG_WIDTH)
#define DUR_PARAM_TYPE(TIMERNUM) _BITS_TYPE(DUR ## TIMERNUM ## _PARAM_WIDTH)

/** defines the 3 modes timer can be configured to.
	- TIMER_ONESHOT Timer stops after the first Period
	- TIMER_PERIODIC Timer runs periodically
	- TIMER_STOP Timer stops immediately (needs to be last in the enum!)
 */
enum timermode { TIMER_ONESHOT, TIMER_PERIODIC, TIMER_STOP };

#define RESET_TIMER(TIMERNUM) TCNT ## TIMERNUM = 0;
#define TIMER_DECLS(TIMERNUM) \
int configure_timer ## TIMERNUM(enum timermode mode, DUR_PARAM_TYPE(TIMERNUM) duration, void (*cb)(void));

#ifdef PRESC0_SHORT
TIMER_DECLS(0)
#endif
#ifdef PRESC1_SHORT
TIMER_DECLS(1)
#endif

#endif
