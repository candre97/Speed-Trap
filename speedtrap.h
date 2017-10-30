#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include "encoder.h"
#include "serial.h"

void splashscreen(); 
void setuptimer();
void portnDDRSetup();
