

#include <avr/io.h>
#include "pwm.h"

// function that produces an output PWM signal with a frequency and duty cycle. For this example
// I am generating a PWM frequency of 15.625 kHz with a 60% duty cycle.
// I will use Timer 1 in the fast PWM mode #7 in Table 17-2, where my TOP value is 0x3FF (1023)
// In this mode I have a frequency that is fixed by the top value. The only setting that
// I can change is the prescaler to lower the frequency to a specific value.
// Use Fast PWM Mode #14 if you need more flexibilty on the frequency value.

void initPWMTimer3()
{
    DDRE |= (1 << DDE3);   // OC3A output

    // Fast PWM 10-bit, non-inverting, prescaler = 1
    TCCR3A = (1 << COM3A1) | (1 << WGM31) | (1 << WGM30);
    TCCR3B = (1 << WGM32) | (1 << CS30);

    OCR3A = 0;
}



/* ------------------- Direction of Motor ------------------ */
// init pins 50, 51 (PB2 & PB3)
  void initMotorDirectionPins(void)
  {
      DDRB |= (1 << DDB2) | (1 << DDB3);  // outputs
  }

  void setMotorClockwise(void)
  {
      PORTB |= (1 << PORTB2);
      PORTB &= ~(1 << PORTB3);
  }

  void setMotorCounterClockwise(void)
  {
      PORTB &= ~(1 << PORTB2);
      PORTB |= (1 << PORTB3);
  }

  void stopMotor(void)
  {
      PORTB &= ~(1 << PORTB2);
      PORTB &= ~(1 << PORTB3);
  }
/* -------------- Potentiometer Adjustment ---------------- */
  void changeDutyCycle(unsigned int adcValue){

        if (adcValue > 524)
        {
            setMotorClockwise();
            OCR3A = ((adcValue - 524) * 1023UL) / (1023 - 524);
        }
        else if (adcValue < 500)
        {
            setMotorCounterClockwise();
            OCR3A = ((500 - adcValue) * 1023UL) / 500;
        }
        else
        {
            stopMotor();
            OCR3A = 0;
        }
    
}