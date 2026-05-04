#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

/* Pin mapping */
#define SPI_PORT PORTB
#define SPI_PIN  PINB
#define SPI_DDR  DDRB

#define SCK_PIN  DDB0
#define MOSI_PIN DDB2
#define MISO_PIN DDB3
#define CS_PIN   DDB4

/* TDC SPI command masks */
#define READ_CMD  0b10111111
#define WRITE_CMD 0b01000000

void initSPI(void);

unsigned char transferSPI(unsigned char dataOut);

uint32_t  readRegister(unsigned char address, int datalength);

void writeRegister(unsigned char address, unsigned char value);

#endif