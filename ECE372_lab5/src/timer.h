#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

/* Timer 0 */
void initTimer0(void);
void delayMs(unsigned int delay);
void delay_3ms(void);

/* Timer 1 */
void initTimer1(void);
void delayUs(unsigned int delay);
void delaySeconds(unsigned int seconds);

/* Timer 3 PWM */
void initTimer3(void);
void setMotorSpeed(unsigned int dutyCycle);

#endif