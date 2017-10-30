/********************************************
*
*  Name: Charles Andre
*  Section: Lab Weds 1:00pm
*  Assignment: Speedtrap - lcd.c

********************************************/


#include "lcd.h"


/*
  init_lcd - Configure the I/O ports and send the initialization commands
*/
void init_lcd()
{
	
    /* ??? */                   // Set the DDR register bits for ports B and D
    DDRD |= (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7); // sets all of these to outputs
    DDRB |= (1 << PB0) | (1 << PB1); // sets these bad boys to outputs
    
    _delay_ms(16);              // Delay at least 15ms

    /* ??? */                   // Use writenibble to send 0011
    unsigned char sendIt = 0b00110000; 
    writenibble(sendIt); // woohoo sent!
    
    
    _delay_ms(5);               // Delay at least 4msec

    /* ??? */                   // Use writenibble to send 0011
    sendIt = 0b00110000; 
    writenibble(sendIt); // woohoo sent!
    
    _delay_us(120);             // Delay at least 100usec

    /* ??? */                   // Use writenibble to send 0011, no delay needed
    
    sendIt = 0b00110000; 
    writenibble(sendIt); // woohoo sent!

    /* ??? */                   // Use writenibble to send 0010
    sendIt = 0b00100000; 
    writenibble(sendIt); // woohoo sent!
    
    _delay_ms(3);               // Delay at least 2ms
    
    writecommand(0x28);         // Function Set: 4-bit interface, 2 lines
    _delay_ms(2);

    writecommand(0x0f);         // Display and cursor on
    _delay_ms(2);
    
    writecommand(1);

}

/*
  moveto - Move the cursor to the row (1 to 2) and column (1 to 16) specified
*/
void moveto(unsigned char row, unsigned char col)
{
	unsigned char target = 0x00; // target
	
	// top row
	if(row == 0){
		target = 0x00;
		}
	// bottom row
	else if(row == 1) {
		target = 0x40; 
		}
    
    target = target + col; 
    
    target = target + 0x80; 
    
    writecommand(target); 
}

/*
  stringout - Write the string pointed to by "str" at the current position
*/
void stringout(char *str)
{
	int i = 0;
    while (str[i] != '\0')
    {
    	writedata(str[i]); 
    	i++; 
    }
}

/*
  writecommand - Send the 8-bit byte "cmd" to the LCD command register
*/
void writecommand(unsigned char cmd)  // commands, cursor movement
{
	// clear RS bit to zero
	PORTB &= ~(1 << PB0);
	
	unsigned char target = cmd;
	writenibble(target);
	
	target = (cmd << 4); // clear 4 to 7, store 0 to 3 in target
	writenibble(target); 
	_delay_ms(2);

}

/*
  writedata - Send the 8-bit byte "dat" to the LCD data register
*/
void writedata(unsigned char dat)  // text
{
	// Set RS to 1
	PORTB |= (1 << PB0);
	
	unsigned char target = dat;
	writenibble(target); 
	
	target = (dat << 4); // clear 4 to 7, store 0 to 3 in target
	writenibble(target); 
	_delay_ms(2);
}

/*
  writenibble - Send four bits of the byte "lcdbits" to the LCD
*/
void writenibble(unsigned char lcdbits)
{
	PORTD &= 0x0F; 
	PORTD |= (lcdbits & 0xF0);
	
	
	// ZERO ONE ZERO
	
	PORTB &= ~(1 << PB1); // clear to zero
	PORTB |= (1 << PB1); // set to one
	PORTB |= (1 << PB1); // set to one again if needed
	PORTB &= ~(1 << PB1); // clear to zero


}

