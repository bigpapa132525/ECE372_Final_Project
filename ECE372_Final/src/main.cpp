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
#include "switch.h"
#include "timer.h"
#include "pwm.h"
#include "spi.h"
#include "I2C.h"
#include "math.h"
#include <avr/interrupt.h>
#include "Arduino.h"

// defines


// Initialize states.  Remember to use volatile 

/*
 * Define a set of states that can be used in the state machine using an enum.
 */
/* ------------------------- Definded things --------------------*/
// angle stuff
int16_t ax, ay, az;
//states stuff
bool alarmTriggered = false;
bool alarmSilenced = false;

volatile int counter=0;
volatile int time_base=10;

int wait = 0;
//Smile States
typedef enum SMILE
{
  SMILEY,
  FROWN,
} statetype;


//Button States
typedef enum stateType_enum {
wait_press, debounce_press,
wait_release, debounce_release
} stateType;

volatile statetype displaystate = SMILEY;
volatile stateType buttonstate = wait_press;


int main(){

// ---- initiallizing ---- //
/* -------- testing -----------*/
init();
Serial.begin(9600);
  // initialize your timer functions (needed for delays)
    initTimer0();
    initTimer1();
    initTimer3();
 
    // initialize buzzer PWM
    initPWMTimer3();
    buzzerOff();
 
    // initialize SPI for MAX7219
    initSPI();
    // Small delay to let MAX7219 stabilize
    delayMs(10);
    
    //init i2c
    initI2C();

    //init button
    switch_init();

    sei(); // Enable global interrupts.
    

// while loop
  while(1){
  /* --------------------------- obtain xyz coord roll -------------------*/
        ReadIMU(&ax, &ay, &az);

    // Now we calculate tilt
    float ax_g = ax / 16384.0;
    float ay_g = ay / 16384.0;
    float az_g = az / 16384.0;

    float pitch = atan2(ax_g, sqrt(ay_g * ay_g + az_g * az_g)) * (180.0 / 3.14159);
    float roll  = atan2(ay_g, sqrt(ax_g * ax_g + az_g * az_g)) * (180.0 / 3.14159);

    bool isTilted = (fabs(pitch) >= 45.0) || (fabs(roll) >= 45.0);

    Serial.print(ax);
    Serial.print(" ");
    Serial.print(ay);
    Serial.print(" ");
    Serial.print(az);
    Serial.print(" ");

    if (isTilted){
      Serial.println("Tilting");
      displaystate = FROWN; 
    }

    else{
      Serial.println("Stable");
      displaystate = SMILEY;
    }

  
  /* ------------------------------------- State Machine Smile ------------------------------------*/
    switch (displaystate) {
      case SMILEY:
        displaySmileyFace();
        alarmTriggered = false;
        alarmSilenced = false;
        buzzerOff();
        break;

      case FROWN:
        displayFrownyFace();
        alarmTriggered = true;
        break;

      default:
        Serial.println("In default LED state.");
        break;
    }


/* ------------------------------------- State Machine (button) ---------------------------------- */   
    switch(buttonstate){
          //wait for the press
          case wait_press:
            break;
          //wait 20 ms for the signal to stabalize before accepting new state
          case debounce_press:
            delayMs(20);
            buttonstate = wait_release;
            break;
          //wait for the button release
          case wait_release:
            break;
          
          //state that sets the timer to 200 if the timer was at 100
          case debounce_release:
            alarmSilenced = true;            
            buzzerOff();
            delayMs(20);
            buttonstate = wait_press;
            break;

          //state that sets the timer to 100 if the timer was at 200
        }
        if (alarmTriggered && !alarmSilenced){
          chirpUp();
          chirpDown(); 
          Serial.println("bruh") ;
        }
       
      }
  return 0;
}


ISR(INT3_vect){
    if(buttonstate == wait_press){
        buttonstate = debounce_press;
    }
    else if(buttonstate == wait_release){
        buttonstate = debounce_release;
    }
}