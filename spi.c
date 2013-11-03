/**\file
SPI driver.*/

#include <avr/io.h>
#include "spi.h"

void spiInit(uint8_t lsb_first, uint8_t cpol, uint8_t cpha, uint8_t div){
	// need to set SS pin to output (else driving it low, will set slave mode)!
	SPI_DDR |= _BV(SPI_SCK) | _BV(SPI_MOSI) | _BV(SPI_SS); // set SCK, MOSI and SS as output
	SPI_DDR &= ~_BV(SPI_MISO); // set MISO as input
	//SPIE SPE DORD MSTR CPOL CPHA SPR1 SPR0
	uint8_t config = 0x50; // disable interrupts, enable SPI, set as master
	config |= (lsb_first)?_BV(DORD):0; // set endianess
	config |= (cpol)?_BV(CPOL):0; // set clock polarity
	config |= (cpha)?_BV(CPHA):0; // set clock phase
	config |= div & 0x3; // set prescaler
	SPCR = config;
	uint8_t spi2x = div & 0x4;
	if(spi2x)
		SPSR |= 1;
	else
		SPSR &= ~1;
}

uint8_t spiSend(uint8_t d){
	SPDR = d;
	while(!(SPSR & _BV(SPIF)))
		;
	d = SPDR;
	return d;
}

inline uint16_t spiSend16(uint16_t d){
	uint16_t ret = spiSend(d>>8)<<8;
	ret |= spiSend(d&0xff);
	return ret;
}

inline uint32_t spiRead20(){
	uint32_t ret = ((uint32_t)spiSend(0))<<12;
	ret |= ((uint16_t)spiSend(0))<<4;
	ret |= spiSend(0)>>4;
	return ret;
}
