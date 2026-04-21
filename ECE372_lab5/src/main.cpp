// Author:
// Net ID:
// Date:
// Assignment:     Lab 3
//
// Description: This file contains a programmatic overall description of the
// program. It should never contain assignments to special function registers
// for the exception key one-line code such as checking the state of the pin.
//
// Requirements:
//----------------------------------------------------------------------//

// ni hao sluts



#include <avr/io.h>
#include "led.h"
#include "switch.h"
#include "timer.h"
#include "lcd.h"
#include "pwm.h"
#include "adc.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include "Arduino.h"

// defines


/*
 * Define a set of states that can be used in the state machine using an enum.
 */
// typedef enum .......


// Initialize states.  Remember to use volatile 

/*
 * Define a set of states that can be used in the state machine using an enum.
 */


volatile int counter=0;
volatile int time_base=10;

// Sequence that the led's are going through
int number = 1;

// Boolian value wheather faster mode is on or not
int fast = 0;

// see if button is pressed
volatile int bttnPress = 0;

int wait = 0;

typedef enum stateType_enum {
wait_press, debounce_press,
wait_release, debounce_release
} stateType;

volatile stateType state = wait_press;


int main(){

// ---- initiallizing ---- //
  initTimer1();
  initTimer0();
  initPWMTimer3();
  switch_init();
  initADC();
  initSevSeg();
  initMotorDirectionPins();
  sei(); // Enable global interrupts.

  /* -------- testing -----------*/
  // Serial.begin(9600);

  /* ------------ NEW VARIALBES FOR THIS LAB4 HERE ------------*/
    int go = 0;
    unsigned int adcValue;

// while loop
  while(1){
    /* --------------- Check State: Button press ---------------- */
    if(go == 1){
    go = 0;
    cli();
    stopMotor();
    for(int i = 9; i >= 0; i--){
      numOut(i);
      delayMs(1000);
    }
    sei();
  }
    numOut(10);
    adcValue = ADC;
/* --------------------------- Motor Control ------------------------------ */    
    changeDutyCycle(adcValue);
/* ------------------------------------- State Machine ---------------------------------- */   
    switch(state){
          //wait for the press
          case wait_press:
            break;
          //wait 20 ms for the signal to stabalize before accepting new state
          case debounce_press:
            delayMs(20);
            state = wait_release;
            break;
          //wait for the button release
          case wait_release:
            break;
          
          //state that sets the timer to 200 if the timer was at 100
          case debounce_release:
            
            delayMs(20);
            go = 1;
            state = wait_press;
            break;

          //state that sets the timer to 100 if the timer was at 200
        }
        //delay between the lights counting up
       
      }
  return 0;
}


ISR(INT0_vect){
    if(state == wait_press){
        state = debounce_press;
    }
    else if(state == wait_release){
        state = debounce_release;
    }
}