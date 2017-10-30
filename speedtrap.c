/********************************************
 *
 *  Name:
 *  Section:
 *  Assignment: Final Lab- Speed Trap
 *
 ********************************************/


#include "speedtrap.h"

// volatile global variables
volatile unsigned char bits, a, b;
volatile unsigned char new_state, old_state;
volatile int count = 0;		// Count to display
volatile int old_count = 0; // old count
volatile unsigned char dataValid = 0; // 1 means valid, @ and $ have been received, 0 means invalid
volatile unsigned int vtens = 0, vones = 0, vtenths = 0; 
volatile char buffer[9]; 
volatile unsigned int numChar = 0; // num of data received
volatile unsigned char dataStart = 0; // 1 means @ has been received, 

// Global Variables
unsigned char tens = 0, ones = 0, tenths = 0;
unsigned int timems = 0;
unsigned int speedlimit = 40;   // default speed limit
char ostr[15]; 

int main(void) {

	// Variables go here
	int i = 0; // index used in loops
	// right phototransistor PB4
	// left phototransistor PB3
	unsigned short speed = 0; 
	char displayspeed[15]; 
	unsigned int speedtenths = 0; 
	unsigned int speedones = 0; 
	unsigned int speedtens = 0; 
	unsigned int speedtar = 0; 
	unsigned int testCount = 0; 
	
	init_lcd(); 
	
	// get last speed limit from previous run
	testCount = eeprom_read_byte((void *) 200); 
	if((testCount > 0) && (testCount < 100))
	{
		count = eeprom_read_byte((void *) 200); 
	}
	else // set a default count
	{
		count = 40; 
	}
	
	// enable receive interrupts
	UCSR0B |= (1 << RXCIE0); 
	
	portnDDRSetup(); 	
	// clock setup
	boringClockStuff(); 
	// enable interrupts
	sei(); 
	splashscreen();
	setuptimer(); 
	rotaryEncoder(); 

	bits = PINC;
    a = bits & (1 << PC4);
    b = bits & (1 << PC5);
    if (!b && !a)
	old_state = 0;
    else if (!b && a)
	old_state = 1;
    else if (b && !a)
	old_state = 2;
    else
	old_state = 3;
    new_state = old_state;

	// endless loop that runs throughout program
	while(1){
		
	// receive data
		if(dataValid == 1)
		{
			// prepare to output data on the screen
			vtens = buffer[0] - '0'; 
			vones = buffer[1] - '0'; 
			vtenths = buffer[2] - '0'; 
			moveto(1,9); 
			sscanf(buffer, "V2:%d%d.%d", &vtens, &vones, &vtenths); 
			snprintf(ostr, 15, "V2:%d%d.%d", vtens, vones, vtenths);
			stringout(ostr); 
		}
	// rotary encoder
		if(old_count != count) { // Did state change?

            snprintf(ostr, 15, "   LIM:%2d", count);
            moveto(0, 7);
            stringout(ostr);    // Print value of count
            old_count = count; 
            speedlimit = old_count; 
            eeprom_update_byte((void *) 200, speedlimit); 
            
        }
		if((PINB &= (1 << PB3)) == 0) { // right photo transistor is blocked
			timems = 0;
			PORTB |= ( 1 << PB5);       // turn on light
			TIMSK1 |= (1 << OCIE1A); 	// enable time interrupt to start timer

		}
		if((PINB &= (1 << PB4)) == 0) 
		{ // left photo transistor is blocked
			PORTB &= ~( 1 << PB5); 
			TIMSK1 &= ~(1 << OCIE1A); 	// Disable time interrupt to stop counting time
			
			// calculate speed 
// 			time = tenths + (10 * ones); 
			
			speed = 48260 / timems;  // speed is in mm/s

			speedtens = speed / 100; 
			speedtar =  speed % 100; 
			speedones = speedtar / 10;
			speedtenths = speedtar % 10;
			
			
			writecommand(1);
			snprintf(ostr, 15, "   LIM:%2d", count);
            moveto(0, 7);
            stringout(ostr); 
            
			// display time
			char time[15];
			snprintf(time, 15, "t:%d", timems); 
			moveto(0, 0);
			stringout(time); 
			
			// display speed in cm/s
			moveto(1,0); 
			snprintf(displayspeed, 15, "V:%d%d.%d", speedtens, speedones, speedtenths); 
			stringout(displayspeed); 
			
			
			// set up array to send speed
			char speedarray[5];
			speedarray[0] = '@';
			speedarray[1] = speedtens + '0'; 
			speedarray[2] = speedones + '0'; 
			speedarray[3] = speedtenths + '0'; 
			speedarray[4] = '$';
			
			// send speed 
			for(i = 0; i < 5; i++) { 
				tx_char(speedarray[i]); 
			}

			speed = speed / 10;
			if(speedlimit < speed)
			{
				moveto(0,10); 
				stringout("  SLOW");   
				moveto(1,12);
				stringout("DOWN"); 
				for(i=0; i < 100; i++)  
				{  
					// generate an infinite sequence 1KHz cycles
					PORTC |= (1 << PC3); // turns PB4 on
					_delay_ms(2); 
					PORTC &= ~(1 << PC3); // turns it off 
					_delay_ms(2);
				}
				_delay_ms(3000); 
				i = 0; 
				moveto(0,10); 
				snprintf(ostr, 15, "LIM:%2d", count);
				stringout(ostr); 
				moveto(1,10);
				stringout("          "); 
			}
		}
		
	}
}
/*
	ISR for TIMER 1
*/
ISR(TIMER1_COMPA_vect) 
{
	timems++;  
}

