// Author:Dylan, John, Josh
// Net ID: dylancorrea13
// Date: 5/4/2026
// Assignment:     Final Project
//
// Description: This file contains a programmatic overall description of the
// program. It should never contain assignments to special function registers
// for the exception key one-line code such as checking the state of the pin.
//
// Requirements:
//----------------------------------------------------------------------//

#include <avr/io.h>
#include "lcd.h"
#include "timer.h"
#include "spi.h"
#include "math.h"
#include <avr/interrupt.h>
#include "Arduino.h"


#define TRIG_PIN PH4
#define INT_PIN  PH3
#define EN_PIN   PH5

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
    DDRH &= ~(1 << TRIG_PIN);
    DDRH &= ~(1 << INT_PIN);

    PORTH |= (1 << TRIG_PIN);
    PORTH |= (1 << INT_PIN);

    DDRH |= (1 << EN_PIN);
// TDC sequence make enable low then high for board init
    PORTH &= ~(1 << EN_PIN);
    delayMs(2);
    PORTH |= (1 << EN_PIN);
}

void initTDCRegisters(void)
{
//setup CONFIG1 register
    writeRegister(0x00, 0x8A);
        //test registers
        // Serial.println(readRegister(0x00,1), HEX);
   //setup CONFIG2 register
   writeRegister(0x01, 0x41); // sets caibration periods and number of stops
   //setup Interrupt status register
   //writeRegister(0x02, 0x00);
   //setup Interrupt MASK register
   writeRegister(0x03, 0x07);
   //setup coarse counter overflow_H register
   writeRegister(0x04, 0xFF);
   //setup coarse counter overflow_L register
   writeRegister(0x05, 0xFF);
   //setup clock counter overflow_H register
   writeRegister(0x06, 0xFF);
   //setup clock counter overflow_L register
   writeRegister(0x07, 0xFF);
   //setup clock counter stop mask_H register
   writeRegister(0x08, 0x00);
   //setup clock counter stop mask_L register
   writeRegister(0x09, 0x00);
}


int main(void)
{
    // ------------------ initiallizing ----------------- //
  initTimer1();
  initTimer0();
  initLCD();
  sei(); // Enable global interrupts.

//monitor stuff
  Serial.begin(9600);  

  moveCursor(0, 0); // moves the cursor to 0,0 position
  writeString("Initializing");
  moveCursor(1, 0);  // moves the cursor to 1,0 position
  writeString("Please wait");


    TDC_State state = TDC_INIT;

    unsigned int TIME1 = 0;
    unsigned int TIME2 = 0;
    unsigned int CLOCK1 = 0;
    unsigned int CALIB1 = 0;
    unsigned int CALIB2 = 0;

    int INT_STATUS = 0;
    int CLK_OVF = 0;
    int CNT_OVF = 0;
    int MEAS_COM = 0;

    float TOF1 = 0;
    double CAL_COUNT = 0;
    float NORM_LSB = 0;

    int elapsed = 0;

    while (1)
    {
        switch (state)
        {
            case TDC_INIT:
                initSPI();
                initTDCGPIO();
                delayMs(500);
                initTDCRegisters();
                moveCursor(0,0);
                writeString("Finished Init");

                state = TDC_START_MEASUREMENT;
                break;

            case TDC_START_MEASUREMENT:
                TIME1 = 0;
                TIME2 = 0;
                CLOCK1 = 0;
                CALIB1 = 0;
                CALIB2 = 0;

                INT_STATUS = 0;
                CLK_OVF = 0;
                CNT_OVF = 0;
                MEAS_COM = 0;

                TOF1 = 0;
                CAL_COUNT = 0;
                NORM_LSB = 0;


                writeRegister(0x00, 0x83);
                writeRegister(0x01, 0x44);
                Serial.println("wait for triger");

                state = TDC_WAIT_TRIG_HIGH;
                break;

            case TDC_WAIT_TRIG_HIGH:
                if (PINH & (1 << TRIG_PIN)) {
                    state = TDC_WAIT_INT_LOW;
                } else {
                // Some pulses are missed and the state machine gets stuck
                   delayMs(1);
                   elapsed+=1;
                   if(elapsed > 50 )
                   state = TDC_CLEAR_FLAGS; 
                }
                break;


            case TDC_WAIT_INT_LOW:
                if (!(PINH & (1 << INT_PIN))) {
                    state = TDC_READ_RESULTS;
                } else {
                //Testing
                //    Serial.println("waiting int pin");
                }
                break;

            case TDC_READ_RESULTS:
                TIME1  = readRegister(0x10, 3);
                CLOCK1 = readRegister(0x11, 3);

                TIME2  = readRegister(0x12, 3);

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

                    TOF1 = (NORM_LSB * (TIME1 - TIME2)) + (CLOCK1 * CLK_PERIOD);

                    /* ---------------------- LCD Output -------------------*/
                    char buffer[16];
                    dtostrf(TOF1 * 1e3, 8, 3, buffer);  // convert to microseconds

                    moveCursor(0,0);
                    writeString("TDC Output:         ");
                    moveCursor(1,0);
                    writeString(buffer);
                    writeString(" ms");
                    // ------------------- test output --------------- //
                    Serial.println(TOF1*10000);
                    // Serial.println(NORM_LSB) ;
                    // Serial.println(CAL_COUNT);
                    // Serial.print("TIME1 = "); Serial.println(TIME1);
                    // Serial.print("TIME2 = "); Serial.println(TIME2);
                    // Serial.print("CLOCK1 = "); Serial.println(CLOCK1);

                    (void)TOF1;
                }

                state = TDC_CLEAR_FLAGS;
                break;

            case TDC_CLEAR_FLAGS:
                writeRegister(0x02, 0x1F);
                state = TDC_DELAY;
                break;

            case TDC_DELAY:
                delayMs(100);
                state = TDC_START_MEASUREMENT;
                break;

            default:
                state = TDC_INIT;
                break;
        }
    }

    return 0;
}