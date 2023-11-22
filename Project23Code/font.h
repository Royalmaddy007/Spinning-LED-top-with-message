/* *******************************************************************
 *
 *         Font for the LED-spin (original LED-Kreisel)
 *                     with two rows of 8 LEDs
 *                          for ATmega8
 *          This source code compiles with avr-gcc (WinAVR)
 *


 ****************************************************************** */

/* ===================================================================
 *  Font Define
 * ================================================================ */
#define LETTER_HEIGHT 7
#define LETTER_WIDTH  5
#define NUMBER_OF_LETTERS 26
#define NUMBER_OF_NUMBERS 10

/* ===================================================================
 *  Public Declaration
 * ================================================================ */
extern const unsigned char fontset[];

