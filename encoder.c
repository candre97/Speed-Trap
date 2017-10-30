/********************************************
*
*  Name: Charles Andre
*  Section: Lab Weds 1:00pm
*  Assignment: Speedtrap - encoder.c

********************************************/


#include "encoder.h"


/*
Rotary Encoder
*/
void rotaryEncoder() {
	// Rotary encoder
	// Set the PCIE1 bit in the PCICR register to enable the pin change interrupts on Port C (PCINT1).
    PCICR |= (1 << PCIE1); 
    // Set the bits in the PCMSK1 mask register to enable interrupts for the PC4 and PC5 I/O lines 
    // For PC4 and PC5, these are bits PCINT12 and PCINT13.
    PCMSK1 |= (1 << PCINT12);
    PCMSK1 |= (1 << PCINT13);
	// Determine the initial state

}