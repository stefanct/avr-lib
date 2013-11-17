/**\file
timer drivers header.*/
#ifndef __TIMERS_H__
#define __TIMERS_H__

#include <stdint.h>
#include <avr/interrupt.h>
#include "pwm.h"

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

#endif // __TIMERS_H__
