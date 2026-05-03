// Description: This file implements the initialization of an external
// switch.
//----------------------------------------------------------------------//

#include<avr/io.h>
void switch_init(){
    DDRD &= ~(1 << DDD3);     // PD3 input
    PORTD |= (1 << PORTD3);   // enable pull-up resistor

    EICRA |= (1 << ISC30);    // any logical change on INT0
    EICRA &= ~(1 << ISC31);

    EIMSK |= (1 << INT3);     // enable external interrupt INT0
}
