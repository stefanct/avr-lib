/** @file
Serial driver + buffer.*/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "macros.h"
#include "serial.h"

#ifndef AVRLIB_SERIAL_WO
/** @name Variables and defines related to the receive buffer*/
//@{
#define RX_BUFFER_SIZE 8 /**< Size of the buffer \c rx_buffer.*/
#define RX_BUFFER_MASK (RX_BUFFER_SIZE - 1)
#if ( RX_BUFFER_SIZE & RX_BUFFER_MASK )
#error RX buffer size is not a power of 2
#endif

static volatile unsigned char rx_buffer[RX_BUFFER_SIZE]; /**< Buffer that stores incoming characters.*/
static volatile uint8_t rx_buffer_head = 0; /**< Index where to write the next incoming character.*/
static volatile uint8_t rx_buffer_tail = 0; /**< Index where to read next.*/
//@}
#endif // !AVRLIB_SERIAL_WO

/** @name Variables and defines related to the transmit buffer*/
//@{
#define TX_BUFFER_SIZE 64 /**< Size of the buffer \c tx_buffer.*/
#define TX_BUFFER_MASK (TX_BUFFER_SIZE - 1)
#if ( TX_BUFFER_SIZE & TX_BUFFER_MASK )
#error TX buffer size is not a power of 2
#endif

#if ( TX_BUFFER_SIZE > 256 || RX_BUFFER_SIZE > 256 )
#error serial buffers have to be <= 256B
#endif

static volatile unsigned char tx_buffer[TX_BUFFER_SIZE]; /**< Buffer that stores outgoing characters.*/
static volatile uint8_t tx_buffer_head = 0; /**< Index where to write next.*/
static volatile uint8_t tx_buffer_tail = 0; /**< Index where to read the next outgoing character.*/
//@}

/** @name stdio wrappers*/
//@{
inline int serialPutCharBlock(char c, FILE *stream){
	(void)stream;
	return serialWriteBlock(c);
}

inline int serialPutCharNoWait(char c, FILE *stream){
	(void)stream;
	serialWriteNoWait(c);
	return 0;
}

#ifndef AVRLIB_SERIAL_WO
inline int serialGetCharBlock(FILE *stream){
	(void)stream;
	return serialReadBlock();
}

inline int serialGetCharNoWait(FILE *stream){
	(void)stream;
	serialReadNoWait();
	return 0;
}
#endif // !AVRLIB_SERIAL_WO
//@}

/** @name baud rate settings
 * The default baud rate is 57600. This can be changed by defining
 * AVRLIB_SERIAL_BAUD to expand to the wanted baud rate. */
//@{
#ifndef BAUD
	#ifdef AVRLIB_SERIAL_BAUD
		#define BAUD AVRLIB_SERIAL_BAUD
	#else
		#define BAUD 57600
	#endif
	#define BAUD_TOL 4
#endif
//@}

/** Initializes the UART.*/
void serialInit(void){
	#include <util/setbaud.h>
	UART_BAUD_HI = UBRRH_VALUE;
	UART_BAUD_LO = UBRRL_VALUE;
	#if USE_2X
		SEB(UART_STATUS, UART_2X);
	#else
		CLB(UART_STATUS, UART_2X);
	#endif
	
	// asynchronous, normal mode
	CLB(UART_MODE, UART_MOD_SEL1);
	CLB(UART_MODE, UART_MOD_SEL0);
	// even parity
	SEB(UART_MODE, UART_PARITY1);
	CLB(UART_MODE, UART_PARITY0);
	// 1 stop bit
	SEB(UART_MODE, UART_STOPB);
	// 8 bits per char
	CLB(UART_CHARR2, UART_CHAR2);
	SEB(UART_CHARR1, UART_CHAR1);
	SEB(UART_CHARR0, UART_CHAR0);
	// clock polarity
	CLB(UART_MODE, UART_POL);
	
	// disable flow control (not available on u4)
#if !(defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__))
	CLB(UART_FLOW, CTSEN);
	CLB(UART_FLOW, RTSEN);
#endif

	// enable rx and tx
	SEB(UART_CONTROL, UART_TX_EN);
#ifndef AVRLIB_SERIAL_WO
		SEB(UART_CONTROL, UART_RX_EN);
		// enable interrupt on complete reception of a byte
		SEB(UART_CONTROL, UART_RX_IE);
#endif // !AVRLIB_SERIAL_WO
}

#ifndef AVRLIB_SERIAL_WO
/** @name Reading*/
//@{
/** Returns the number of available characters in the buffer.*/
uint8_t serialReadAvailableCnt(void){
	CLB(UART_CONTROL, UART_RX_IE);
	uint8_t ret = (rx_buffer_head - rx_buffer_tail) & RX_BUFFER_MASK;
	SEB(UART_CONTROL, UART_RX_IE);
	return ret;
}

