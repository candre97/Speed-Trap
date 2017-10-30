#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
//#include "speedtrap.h"

void init_lcd(); 
void writecommand(unsigned char cmd);
void writedata(unsigned char dat); 
void moveto(unsigned char row, unsigned char col); 
void stringout(char *str);