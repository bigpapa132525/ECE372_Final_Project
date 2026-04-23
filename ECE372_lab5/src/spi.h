#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

/* SPI initialization for MAX7219 8x8 LED matrix */
void initSPI(void);

/* Send 16-bit data to MAX7219 */
void sendDataSPI(unsigned char address, unsigned char data);

void displaySmileyFace();

void displayFrownyFace();


#endif