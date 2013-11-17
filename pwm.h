/** \file
PWM driver header.*/
#ifndef __PWM_H__
#define __PWM_H__

#include <avr/io.h>
#include "macros.h"

#if defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)	/* no OCF1C */ || \
    defined(__AVR_AT90USB162__) || defined(__AVR_ATmega8U2__) || defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega32U2__) || \
    defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
/*			bits	modes									trigger sources						ICPins	prescaler
	timer0:	8		norm, ctc, fast, phase-cor				TOV0, OCF0A, OCF0B					0		stop, 1, 8, 64, 256, 1024, extf, extr
	timer1:	16		norm, ctc, fast, phase-cor, freq-cor	TOV1, OCF1A, OCF1B, OCF1C, ICF1		1		stop, 1, 8, 64, 256, 1024, extf, extr
	timer3 on U4 series is about equal to timer0 (but can operate asynchronously from an external clock)
*/
	#define TIMER0WIDTH 8
	#define TIMER1WIDTH 16
	#define TIMER3WIDTH 8

#else
	#error "Unsupported Target!\n"
#endif

#if defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)
	#define OC0A_DDR   DDRD
	#define OC0A_PORT  PORTD
	#define OC0A_PIN   PIND
	#define OC0A_BIT   6

	#define OC0B_DDR   DDRD
	#define OC0B_PORT  PORTD
	#define OC0B_PIN   PIND
	#define OC0B_BIT   5

	#define OC1A_DDR   DDRB
	#define OC1A_PORT  PORTB
	#define OC1A_PIN   PINB
	#define OC1A_BIT   1

	#define OC1B_DDR   DDRB
	#define OC1B_PORT  PORTB
	#define OC1B_PIN   PINB
	#define OC1B_BIT   2

	#define OC2A_DDR   DDRB
	#define OC2A_PORT  PORTB
	#define OC2A_PIN   PINB
	#define OC2A_BIT   3

	#define OC2B_DDR   DDRD
	#define OC2B_PORT  PORTD
	#define OC2B_PIN   PIND
	#define OC2B_BIT   3
#elif defined(__AVR_AT90USB162__) || defined(__AVR_ATmega8U2__) || defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega32U2__)
	#define OC0A_DDR   DDRB
	#define OC0A_PORT  PORTB
	#define OC0A_PIN   PINB
	#define OC0A_BIT   7

	#define OC0B_DDR   DDRD
	#define OC0B_PORT  PORTD
	#define OC0B_PIN   PIND
	#define OC0B_BIT   0

	#define OC1A_DDR   DDRC
	#define OC1A_PORT  PORTC
	#define OC1A_PIN   PINC
	#define OC1A_BIT   6

	#define OC1B_DDR   DDRC
	#define OC1B_PORT  PORTC
	#define OC1B_PIN   PINC
	#define OC1B_BIT   5

	#define OC1C_DDR   OC0A_DDR
	#define OC1C_PORT  OC0A_PORT
	#define OC1C_PIN   OC0A_PIN
	#define OC1C_BIT   OC0A_BIT
#elif defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
	#define OC0A_DDR   DDRB
	#define OC0A_PORT  PORTB
	#define OC0A_PIN   PINB
	#define OC0A_BIT   7

	#define OC0B_DDR   DDRD
	#define OC0B_PORT  PORTD
	#define OC0B_PIN   PIND
	#define OC0B_BIT   0

	#define OC1A_DDR   DDRB
	#define OC1A_PORT  PORTB
	#define OC1A_PIN   PINB
	#define OC1A_BIT   5

	#define OC1B_DDR   DDRB
	#define OC1B_PORT  PORTB
	#define OC1B_PIN   PINB
	#define OC1B_BIT   6

	#define OC1C_DDR   OC0A_DDR
	#define OC1C_PORT  OC0A_PORT
	#define OC1C_PIN   OC0A_PIN
	#define OC1C_BIT   OC0A_BIT

	#define OC3A_DDR   DDRC
	#define OC3A_PORT  PORTC
	#define OC3A_PIN   PINC
	#define OC3A_BIT   6
#else
	
#endif

#define _IS_TIMER_COMPLEX(WIDTH)	(WIDTH > 8)
#define IS_TIMER_COMPLEX(TIMERNUM)	_IS_TIMER_COMPLEX(TIMER ## TIMERNUM ## WIDTH)

// Valid for all timers but the high speed timer4 in the U4
/** \name Prescaler macros for the timers
\anchor prescalers
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
enum PRESCALER {
	PRESC_OFF	= __B(000),
	PRESC_1		= __B(001),
	PRESC_8		= __B(010),
	PRESC_64	= __B(011),
	PRESC_256	= __B(100),
	PRESC_1024	= __B(101),
	PRESC_EXTF	= __B(110),
	PRESC_EXTR	= __B(111),
};
//@}
/** Configures the prescaler of timer NUM to be PRESCNUM.
\code
\c PRESC has to be one of: \c PRESC_OFF, \c PRESC_1, \c PRESC_8, \c PRESC_64, \c PRESC_256, \c PRESC_1024,\n
\c PRESC_EXTF (external clock; falling edge), \c PRESC_EXTR (external clock; rising edge)
\endcode */
#define SET_TPRESC(NUM, PRESC) TCCR ## NUM ## B |= (0x7 & PRESC); TCCR ## NUM ## B &= (~0x7 | PRESC)

/** PWM modes of operation. */
//@{
enum PWM_SIMPLE_OP_MODE {
	PWM_SIMPLE_OP_NORMAL	= 0,
	PWM_SIMPLE_OP_PHASE_MAX	= 1,
	PWM_SIMPLE_OP_CTC		= 2,
	PWM_SIMPLE_OP_FAST_MAX	= 3,
	PWM_SIMPLE_OP_PHASE_OCR	= 5,
	PWM_SIMPLE_OP_FAST_OCR	= 7,
};

