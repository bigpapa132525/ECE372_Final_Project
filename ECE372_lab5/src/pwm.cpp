#include <avr/io.h>
#include "timer.h"
#include "pwm.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Timer3 -> OC3A -> PE3 on ATmega2560
// Generates a 50% duty cycle square wave for a passive piezo buzzer

void initPWMTimer3()
{
    DDRE |= (1 << DDE3);      // PE3 / OC3A as output

    // CTC mode, toggle OC3A on compare match
    // WGM33:0 = 0100  -> CTC with OCR3A as top
    // COM3A1:0 = 01   -> toggle OC3A on compare match
    // Prescaler = 8
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 << CS31);

    OCR3A = 0;
}

void buzzerOn(unsigned int frequency)
{
    unsigned long ocr;

    if (frequency == 0)
    {
        buzzerOff();
        return;
    }

    // f_out = F_CPU / (2 * N * (1 + OCR3A))
    // N = 8
    ocr = (F_CPU / (2UL * 8UL * frequency)) - 1;

    if (ocr > 65535UL)
    {
        ocr = 65535UL;
    }

    OCR3A = (unsigned int)ocr;
}

void buzzerOff()
{
    // disconnect OC3A so pin stops toggling
    TCCR3A &= ~(1 << COM3A0);
    PORTE &= ~(1 << PORTE3);
}

void buzzerEnable()
{
    // reconnect toggle output
    TCCR3A |= (1 << COM3A0);
}

void chirpUp()
{
    unsigned int f;
    buzzerEnable();

    for (f = 1000; f <= 3000; f += 100)
    {
        buzzerOn(f);
        delayMs(50);  // delay between frequency changes
    }
}

void chirpDown()
{
    int f;
    buzzerEnable();

    for (f = 3000; f >= 1000; f -= 100)
    {
        buzzerOn((unsigned int)f);
        delayMs(50);  // delay between frequency changes
    }
}