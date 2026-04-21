// Description: This file implements the initialization of an external
// switch.
//----------------------------------------------------------------------//

#include<avr/io.h>
void switch_init(){
    DDRD &= ~(1 << DDD0);     // PD0 input
    PORTD |= (1 << PORTD0);   // enable pull-up resistor

    EICRA |= (1 << ISC00);    // any logical change on INT0
    EICRA &= ~(1 << ISC01);

    EIMSK |= (1 << INT0);     // enable external interrupt INT0
}