enum PWM_COMPLEX_OP_MODE {
	PWM_COMPLEX_OP_NORMAL			= 0,
	PWM_COMPLEX_OP_PHASE_MAX8		= 1,
	PWM_COMPLEX_OP_PHASE_MAX9		= 2,
	PWM_COMPLEX_OP_PHASE_MAX10		= 3,
	PWM_COMPLEX_OP_CTC_OCR			= 4,
	PWM_COMPLEX_OP_FAST_MAX8		= 5,
	PWM_COMPLEX_OP_FAST_MAX9		= 6,
	PWM_COMPLEX_OP_FAST_MAX10		= 7,
	PWM_COMPLEX_OP_PHASEFREQ_ICR	= 8,
	PWM_COMPLEX_OP_PHASEFREQ_OCR	= 9,
	PWM_COMPLEX_OP_PHASE_ICR		= 10,
	PWM_COMPLEX_OP_PHASE_OCR		= 11,
	PWM_COMPLEX_OP_CTC_ICR			= 12,
	PWM_COMPLEX_OP_FAST_ICR			= 14,
	PWM_COMPLEX_OP_FAST_OCR			= 15,
};
//@}

#define _PWM_SET_MODE_16(TIMERNUM, MODE) \
		CFG_REG(TCCR ## TIMERNUM ## B, MODE << (WGM ## TIMERNUM ## 3 - 3), _BV(WGM ## TIMERNUM ## 3));

#define _PWM_SET_MODE_8(TIMERNUM, MODE) /* No WGMn3 on simple timers */

#define __PWM_SET_MODE(WIDTH, TIMERNUM, MODE) \
	_PWM_SET_MODE_ ## WIDTH(TIMERNUM, MODE)

#define _PWM_SET_MODE(WIDTH, TIMERNUM, MODE) \
	__PWM_SET_MODE(WIDTH, TIMERNUM, MODE)

#define PWM_SET_MODE(TIMERNUM, MODE) \
	_PWM_SET_MODE(TIMER ## TIMERNUM ## WIDTH, TIMERNUM, MODE);\
	CFG_REG(TCCR ## TIMERNUM ## B, MODE << (WGM ## TIMERNUM ## 2 - 2), _BV(WGM ## TIMERNUM ## 2));\
	CFG_REG(TCCR ## TIMERNUM ## A, MODE << (WGM ## TIMERNUM ## 1 - 1), _BV(WGM ## TIMERNUM ## 1));\
	CFG_REG(TCCR ## TIMERNUM ## A, MODE << (WGM ## TIMERNUM ## 0 - 0), _BV(WGM ## TIMERNUM ## 0));\

/**  */
enum PWM_COMP_MODE {
	PWM_NORMAL	= 0,
	PWM_TOGGLE	= 1,
	PWM_CLEAR	= 2,
	PWM_SET		= 3
};

/** Initializes a PWM channel/pin.
Sets the pin associated to the given \c TIMERNUM  and \c CHANNEL and
configures its compare mode respectively.
*/
#define PWM_INIT_CHANNEL(TIMERNUM, CHANNEL, COMP_MODE) \
	OC ## TIMERNUM ## CHANNEL ## _DDR |= _BV(OC ## TIMERNUM ## CHANNEL ## _BIT); \
	CFG_REG(TCCR ## TIMERNUM ## A, COMP_MODE << COM ## TIMERNUM ## CHANNEL ## 0, 0x3 << COM ## TIMERNUM ## CHANNEL ## 0)

#define _SET_ALLCOM_OFF8(TIMERNUM) \
	TCCR ## TIMERNUM ## A &= ~(_BV(COM ## TIMERNUM ## A1) | _BV(COM ## TIMERNUM ## A0) | \
							   _BV(COM ## TIMERNUM ## B1) | _BV(COM ## TIMERNUM ## B0))

#define _SET_ALLCOM_OFF16(TIMERNUM) \
	TCCR ## TIMERNUM ## A &= ~(_BV(COM ## TIMERNUM ## A1) | _BV(COM ## TIMERNUM ## A0) | \
							   _BV(COM ## TIMERNUM ## B1) | _BV(COM ## TIMERNUM ## B0) | \
							   _BV(COM ## TIMERNUM ## C1) | _BV(COM ## TIMERNUM ## C0))

#define __SET_ALLCOM_OFF(WIDTH, TIMERNUM) \
	_SET_ALLCOM_OFF ## WIDTH(TIMERNUM)

#define _SET_ALLCOM_OFF(WIDTH, TIMERNUM) \
	__SET_ALLCOM_OFF(WIDTH, TIMERNUM)

#define SET_ALLCOM_OFF(TIMERNUM) \
	_SET_ALLCOM_OFF(TIMER ## TIMERNUM ## WIDTH, TIMERNUM);
//@}

/** Initializes the PWM unit.
Sets the prescaler bits \c CSn of timer \c TIMERNUM according to \c PRESCALER.
Disables all interrupts related to timer \c TIMERNUM.
Sets the #PWM_PIN to output and sets CTC mode for timer1.
@see prescalers
*/
#define PWM_INIT(TIMERNUM, PRESCALER, CHANNEL, OP_MODE) \
	TIMSK ## TIMERNUM = 0; \
	SET_TPRESC(TIMERNUM, PRESCALER); \
	PWM_SET_MODE(TIMERNUM, OP_MODE);

#endif // __PWM_H__
