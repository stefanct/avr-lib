/** \file
Helper macros of various kind.*/

#ifndef __MACROS_H__
#define __MACROS_H__

#include <stdint.h>

/** returns the sizeof member \a m of a struct (type) \a st*/
#define msizeof(struct_type, member) (sizeof(((struct_type *)(0) )->member))

/** \name bit operations on integers*/
//@{
/** SEB sets the bit on position BIT in variable VAR */
#define SEB(VAR, BIT)   (_SFR_BYTE(VAR) |=  (1<<(BIT)))
/** CLB clears the bit on position BIT in variable VAR */
#define CLB(VAR, BIT)   (_SFR_BYTE(VAR) = (uint8_t)((~(_BV(BIT))) & _SFR_BYTE(VAR)))
/** TOB toggles the bit on position BIT in variable VAR */
#define TOB(VAR, BIT)   (_SFR_BYTE(VAR) ^=  (1<<(BIT)))
// @}

/** \name 8-bit binary constants

Allows to specify binary constants in C. */
//@{
#define __B(d) ((uint8_t)__B8(0x##d))
#define __B8(x) \
 ((x&0x0000000F)?1:0) \
+((x&0x000000F0)?2:0) \
+((x&0x00000F00)?4:0) \
+((x&0x0000F000)?8:0) \
+((x&0x000F0000)?16:0) \
+((x&0x00F00000)?32:0) \
+((x&0x0F000000)?64:0) \
+((x&0xF0000000)?128:0)
//@}

/** sets bits in register REG, masked by MASK, given by VALUE */
#define CFG_REG(REG, VALUE, MASK) (REG) |= ((MASK) & (VALUE)); ((REG) &= (~(MASK) | (VALUE)))

/** Convenience macro to combine I/O DDR/PORT/PIN and pin assignments together.

	Taken from avr-freaks, unkown author, sorry!

	Usage example:

\code	#define LED2(reg) BIT(D,5,reg)
	#define SW2(reg) BIT(D,2,reg)

	int main()
	{
		SW2(DDR) = INPUT;
		SW2(PORT) = PULLUP_ON;
		LED2(DDR) = OUTPUT;
		while(1)
		{
			if(SW2(PIN) == LOW)
			{
				LED2(PORT) = HIGH;
			}
			else
			{
				LED2(PORT) = LOW;
			}
		}
	}\endcode
*/
//@{
__extension__ struct bits { 
	uint8_t b0:1;
	uint8_t b1:1;
	uint8_t b2:1;
	uint8_t b3:1;
	uint8_t b4:1;
	uint8_t b5:1;
	uint8_t b6:1;
	uint8_t b7:1;
} __attribute__((__packed__,));
#define BIT(port,pin,reg)\
((*(volatile struct bits*)&reg##port).b##pin)
// @}

/** \name Register name conversions

Allows to derive port, ddr and pin register names. */
//@{

#define PORT_(port) PORT ## port
#define DDR_(port)  DDR  ## port
#define PIN_(port)  PIN  ## port

#define PORT(port) PORT_(port)
#define DDR(port)  DDR_(port)
#define PIN(port)  PIN_(port)
// @}

#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)

/* http://www.pixelbeat.org/programming/gcc/static_assert.html */
#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
/* These can't be used after statements in c89. */
#ifdef __COUNTER__
	#define STATIC_ASSERT(e,m) \
		{ enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1/(!!(e)) }; }
#else
	/* This can't be used twice on the same line so ensure if using in headers
	 * that the headers are not included twice (by wrapping in #ifndef...#endif)
	 * Note it doesn't cause an issue when used on same line of separate modules
	 * compiled with gcc -combine -fwhole-program.  */
	#define STATIC_ASSERT(e,m) \
		{ enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }; }
#endif // __COUNTER__

#endif // __MACROS_H__
