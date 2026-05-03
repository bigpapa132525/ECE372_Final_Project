#include <avr/io.h>
#include "timer.h"
#include "spi.h"

/* ---------------------------------------------------------
   Bit-banged SPI for TDC
   SPI Mode 1 style:
   CPOL = 0, clock idles LOW
   CPHA = 1, data sampled near falling edge

   Example pin mapping:
   PB0 = SCK
   PB2 = MOSI
   PB3 = MISO
   PB4 = CS
   --------------------------------------------------------- */


void initSPI(void) {
    // SCK, MOSI, CS outputs
    SPI_DDR |= (1 << SCK_PIN) | (1 << MOSI_PIN) | (1 << CS_PIN);

    // MISO input
    SPI_DDR &= ~(1 << MISO_PIN);

    // Idle states
    SPI_PORT &= ~(1 << SCK_PIN);   // SCK low
    SPI_PORT &= ~(1 << MOSI_PIN);  // MOSI low
    SPI_PORT |=  (1 << CS_PIN);    // CS high
}

unsigned char transferSPI(unsigned char dataOut) {
    unsigned char dataIn = 0;

    for (int i = 7; i >= 0; i--) {
        // Set MOSI bit
        if (dataOut & (1 << i)) {
            SPI_PORT |= (1 << MOSI_PIN);
        } else {
            SPI_PORT &= ~(1 << MOSI_PIN);
        }

        delayUs(1);

        // Rising edge
        SPI_PORT |= (1 << SCK_PIN);
        delayUs(1);

        // Read MISO while clock is high
        dataIn <<= 1;
        if (SPI_PIN & (1 << MISO_PIN)) {
            dataIn |= 1;
        }

        // Falling edge
        SPI_PORT &= ~(1 << SCK_PIN);
        delayUs(1);
    }

    return dataIn;
}

unsigned int readRegister(unsigned char address, int datalength) {
    unsigned char data = 0;
    unsigned int fulldata = 0;

    address = address & READ_CMD;

    SPI_PORT &= ~(1 << CS_PIN);
    delayUs(1);

    transferSPI(address);

    fulldata = transferSPI(0x00);
    datalength--;

    while (datalength > 0) {
        fulldata <<= 8;
        data = transferSPI(0x00);
        fulldata |= data;
        datalength--;
    }

    delayUs(1);
    SPI_PORT |= (1 << CS_PIN);

    return fulldata;
}

void writeRegister(unsigned char address, unsigned char value) {
    address = address | WRITE_CMD;

    SPI_PORT &= ~(1 << CS_PIN);
    delayUs(1);

    transferSPI(address);
    transferSPI(value);

    delayUs(1);
    SPI_PORT |= (1 << CS_PIN);
}