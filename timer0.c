/**\file
timer0 driver.

Can be used to execute functions periodically or after a one-time delay.*/

#include <avr/io.h>
#include <stdlib.h>
#include "timer0.h"
#include "debug.h"

static enum timer0mode mode	= 0;/**< stores the timer mode */
static uint16_t cnt_long	= 0;/**< gets decremented every \c DUR0_LONG s*/
static uint8_t  cnt_short	= 0;/**< gets decremented every \c RES0 s, if there are no more \c DUR0_LONG s intervals for this period to do */
static uint16_t dur_long 	= 0;/**< stores how many \c DUR0_LONG s intervals are needed in one period */
static uint8_t  dur_short	= 0;/**< stores how many \c RES0 s intervals are needed (in addition to the \c DUR0_LONG s intervals) in one period */

static void (*t0callback)(void);/**< function pointer that references the function called every \c duration \c RES0 s*/

/** Sets up timer0 so that \c cb is called after/every \c dur \c RES0 s.
Stops the timer if \c intmod is \c TIMER_STOP or \c dur is 0.
Otherwise divides \c dur into a maximum amount of \c DUR0_LONG s time slices
and uses the remainder as counter for short 1 \c RES0 s time slices.
\param intmode Timer mode. one of TIMER_PERIODIC, TIMER_ONESHOT or TIMER_STOP as defined by #timer0mode
\param dur Duration to/between interrupt(s)
\param cb functionpointer that references the function to call */
void configure_timer0(uint8_t intmode, uint16_t dur, void (*cb)(void)){
	if(intmode >= TIMER_STOP || dur == 0 || *cb == NULL){
		// incorrect parameters, prevent execution of callback method
		// because that's the only thing we know for sure the caller intended us to do
		TIMSK0 &= ~_BV(OCIE0A);
		T0_PRESC(PRESC0_OFF); // disable timer
	}else{
		// common settings
		cnt_long = dur_long = dur / DUR0_LONG;
		cnt_short = dur_short = dur % DUR0_LONG;
		T0_WGM_CTC();
		T0_PWMA_OFF();
		TCNT0 = 0;
		t0callback = cb;
		mode = intmode;
	
		if(dur_long > 0){
			// setup long timer
			T0_PRESC(PRESC0_LONG);
			OCR0A = OCR0_LONG;
		}else if(dur_short > 0){
			// setup short timer
			T0_PRESC(PRESC0_SHORT);
			OCR0A = OCR0_SHORT;
		}
		DDRD |= _BV(PD0);
		PORTD |= _BV(PD0);
		OCR0B = 0;
		TCCR0A &= ~_BV(COM0B1);
		TCCR0A |= _BV(COM0B0);
		TIMSK0 |= _BV(OCIE0A);// enable OCR0 match interrupt
	}
}

/** decrements the cnt_long (or cnt_short, if cnt_long is already 0). if both counters
are 0, it stops the timer or reconfigures it, if \c mode = \c TIMER_PERIODIC.
the interrupt is temporarily disabled while the callback function is called to
prevent a stack overflow when the callback function takes longer to finish than
the timer's period. */
ISR(TIMER0_COMPA_vect){
	DEBUG_PIN0(PORT) = 1;
	if(cnt_long > 1){
		cnt_long--;
	}else{
		if(cnt_short > 1){
			if((TCCR0B & 0x7) == PRESC0_LONG){ // last interrupt form cnt_long
				// setup short timer
				T0_PRESC(PRESC0_SHORT);
				TCNT0 = 0;
				OCR0A = OCR0_SHORT;
			}else
				cnt_short--;
		}else{ // duration is over
			if(mode == TIMER_PERIODIC){
				cnt_long = dur_long;
				cnt_short  = dur_short;
				if(dur_long > 0){
					// setup long timer
					T0_PRESC(PRESC0_LONG);
					TCNT0 = 0;
					OCR0A = OCR0_LONG;
				}
				// else ... not necessary to set the short timer again
			}else{ // TIMER_ONESHOT
				T0_PRESC(PRESC0_OFF);
			}
			// disable OCR0 match interrupt at least temporarily
			TIMSK0 &= ~_BV(OCIE0A);
			sei(); // reenable other interrupts asap (before callback!)
			(*t0callback)();
			if(mode == TIMER_PERIODIC)
				TIMSK0 |= _BV(OCIE0A); // enable OCR0 match interrupt
		}
	}
	DEBUG_PIN0(PORT) = 0;
}
