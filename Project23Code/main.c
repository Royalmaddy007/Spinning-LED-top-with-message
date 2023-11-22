
/*
CHAPTER - 5 
Project 23 - Spinning LED top with message display
*/

//Hardware Description
//8 LEDs on PORTA
//Interrupt on PB2(INT0)
//Direction on PB1
//PB0 not used
//PB3 is RESET

//Start of Program

//Header Files
#include<avr/io.h>
#define F_CPU 1000000UL
#include<util/delay.h>
#include<avr/interrupt.h>
#include<avr/pgmspace.h>
#include"font.h"





# define STRING_2 "Not War"
# define STRING_1 "MaKe?@" //In the file font.h the bits of ? and @ have been changed to display a heart



// Number of letters in text_xxx[], maximum is (COLUMN_NUMBER)/(2*6)
#define STRING_LENGTH1     9
#define STRING_LENGTH2     7
#define LED_ALL_ON  0xFF
#define LED_ALL_OFF 0x00

// TOP events
#define TOP_TURNING 1
#define TOP_STOPPED 0

// number of presentable crevices in
// one rotation (incl. spaces in letters!)
#define COLUMN_NUMBER 200

#define CLOCKWISE 1
#define ANTICLOCKWISE 2

// Column macros
enum COLUMNS
{
  COLUMN_1 = 0,
  COLUMN_2,
  COLUMN_3,
  COLUMN_4,
  COLUMN_5,
  COLUMN_6,
  

  ENDOF_COLUMNS
};


// Fast rotation ( fast times to make one round )
// minimum time of one rotation in ms (30 Hz = 33.33 ms)
#define time_round_min 33

// Slow rotation ( takes long time to make one round )
// maximum time of one rotation in ms (5 Hz = 200 ms)
#define time_round_max 200

const unsigned char text_clock[STRING_LENGTH1+1] PROGMEM = STRING_1;
const unsigned char text_anti[STRING_LENGTH2+1] PROGMEM = STRING_2;

// time of the last rotation
volatile unsigned char current_round_time = 0;

// counter for the time since interrupt
volatile unsigned char current_round_time_count = 0,i=0;

// currently displayed column
volatile unsigned int current_column = 0;
volatile unsigned char proxy=0;

// dynamic duration of a column in µs
volatile unsigned long timer1_startvalue = 600;
volatile unsigned char display_field_clock[100/*int(COLUMN_NUMBER/2)*/];

volatile unsigned char mode=0;
volatile unsigned char char_nr=0, column_nr=0,character=0,no_char=0;
volatile unsigned char majority_clock=0,majority_anti=0;
void init_interrupt (void);
unsigned char runing_condition (void);
void set_leds(unsigned char led);
void double_string_display (void);



int main(void)
{
	//PORT INIT
	DDRA = 0xFF;
	PORTA = 0xFF;
	
	init_interrupt();
	
	
	
	while(1)
	{	
		
		
		double_string_display();
		
		
		
	}	

}

void init_interrupt (void)
{
	// as tactful source for Timer1: CPU-Frequency 
	// (adapt to time of rotation, column group)
	// TCCR1B = Timer/Counter Control Register B
	TCCR1B= (1 << CS10);

	// as tactful source for Timer0: CPU_Takt/8 (aim: 1ms)
	// TCCR0 = Timer/Counter Control Register
	// ClkIO/8 from presacler for Timer 0
	TCCR0B = (1 << CS01);

	// Activate Timer0 and Timer1 as interrupt-source
	
	TIMSK0 = (1 << TOIE0);
	TIMSK1 = (1 << TOIE1);
 

	// engaging extern Interrupt0 (sensor-signal)
	GIMSK = (1 << INT0);

	// dissolving the interrupts in case of growing shoulder
	// MCUCR = MCU Control Register
	// configured for Power Save
	MCUCR = (1 << ISC01) | (1 << ISC00);

	// enable interrupts
	sei();
}

unsigned char runing_condition (void)
{
	// is the actual lap-time possible?
	if ((current_round_time > time_round_min) &&
		(current_round_time < time_round_max))
	{
		return(TOP_TURNING);
	}

	else
		return(TOP_STOPPED);
}

void set_leds(unsigned char led)
{
	
	proxy = 0;
	for(unsigned char j=0;j<=7;j++)
	{
		
		proxy |= ((led&(1<<(7-j)))&&1)<<j;
	}
	proxy = ~proxy;
	PORTA = proxy;
}


