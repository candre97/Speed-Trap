#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
//#include "speedtrap.h"

void boringClockStuff(); 
char rx_char(); 
void tx_char(char ch); 