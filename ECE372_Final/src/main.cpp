// Author:
// Net ID:
// Date:
// Assignment:     Final Project
//
// Description: This file contains a programmatic overall description of the
// program. It should never contain assignments to special function registers
// for the exception key one-line code such as checking the state of the pin.
//
// Requirements:
//----------------------------------------------------------------------//

// ni hao sluts



#include <avr/io.h>
#include "switch.h"
#include "timer.h"
#include "pwm.h"
#include "spi.h"
#include "math.h"
#include <avr/interrupt.h>
#include "Arduino.h"


#define TRIG_PIN PD7
#define INT_PIN  PD6
#define STR_PIN  PD5
#define EN_PIN   PD4

#define CLK_PERIOD     125e-9
#define CALIB_PERIODS  10

typedef enum {
    TDC_INIT = 0,
    TDC_START_MEASUREMENT,
    TDC_WAIT_TRIG_HIGH,
    TDC_SEND_PULSES,
    TDC_WAIT_INT_LOW,
    TDC_READ_RESULTS,
    TDC_COMPUTE_TOF,
    TDC_CLEAR_FLAGS,
    TDC_DELAY
} TDC_State;

void initTDCGPIO(void)
{
    DDRD &= ~(1 << TRIG_PIN);
    DDRD &= ~(1 << INT_PIN);

    DDRD |= (1 << STR_PIN);
    DDRD |= (1 << EN_PIN);

    PORTD &= ~(1 << STR_PIN);

    PORTD &= ~(1 << EN_PIN);
    delayMs(2);
    PORTD |= (1 << EN_PIN);
}

void initTDCRegisters(void)
{
    writeRegister(0x00, 0x8A);
    writeRegister(0x01, 0x41);
    writeRegister(0x03, 0x07);
    writeRegister(0x04, 0xFF);
    writeRegister(0x05, 0xFF);
    writeRegister(0x06, 0xFF);
    writeRegister(0x07, 0xFF);
    writeRegister(0x08, 0x00);
    writeRegister(0x09, 0x00);
}

void sendOnePulse(void)
{
    PORTD |= (1 << STR_PIN);
    delayUs(1);
    PORTD &= ~(1 << STR_PIN);
}

int main(void)
{
    TDC_State state = TDC_INIT;

    unsigned int TIME1 = 0;
    unsigned int TIME2 = 0;
    unsigned int CLOCK1 = 0;
    unsigned int CLOCK2 = 0;
    unsigned int CALIB1 = 0;
    unsigned int CALIB2 = 0;

    int INT_STATUS = 0;
    int CLK_OVF = 0;
    int CNT_OVF = 0;
    int MEAS_COM = 0;

    double TOF1 = 0;
    double CAL_COUNT = 0;
    double NORM_LSB = 0;

    int pulseCount = 0;

    while (1)
    {
        switch (state)
        {
            case TDC_INIT:
                initSPI();
                initTDCGPIO();
                delayMs(500);
                initTDCRegisters();

                state = TDC_START_MEASUREMENT;
                break;

            case TDC_START_MEASUREMENT:
                TIME1 = 0;
                TIME2 = 0;
                CLOCK1 = 0;
                CLOCK2 = 0;
                CALIB1 = 0;
                CALIB2 = 0;

                INT_STATUS = 0;
                CLK_OVF = 0;
                CNT_OVF = 0;
                MEAS_COM = 0;

                TOF1 = 0;
                CAL_COUNT = 0;
                NORM_LSB = 0;

                pulseCount = 0;

                writeRegister(0x00, 0x83);
                writeRegister(0x01, 0x44);

                state = TDC_WAIT_TRIG_HIGH;
                break;

            case TDC_WAIT_TRIG_HIGH:
                if (PIND & (1 << TRIG_PIN)) {
                    state = TDC_SEND_PULSES;
                } else {
                    /*
                    WAITING FOR TRIG HIGH
                    ADD TIMEOUT HERE IF NEEDED
                    THIS REPLACES SERIAL DEBUG PRINTS
                    */
                }
                break;

            case TDC_SEND_PULSES:
                if (pulseCount < 6) {
                    sendOnePulse();
                    pulseCount++;

                    if (pulseCount == 1 || pulseCount == 4) {
                        delayMs(2);
                    } else {
                        delayMs(1);
                    }
                } else {
                    state = TDC_WAIT_INT_LOW;
                }
                break;

            case TDC_WAIT_INT_LOW:
                if (!(PIND & (1 << INT_PIN))) {
                    state = TDC_READ_RESULTS;
                } else {
                    /*
                    WAITING FOR INT LOW
                    MEASUREMENT NOT COMPLETE YET
                    THIS REPLACES SERIAL DEBUG PRINTS
                    */
                }
                break;

            case TDC_READ_RESULTS:
                TIME1  = readRegister(0x10, 3);
                CLOCK1 = readRegister(0x11, 3);

                TIME2  = readRegister(0x12, 3);
                CLOCK2 = readRegister(0x13, 3);

                CALIB1 = readRegister(0x1B, 3);
                CALIB2 = readRegister(0x1C, 3);

                INT_STATUS = readRegister(0x02, 1);

                CLK_OVF  = (INT_STATUS >> 2) & 1;
                CNT_OVF  = (INT_STATUS >> 1) & 1;
                MEAS_COM = (INT_STATUS >> 0) & 1;

                state = TDC_COMPUTE_TOF;
                break;

            case TDC_COMPUTE_TOF:
                if (MEAS_COM == 1 && CLK_OVF == 0 && CNT_OVF == 0) {
                    CAL_COUNT = (double)(CALIB2 - CALIB1) / (CALIB_PERIODS - 1);
                    NORM_LSB = CLK_PERIOD / CAL_COUNT;

                    TOF1 = (NORM_LSB * (TIME1 - TIME2)) +
                           (CLOCK1 * CLK_PERIOD);

                    /*
                    PRINT TOF1 VALUE HERE
                    USE UART OR LCD OUTPUT
                    THIS REPLACES SERIAL.PRINT
                    */

                    (void)TOF1;
                }

                state = TDC_CLEAR_FLAGS;
                break;

            case TDC_CLEAR_FLAGS:
                writeRegister(0x02, 0x1F);
                state = TDC_DELAY;
                break;

            case TDC_DELAY:
                delayMs(500);
                state = TDC_START_MEASUREMENT;
                break;

            default:
                state = TDC_INIT;
                break;
        }
    }

    return 0;
}