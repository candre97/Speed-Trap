/********************************************
*
*  Name: Charles Andre
*  Section: Lab Weds 1:00pm
*  Assignment: Speedtrap - serial.c

********************************************/

#include "serial.h"

/*
Clock Setup
*/
void boringClockStuff() {
	#define FOSC 16000000 // Clock frequency
	#define BAUD 9600 // Baud rate used
	#define MYUBRR (FOSC/16/BAUD-1) // Value for UBRR0
	UCSR0B |= (1 << TXEN0 | 1 << RXEN0); // Enable RX and TX
	UCSR0C = (3 << UCSZ00); // Async., no parity,
	// 1 stop bit, 8 data bits
	UBRR0 = MYUBRR; // Set baud rate
}

char rx_char()
{
// Wait for receive complete flag to go high
while ( !(UCSR0A & (1 << RXC0)) ) {}
return UDR0;
}

void tx_char(char ch)
{
// Wait for transmitter data register empty
while ((UCSR0A & (1<<UDRE0)) == 0) {}
UDR0 = ch;
}