/** \file
PWM driver header.*/
#include "macros.h"

/** Selects the correct bits of the various pin registers.*/
#define PWM_PIN(reg) BIT(B,7,reg)

/** \addtogroup prescaler1 Prescaler macros for timer 1
\code
Macro        CSn2   CSn1   CSn0    Description
PRESC1_OFF      0      0      0     No clock source. (Timer/Counter stopped)
PRESC1_1        0      0      1     clk_io/1 (No prescaling
PRESC1_8        0      1      0     clk_io/8 (From prescaler)
PRESC1_64       0      1      1     clk_io/64 (From prescaler)
PRESC1_256      1      0      0     clk_io/256 (From prescaler)
PRESC1_1024     1      0      1     clk_io/1024 (From prescaler)
PRESC1_EXTF     1      1      0     External clock source on Tn pin. Clock on falling edge
PRESC1_EXTR     1      1      1     External clock source on Tn pin. Clock on rising edge
\endcode*/
//@{
#define PRESC1_OFF	__B(000)
#define PRESC1_1	__B(001)
#define PRESC1_8	__B(010)
#define PRESC1_64	__B(011)
#define PRESC1_256	__B(100)
#define PRESC1_1024	__B(101)
#define PRESC1_EXTF	__B(110)
#define PRESC1_EXTR	__B(111)
//@}

#define SET_PRESC1(PRESC) CFG_REG(TCCR1B, PRESC, __B(111))

enum PWM_CHANNEL {PWM_A=2, PWM_B=1, PWM_C=0};
enum PWM_COMP_MODE {PWM_NORMAL=0, PWM_TOGGLE=1, PWM_CLEAR=2, PWM_SET=3};

#define pwmSetCompareMode(CHANNEL, COMP_MODE) \
	TCCR1A |= COMP_MODE<<(2*CHANNEL + 2);

#define SET_MODE1_CTC()	TCCR1A &= ~(_BV(WGM11) | _BV(WGM10));\
	TCCR1B &= ~(_BV(WGM13));\
	TCCR1B |= _BV(WGM12)

void pwm_init(uint8_t prescaler, uint8_t OCRnA);
