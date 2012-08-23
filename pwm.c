/** \file
PWM driver.*/

#include <avr/io.h>
#include "macros.h"
#include "pwm.h"


/** Initializes the PWM unit.
Sets the prescaler bits \c CSn[2:0] according to \a prescaler.
Sets the \c OCRnA bits of the n-th timer according to \a OCRnA.
The resulting PWM frequency \f$f_{\mathrm{OCnA}} = \frac{1}{2 \cdot \mathrm{prescaler} \cdot (1 + \mathrm{OCRnA})}\f$.
Disables all timer1-related interrupts, sets the #PWM_PIN to output and sets CTC mode for timer1.
@see prescaler1
*/
void pwm_init(uint8_t prescaler, uint8_t OCRnA){
	TIMSK1 = 0; // disable interrupts
	OCR1A = OCRnA;
	PWM_PIN(DDR) = 1;
	pwmSetCompareMode(PWM_C, PWM_TOGGLE);
	SET_MODE1_CTC();
	SET_PRESC1(prescaler);
}
