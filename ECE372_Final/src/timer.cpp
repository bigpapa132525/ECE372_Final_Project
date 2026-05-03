// Author:
// Net ID:
// Date:
// Assignment: Lab 3
//----------------------------------------------------------------------//

#include "timer.h"
#include <avr/io.h>

/* --------------------------------------------------------------------
   TIMER 0
   Used for millisecond delays / debounce
   CTC mode
   -------------------------------------------------------------------- */
void initTimer0(void) {
    // Clear control registers
    TCCR0A = 0x00;
    TCCR0B = 0x00;

    // CTC mode: WGM02:0 = 010
    TCCR0A |= (1 << WGM01);

    // Prescaler = 64
    // 16 MHz / 64 = 250 kHz
    // Tick = 4 us
    TCCR0B |= (1 << CS01) | (1 << CS00);

    // 1 ms = 250 ticks
    // Count 0 to 249
    OCR0A = 249;

    // Clear counter
    TCNT0 = 0;

    // Clear compare flag
    TIFR0 |= (1 << OCF0A);
}

void delayMs(unsigned int delay) {
    unsigned int i;

    for (i = 0; i < delay; i++) {
        TCNT0 = 0;                 // reset counter
        TIFR0 |= (1 << OCF0A);     // clear compare flag

        while (!(TIFR0 & (1 << OCF0A))) {
            ;
        }
    }
}

void delay_3ms(void) {
    delayMs(3);
}

/* --------------------------------------------------------------------
   TIMER 1
   Used for microsecond delays and second countdown helper
   CTC mode
   -------------------------------------------------------------------- */
void initTimer1(void) {
    // Clear control registers
    TCCR1A = 0x00;
    TCCR1B = 0x00;

    // CTC mode: WGM13:0 = 0100
    TCCR1B |= (1 << WGM12);

    // Prescaler = 8
    // 16 MHz / 8 = 2 MHz
    // Tick = 0.5 us
    TCCR1B |= (1 << CS11);

    // 1 us = 2 ticks
    // Count 0 to 1
    OCR1A = 1;

    // Clear counter
    TCNT1 = 0;

    // Clear compare flag
    TIFR1 |= (1 << OCF1A);
}

void delayUs(unsigned int delay) {
    unsigned int i;

    for (i = 0; i < delay; i++) {
        TCNT1 = 0;                 // reset counter
        TIFR1 |= (1 << OCF1A);     // clear compare flag

        while (!(TIFR1 & (1 << OCF1A))) {
            ;
        }
    }
}

void delaySeconds(unsigned int seconds) {
    unsigned int i;
    for (i = 0; i < seconds; i++) {
        delayMs(1000);
    }
}

/* --------------------------------------------------------------------
   TIMER 3
   Used for PWM motor control
   Fast PWM, 10-bit
   Output on OC3A
   -------------------------------------------------------------------- */
void initTimer3(void) {
    // Set OC3A pin as output
    // Arduino Mega pin 5 = PE3 = OC3A
    DDRE |= (1 << DDE3);

    // Clear control registers
    TCCR3A = 0x00;
    TCCR3B = 0x00;

    // Fast PWM 10-bit
    // WGM33:0 = 0111
    TCCR3A |= (1 << WGM30) | (1 << WGM31);
    TCCR3B |= (1 << WGM32);

    // Non-inverting mode on OC3A
    // Clear on compare match, set at BOTTOM
    TCCR3A |= (1 << COM3A1);

    // Prescaler = 1
    TCCR3B |= (1 << CS30);

    // Start with 0 duty cycle
    OCR3A = 0;
}

/* dutyCycle should be 0 to 1023 */
void setMotorSpeed(unsigned int dutyCycle) {
    if (dutyCycle > 1023) {
        dutyCycle = 1023;
    }

    OCR3A = dutyCycle;
}