



#ifndef PWM_H
#define PWM_H

void initPWMTimer3();
void changeDutyCycle(unsigned int adcValue);
void setMotorClockwise();
void setMotorCounterClockwise();
void stopMotor();
void initMotorDirectionPins();

#endif