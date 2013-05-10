/**\file
timer1 driver.

Can be used to execute functions periodically or after a one-time delay.*/

#include <avr/io.h>
#include <stdlib.h>
#include "timers.h"

#define TIMER_CODE(TIMERNUM) /* code duplication is for wussies! */ \
static enum timermode t ## TIMERNUM ## _mode	= 0;/**< stores the timer mode */ \
\
static CNT_LONG_TYPE(TIMERNUM) t ## TIMERNUM ## _cnt_long = 0;/**< gets decremented every \c DUR ## TIMERNUM ## _LONG s*/ \
static uint8_t  t ## TIMERNUM ## _cnt_short	= 0;/**< gets decremented every \c RES ## TIMERNUM s, if there are no more \c DUR ## TIMERNUM ## _LONG s intervals for this period to do */ \
static CNT_LONG_TYPE(TIMERNUM) t ## TIMERNUM ## _dur_long = 0;/**< stores how many \c DUR ## TIMERNUM ## _LONG s intervals are needed in one period */ \
static uint8_t  t ## TIMERNUM ## _dur_short = 0;/**< stores how many \c RES ## TIMERNUM s intervals are needed (in addition to the \c DUR ## TIMERNUM ## _LONG s intervals) in one period */ \
\
static void (*t ## TIMERNUM ## _callback)(void);/**< function pointer that references the function called every \c dur \c RES ## TIMERNUM s*/ \
\
/** Sets up timer ## TIMERNUM so that \c cb is called after/every \c dur \c RES ## TIMERNUM s. \
Stops the timer if \c intmod is \c TIMER_STOP or \c dur is 0. \
Otherwise divides \c dur into a maximum amount of \c DUR ## TIMERNUM ##_LONG s time slices \
and uses the remainder as counter for short 1 \c RES ## TIMERNUM s time slices. \
\param intmode Timer mode. one of TIMER_PERIODIC, TIMER_ONESHOT or TIMER_STOP as defined by #timer ## TIMERNUM ## mode \
\param dur Duration to/between interrupt(s) \
\param cb functionpointer that references the function to call */ \
int configure_timer ## TIMERNUM (enum timermode intmode, CNT_LONG_TYPE(TIMERNUM) dur, void (*cb)(void)) { \
	SET_TPRESC(TIMERNUM, PRESC_OFF); /* disable timer (at least temporarily) */ \
	if (intmode >= TIMER_STOP || dur == 0 || *cb == NULL) { \
		/* incorrect parameters, prevent execution of callback method \
		 * because that's the only thing we know for sure the caller intended us to do */ \
		TIMSK ## TIMERNUM &= ~_BV(OCIE ## TIMERNUM ## A); \
		t ## TIMERNUM ## _mode = TIMER_STOP; \
		return 1; \
	} \
	/* common settings */ \
	t ## TIMERNUM ## _cnt_long = t ## TIMERNUM ## _dur_long = dur / DUR ## TIMERNUM ## _LONG; \
	t ## TIMERNUM ## _cnt_short = t ## TIMERNUM ## _dur_short = dur % DUR ## TIMERNUM ## _LONG; \
	T ## TIMERNUM ## _WGM_CTC(); \
	PWMA_OFF(TIMERNUM); \
	TCNT ## TIMERNUM = 0; \
	t ## TIMERNUM ## _callback = cb; \
	t ## TIMERNUM ## _mode = intmode; \
\
	if (t ## TIMERNUM ## _dur_long > 0) { \
		/* setup long timer */ \
		SET_TPRESC(TIMERNUM, PRESC ## TIMERNUM ## _LONG); \
		OCR ## TIMERNUM ## A = OCR ## TIMERNUM ## _LONG; \
	} else if(t ## TIMERNUM ## _dur_short > 0) { \
		/* setup short timer */ \
		SET_TPRESC(TIMERNUM, PRESC ## TIMERNUM ## _SHORT); \
		OCR ## TIMERNUM ## A = OCR ## TIMERNUM ## _SHORT; \
	} \
	OCR ## TIMERNUM ## B = 0; \
	/* TCCR ## TIMERNUM ## A &= ~_BV(COM ## TIMERNUM ## B1); */ \
	/* TCCR ## TIMERNUM ## A |= _BV(COM ## TIMERNUM ## B0); */ \
	TIMSK ## TIMERNUM |= _BV(OCIE ## TIMERNUM ## A); /* enable OCRnA match interrupt */ \
	return 0; \
} \
\
/** decrements the t TIMERNUM _cnt_long (or t TIMERNUM _cnt_short, if t TIMERNUM _cnt_long is already 0). if both counters \
are 0, it stops the timer or reconfigures it, if \c mode = \c TIMER_PERIODIC. \
the interrupt is temporarily disabled while the callback function is called to \
prevent a stack overflow when the callback function takes longer to finish than \
the timer's period. */ \
ISR(TIMER ## TIMERNUM ## _COMPA_vect) { \
	if (t ## TIMERNUM ## _cnt_long > 1) { \
		t ## TIMERNUM ## _cnt_long--; \
	} else { \
		if (t ## TIMERNUM ## _cnt_short > 1) { \
			if ((TCCR ## TIMERNUM ## B & 0x7) == PRESC ## TIMERNUM ## _LONG) { /* last interrupt form tX_cnt_long */ \
				/* setup short timer */ \
				SET_TPRESC(TIMERNUM, PRESC ## TIMERNUM ## _SHORT); \
				TCNT ## TIMERNUM = 0; \
				OCR ## TIMERNUM ## A = OCR ## TIMERNUM ## _SHORT; \
			}else \
				t ## TIMERNUM ## _cnt_short--; \
		} else { /* duration is over */ \
			if (t ## TIMERNUM ## _mode == TIMER_PERIODIC) { \
				t ## TIMERNUM ## _cnt_long = t ## TIMERNUM ## _dur_long; \
				t ## TIMERNUM ## _cnt_short  = t ## TIMERNUM ## _dur_short; \
				if(t ## TIMERNUM ## _dur_long > 0){ \
					/* setup long timer */ \
					SET_TPRESC(TIMERNUM, PRESC ## TIMERNUM ## _LONG); \
					TCNT ## TIMERNUM = 0; \
					OCR ## TIMERNUM ## A = OCR ## TIMERNUM ## _LONG; \
				} \
				/* else ... not necessary to set the short timer again */ \
			} else { /* TIMER_ONESHOT */ \
				SET_TPRESC(TIMERNUM, PRESC_OFF); \
			} \
			/* disable OCR1 match interrupt at least temporarily */ \
			TIMSK ## TIMERNUM &= ~_BV(OCIE ## TIMERNUM ## A); \
			sei(); /* reenable other interrupts asap (before callback!) */ \
			(*t ## TIMERNUM ## _callback)(); \
			if (t ## TIMERNUM ## _mode == TIMER_PERIODIC) \
				TIMSK ## TIMERNUM |= _BV(OCIE ## TIMERNUM ## A); /* enable OCR1 match interrupt */ \
			else \
				t ## TIMERNUM ## _mode = TIMER_STOP; \
		} \
	} \
}

#ifdef PRESC0_SHORT
TIMER_CODE(0)
#endif
#ifdef PRESC1_SHORT
TIMER_CODE(1)
#endif
