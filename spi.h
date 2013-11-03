/** @file
SPI driver header.*/

#ifndef __SPIH_H__
#define __SPIH_H__
#include <stdint.h>
#include <stdio.h>

#if defined (__DOXYGEN__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)	/*********************/
/** \name SPI Pin definitions.*/
//@{
#define SPI_PORT PORTB
#define SPI_DDR	DDRB
#define SPI_SS PB2
#define SPI_MOSI PB3
#define SPI_MISO PB4
#define SPI_SCK PB5
//@}

#elif defined(__AVR_AT90USB162__) || defined(__AVR_ATmega8U2__) || defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega32U2__) || \
    defined(__AVR_ATmega16U4__) || defined(__AVR_ATmega32U4__)
#define SPI_PORT PORTB
#define SPI_DDR	DDRB
#define SPI_SS PB0
#define SPI_MOSI PB2
#define SPI_MISO PB3
#define SPI_SCK PB1

#else /*********************/
	#error "Unsupported Target!\n"
#endif

/** \name SPI prescaler settings*/
//@{
#define SPI_DIV_2   __B(100)
#define SPI_DIV_4   __B(000)
#define SPI_DIV_8   __B(101)
#define SPI_DIV_16  __B(001)
#define SPI_DIV_32  __B(110)
#define SPI_DIV_64  __B(010)
#define SPI_DIV_128 __B(011)
//@}

void spiInit(uint8_t lsb_first, uint8_t cpol, uint8_t cpha, uint8_t div);
uint8_t spiSend(uint8_t d);
uint16_t spiSend16(uint16_t d);
uint32_t spiRead20(void);
#endif // __SPIH_H__
