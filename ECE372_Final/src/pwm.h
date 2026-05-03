#ifndef PWM_H
#define PWM_H

void initPWMTimer3();
void buzzerOn(unsigned int frequency);
void buzzerOff();
void buzzerEnable();
void chirpUp();
void chirpDown();

#endif