// rotary encoder on PINC
ISR(PCINT1_vect) 
{
	bits = PINC;		// Read the two encoder input at the same time
	a = bits & (1 << PC4);
	b = bits & (1 << PC5);
	

	// State transitions
	if (old_state == 0) {
	    if (a) {
		new_state = 1;
		count++;
	    }
	    else if (b) {
		new_state = 2;
		count--;
	    }
	}
	else if (old_state == 1) {
	    if (!a) {
		new_state = 0;
		count--;
	    }
	    else if (b) {
		new_state = 3;
		count++;
	    }
	}
	else if (old_state == 2) {
	    if (a) {
		new_state = 3;
		count--;
	    }
	    else if (!b) {
		new_state = 0;
		count++;
	    }
	}
	else {   // old_state = 3
	    if (!a) {
		new_state = 2;
		count++;
	    }
	    else if (!b) {
		new_state = 1;
		count--;
	    }
	}
	if(count < 1){
		count = 1; 
	}
	if(count > 99){
		count = 99; 
	}
	
	old_state = new_state; 		
}

/*
SPLASH SCREEN
*/
void splashscreen()
{
	char splash[] = "EE109-SPEED TRAP"; 
	char splash2[] = "CHARLIE ANDRE"; 
	char splash3[15] = "t:        LIM:"; 
	writecommand(1); 
	stringout(splash);		// top row splash
	moveto(1,0);			// go to bottom row
	stringout(splash2); 	// bottom row splash
	_delay_ms(2000);		// keep splash for 2 seconds
	writecommand(1); 		// clear screen again
	stringout(splash3); 	// 00.0
}

/*
timer set up
*/
void setuptimer() {
	TCCR1B |= (1 << WGM12); 	// sets to compare mode
	TIMSK1 &= ~(1 << OCIE1A); 	// DO NOT enable time interrupt
	OCR1A = 2000;	  			// timer module
	TCCR1B |= (1 << CS11);		// prescalar = 256
}

/*
Ports and DDR Setup
*/
void portnDDRSetup() {
	// D0 receive as input - receive
	DDRD &= ~(1 << PD0); 
	// D1 as output - send
	DDRD |= (1 << PD1); 
	// D2 as output - tri state
	DDRD |= (1 << PD2); 
	PORTD &= ~(1 << PD2);
	// set PB5, PC3 as outputs
	DDRB |= (1 << PB5); 
	DDRC |= (1 << PC3); 
	// Enable pull-ups on PC4, PC5
    PORTC |= (1 << PC5 | 1 << PC4); 

}

/*
	RECEIVE DATA
*/
ISR(USART_RX_vect)
{
	char x = 0;
	x = UDR0;

	if(x == '@')
	{
		dataStart = 1; // data has started to flow 
		dataValid = 0; // data is incomplete
		int index = 0;
		numChar = 0; 	// num of data received cleared to 0
		for(index = 0; index < 4; index++)
		{
			buffer[index] = '0';
		}
		buffer[4] = '\0';
	}
	else if(x == '$')
	{
		dataStart = 0; 
		if(numChar != 0) 
		{
			dataValid = 1; // data is valid, has been started w a @ and ended w a $
		}
		else if(numChar == 0)
		{
			dataValid = 0; // data invalid, no speed has been received
		}
		dataValid = 1; 
	}
	if((dataStart == 1) && (x >= '0') && (x <= '9'))  // data started flowing, num received
	{
		buffer[numChar] = x; 
		numChar++; // increment counter
	}
	if(((x < '0' ) || (x > '9')) && (x != '$') && (x != '@')) // invalid data
	{
		dataStart = 0; 
		dataValid = 0; 
		int index = 0;
		numChar = 0; 	// num of data received cleared to 0
		
		// clear contents of data received 
		for(index = 0; index < 4; index++)
		{
			buffer[index] = '0';
		}
		
	}	
}




