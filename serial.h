/** @file
Serial driver header.*/
#ifndef Serial_h
#define Serial_h
#include <stdint.h>
#include <stdio.h>

									/********** ATmegaxxU2 ***********/
#if defined(__AVR_AT90USB162__) \
|| defined(__AVR_ATmega8U2__) || defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega32U2__)
// u2 USB ATmega with one UART
// #define AT90_UART
// interrupts
#define UART_RX_VECT  USART1_RX_vect
#define UART_TX_VECT  USART1_UDRE_vect

// registers
#define UART_DATA     UDR1
#define UART_BAUD_LO  UBRR1L
#define UART_BAUD_HI  UBRR1H
#define UART_STATUS   UCSR1A
#define UART_CONTROL  UCSR1B
#define UART_MODE     UCSR1C
#define UART_FLOW     UCSR1D
#define UART_CHARR0   UCSR1C
#define UART_CHARR1   UCSR1C
#define UART_CHARR2   UCSR1B


// bits
#define UART_2X       U2X1

#define UART_RX_IE    RXCIE1
//#define UART_TX_IE    TXCIE1
#define UART_TXR_IE   UDRIE1
#define UART_RX_EN    RXEN1
#define UART_TX_EN    TXEN1

#define UART_DRE      UDRE1

#define UART_MOD_SEL1 UMSEL11
#define UART_MOD_SEL0 UMSEL10
#define UART_PARITY1  UPM11
#define UART_PARITY0  UPM10
#define UART_STOPB    USBS1
#define UART_CHAR0    UCSZ10
#define UART_CHAR1    UCSZ12
#define UART_CHAR2    UCSZ12
#define UART_POL      UCPOL1
									/********** ATmega168 ***********/
#elif defined(__AVR_ATmega168__)
// interrupts
#define UART_RX_VECT  USART0_RX_vect
#define UART_TX_VECT  USART0_UDRE_vect

// registers
#define UART_DATA     UDR0
#define UART_BAUD_LO  UBRR0L
#define UART_BAUD_HI  UBRR0H
#define UART_STATUS   UCSR0A
#define UART_CONTROL  UCSR0B
#define UART_MODE     UCSR0C
#define UART_FLOW     UCSR0D

// bits
#define UART_RX_IE    RXCIE0
//#define UART_TX_IE    TXCIE0
#define UART_TXR_IE    UDRIE0
#define UART_RX_EN    RXEN0
#define UART_TX_EN    TXEN0

#define UART_DRE      UDRE0

#define UART_STOPB    USBS0
#endif

int serialPutCharBlock(char c, FILE *stream);
int serialGetCharBlock(FILE *stream);
int serialPutCharNoWait(char c, FILE *stream);
int serialGetCharNoWait(FILE *stream);
void serialInit(void);
uint8_t serialReadAvailableCnt(void);
uint8_t serialReadAvailableCntWait(uint8_t bytes, uint8_t milliSeconds);
uint8_t serialReadIsAvailable(void);
uint8_t serialReadPeek(uint8_t i);
uint8_t serialReadNoWait(void);
uint16_t serialReadWait(uint8_t milliSeconds);
uint8_t serialReadBlock(void);
uint8_t serialWriteAvailableCnt(void);
uint8_t serialWriteIsAvailable(void);
uint8_t serialWriteNoWait(unsigned char c);
uint8_t serialWriteBlock(unsigned char c);

#endif
