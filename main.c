#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "config.h"
#include "UART.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lcd-library.h"

//#define F_CPU 1000000UL

// const char Char10[] PROGMEM = {0b00000000,
//                          0b00000111,
//                          0b00011111,
//                          0b00011110,
//                          0b00001100,
//                          0b00001100,
//                          0b00011111,
//                          0b00000000};

// const char Char11[] PROGMEM = {0b00000000,
//                          0b00011100,
//                          0b00011111,
//                          0b00001111,
//                          0b00000110,
//                          0b00000110,
//                          0b00011111,
//                          0b00000000};

// const char Char12[] PROGMEM = {0b00001010,
//                          0b00001110,
//                          0b00000010,
//                          0b00000000,
//                          0b00001100,
//                          0b00000010,
//                          0b00000100,
//                          0b00001110};

const char stringOne[] PROGMEM = "  Hello";
const char stringTwo[] PROGMEM = " World!";
    
int main(void)
{
    char i = 1;
    char dir = LCD_SCROLL_LEFT;
    
    lcdInit();
    
    lcdLoadCharacterf(0, Char10);
    lcdLoadCharacterf(1, Char11);
    lcdLoadCharacterf(2, Char12);
    
    lcdGotoXY(0,0);
    lcdPutsf(stringOne);
    
    lcdGotoXY(1,0); 
    lcdPutsf(stringTwo);
    
    DDRB |= (1<<7);
    
    while(1)
    {
        
        // PORTD |= (1<<7);
        // _delay_ms(200);
        
        // PORTD &= ~(1<<7);
        // _delay_ms(100);
        // lcdDisplayScroll(1, dir);
        // if (i == 11){
        //     if (dir == LCD_SCROLL_LEFT){
        //         dir = LCD_SCROLL_RIGHT;
        //     }
        //     else{
        //         dir = LCD_SCROLL_LEFT;
        //     }               
        //     i = 0;
        // }
        // i++;

    }
}