#include "SPI.h"
#include "timer.h"
#include <avr/io.h>

/* --------------------------------------------------------------------
   Bit-banging SPI for MAX7219 8x8 LED Matrix
   Using PB0 = SCK, PB2 = MOSI, PB4 = CS
   -------------------------------------------------------------------- */

void initSPI(void) {
    // Set SCK (PB0), MOSI (PB2), and CS (PB4) as outputs
    DDRB |= (1 << DDB0) | (1 << DDB2) | (1 << DDB4);
    
    // Initialize pins to idle state
    PORTB &= ~(1 << PORTB0);  // SCK low
    PORTB &= ~(1 << PORTB2); // MOSI low
    PORTB |= (1 << PORTB4);  // CS high (idle)
}

void sendDataSPI(unsigned char address, unsigned char data) {
    // Pull CS low
    PORTB &= ~(1 << PORTB4);
    delayUs(1);
    
    // Send address byte (8 bits), MSB first
    for (int i = 7; i >= 0; i--) {
        // Set MOSI based on bit
        if (address & (1 << i)) {
            PORTB |= (1 << PORTB2);
        } else {
            PORTB &= ~(1 << PORTB2);
        }
        delayUs(1);
        
        // Clock pulse (low to high to low)
        PORTB |= (1 << PORTB0);
        delayUs(1);
        PORTB &= ~(1 << PORTB0);
        delayUs(1);
    }
    
    // Send data byte (8 bits), MSB first
    for (int i = 7; i >= 0; i--) {
        // Set MOSI based on bit
        if (data & (1 << i)) {
            PORTB |= (1 << PORTB2);
        } else {
            PORTB &= ~(1 << PORTB2);
        }
        delayUs(1);
        
        // Clock pulse
        PORTB |= (1 << PORTB0);
        delayUs(1);
        PORTB &= ~(1 << PORTB0);
        delayUs(1);
    }
    
    delayUs(1);
    
    // Pull CS high to latch data
    PORTB |= (1 << PORTB4);
}    
void displaySmileyFace(){
            // Happy face - eyes 00100100, mouth straight line with corner curves
        sendDataSPI(1, 0x00);  // top
        sendDataSPI(2, 0x24);  // eyes: 00100100
        sendDataSPI(3, 0x24);  // eyes: 00100100
        sendDataSPI(4, 0x24);
        sendDataSPI(5, 0x00);  
        sendDataSPI(6, 0x42);  // corners curve up
        sendDataSPI(7, 0x3C);  
        sendDataSPI(8, 0x00);  // bottom
       
        delayMs(100);
}
void displayFrownyFace(){
        // Sad face
        sendDataSPI(1, 0x00);  // top
        sendDataSPI(2, 0x24);  // eyes
        sendDataSPI(3, 0x24);  // eyes
        sendDataSPI(4, 0x24);
        sendDataSPI(5, 0x00);  
        sendDataSPI(6, 0x3C);
        sendDataSPI(7, 0x42);  
        sendDataSPI(8, 0x00);  // bottom

        delayMs(100);
}