/** Returns 1, if there are \a bytes bytes available in the buffer before \a milliSeconds milliSeconds have passed, 0 otherwise.*/
uint8_t serialReadAvailableCntWait(uint8_t bytes, uint8_t milliSeconds){
	if(milliSeconds == 0)
		milliSeconds = 1;
	uint16_t cnt = milliSeconds;
	for(;cnt>0;cnt--){
		if(serialReadAvailableCnt() >= bytes)
			return 1;
		_delay_ms(1);
	}
	return 0;
}

/** Returns true if there is at least one available character in the rx buffer*/
uint8_t serialReadIsAvailable(void){
	CLB(UART_CONTROL, UART_RX_IE);
	uint8_t ret = rx_buffer_head != rx_buffer_tail;
	SEB(UART_CONTROL, UART_RX_IE);
	return ret;
}

/** Returns the value stored at the \c i-th position to be read.
Note that this may not be the value that is actually returned by a following read.*/
uint8_t serialReadPeek(uint8_t i){
	CLB(UART_CONTROL, UART_RX_IE);
	uint8_t ret = rx_buffer[(rx_buffer_tail + i) & RX_BUFFER_MASK];
	SEB(UART_CONTROL, UART_RX_IE);
	return ret;
}

/** Returns the next value from the buffer and increases the index.
It is not checked, if there is a valid value at the current position that is returned.*/
uint8_t serialReadNoWait(void){
	CLB(UART_CONTROL, UART_RX_IE);
	unsigned char c = rx_buffer[rx_buffer_tail];
	rx_buffer_tail = (rx_buffer_tail + 1) & RX_BUFFER_MASK;
	SEB(UART_CONTROL, UART_RX_IE);
	return c;
}

/** A read blocking up to \a milliSeconds milliseconds.
Returns -1 if there is no data available even after waiting for \a milliSeconds milliseconds.*/
uint16_t serialReadWait(uint8_t milliSeconds){
	if(milliSeconds == 0)
		milliSeconds = 1;
	uint8_t cnt = milliSeconds;
	for(;cnt>0;cnt--){
		if(serialReadIsAvailable())
			return serialReadNoWait();
		_delay_ms(1);
	}
	return -1;
}

/** Blocking read.*/
uint8_t serialReadBlock(void){
	while(!serialReadIsAvailable())
		;
	return serialReadNoWait();
}
//@}

#endif // !AVRLIB_SERIAL_WO

/** @name Writing*/
//@{
/** Returns the number of free characters in the tx_buffer.*/
uint8_t serialWriteAvailableCnt(void){
	CLB(UART_CONTROL, UART_TXR_IE);
	uint8_t ret = TX_BUFFER_SIZE - 1 - ((tx_buffer_head - tx_buffer_tail) & TX_BUFFER_MASK);
	if(tx_buffer_head != tx_buffer_tail){
		SEB(UART_CONTROL, UART_TXR_IE);
	}
	return ret;
}

/** Returns true if there is at least one free character in the tx buffer*/
uint8_t serialWriteIsAvailable(void){
	return serialWriteAvailableCnt() != 0;
}

/** Non-blocking write.
\return 0 on success, 1 if the buffer was full.*/
uint8_t serialWriteNoWait(unsigned char c){
	if(serialWriteAvailableCnt() == 0)
		return 1;
	CLB(UART_CONTROL, UART_TXR_IE);
	tx_buffer[tx_buffer_head] = c;
	tx_buffer_head = (tx_buffer_head + 1) & TX_BUFFER_MASK;
	SEB(UART_CONTROL, UART_TXR_IE);
	return 0;
}

/** Blocking write.*/
uint8_t serialWriteBlock(unsigned char c){
	while(serialWriteNoWait(c) != 0)
		;
	return 0;
}
//@}

#ifndef AVRLIB_SERIAL_WO
/** Interrupt routine that is called when a new character has arrived.
If there is space it stores the recevied data in the buffer at index \c rx_buffer_head.*/
ISR(UART_RX_VECT){
	unsigned char c = UART_DATA;

	int i = (rx_buffer_head + 1) & RX_BUFFER_MASK;

	if (i != rx_buffer_tail) {
		rx_buffer[rx_buffer_head] = c;
		rx_buffer_head = i;
	}
}
#endif // !AVRLIB_SERIAL_WO

/** Interrupt routine that is called when the UART is ready to send a new byte.
It checks the tx_buffer for unsent data and puts it into the UART's output register if available.
If there is none it disables the transmit ready interrupt,
which has to be reenabled by a write to the buffer.*/
ISR(UART_TX_VECT){
	if(tx_buffer_tail != tx_buffer_head) {
		UART_DATA = tx_buffer[tx_buffer_tail];
		tx_buffer_tail = (tx_buffer_tail + 1) & TX_BUFFER_MASK;
	}else{
		CLB(UART_CONTROL, UART_TXR_IE);
	}
}