void construct_display_field (void)
{
	
	char_nr=0;column_nr=0;character=0;no_char=0;

	// Clear array
	for (column_nr = COLUMN_1; column_nr < 100; column_nr++)
	{
		display_field_clock[column_nr] = 0;
		
	}
	if(mode==CLOCKWISE)
	no_char = STRING_LENGTH1;
	else if(mode==ANTICLOCKWISE)
	no_char = STRING_LENGTH2;
	for (char_nr = 0; char_nr < no_char; char_nr++)//Neglect Null character
	{
		if(mode==CLOCKWISE)
		character = pgm_read_byte(&text_clock[char_nr]);
		else if(mode==ANTICLOCKWISE)
		character = pgm_read_byte(&text_anti[char_nr]);
		for (column_nr = COLUMN_1; column_nr < COLUMN_6; column_nr++)
		{

			//Column6 is the space between two letter
			
			if ((character<0x20)||(character>0x7f));//Do nothing because illegal character
			else
			display_field_clock[(char_nr*6 + column_nr)+1] = pgm_read_byte(&fontset[5*character-160+(column_nr)]);
			
		}
			if(character=='?')
			display_field_clock[(char_nr*6+5)+1]=0x3e;
			else if(character=='@')
			display_field_clock[(char_nr*6+5)+1]=0x06;
		
	}
	
}


void double_string_display (void)

{
	
	
	


	

	// Initialise display
	construct_display_field();

	// TOP runs
		while (runing_condition() == TOP_TURNING)
		{
			
			i = current_column/2;
			if(current_column%2==1)
			{
				set_leds(LED_ALL_OFF); 
			}
			else if(current_column%2==0)
			{
				if(mode==CLOCKWISE)
				{
					if(i<=(STRING_LENGTH1*6))
					set_leds((display_field_clock[i])&0x7F);
					else 
					set_leds(LED_ALL_OFF);
					
				}
				else if(mode==ANTICLOCKWISE)
				{
					if(((STRING_LENGTH2*6)-i)>=0)
					set_leds((display_field_clock[(STRING_LENGTH2*6)-i])&0x7F);
					else
					set_leds(LED_ALL_OFF);
				}
				
			}
			
			
			
			
		}

		// TOP does not run
		while(runing_condition() != TOP_TURNING)
		{
			// Disable all leds
			set_leds(LED_ALL_OFF);
		}
} /* double_string_display */



ISR (INT0_vect)
{
	majority_clock=0;majority_anti=0;
	// growing shoulder of the sensor
	current_round_time = current_round_time_count;
	unsigned char majority_clock=0,majority_anti=0;
	// counts the duration of the last round in ms
	// is start of new round realistic? (80% of the time of the last round)
	if (current_column > (COLUMN_NUMBER*8)/10)
	{
		// here: adopt lap time for new column-timing, Timer1 runs with 1MHz
		timer1_startvalue = 1000/COLUMN_NUMBER*current_round_time;
		current_column = 0;

		
	}
  
	// clear elapsed-time meter for the time in ms between two growing shoulders
	current_round_time_count = 0;
	for(unsigned char j=0;j<=2;j++)
	{
		if(PINB&(1<<1))
		{
			majority_anti++;
		}
		else if(!(PINB&(1<<1)))
		{
			majority_clock++;
		}
		_delay_us(30);
	}
	if(majority_clock>majority_anti)
	{
		mode = ANTICLOCKWISE;
	}
	else if(majority_anti>majority_clock)
	{
		mode = CLOCKWISE;
	}
	
}


ISR (TIM0_OVF_vect)
{
		// Set routine call every ms
		TCNT0 = 255 - 125;

		// Increase round counter (without Overflow)
		if (current_round_time_count < 255)
		current_round_time_count++;
		else
		current_round_time_count = 255;
}

ISR (TIM1_OVF_vect)
{
	// the calling-time is based upon the actual speed
	TCNT1H = 255 - (timer1_startvalue >> 8);

	TCNT1L = 255 - (timer1_startvalue & 255);

	// next column, or missed synchronisation-condition,
	// then new start: time-controlled
	if(current_column < COLUMN_NUMBER)
		current_column++;
	else
		current_column--;
}
