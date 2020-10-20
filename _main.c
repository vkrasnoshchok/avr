
#include <avr/pgmspace.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "UART.h"
//#include "TWI.h"
#include "config.h"

#define TIMER_START()       ( TCCR1B |= (1 << CS10) | (1 << CS12) ) /* clk I/O /1024 (from prescaler) */
//#define TIMER_START()       ( TCCR1B |= (1 << CS12) )               /* clk I/O /256  (from prescaler) */
//#define TIMER_START()       ( TCCR1B |= (1 << CS10) | (1 << CS11) ) /* clk I/O /64 (from prescaler) */
//#define TIMER_START()       ( TCCR1B |= (1 << CS11) )               /* clk I/O /8 (from prescaler) */
// #define TIMER_START()       ( TCCR1B |= (1 << CS10) )                 /* clk I/O /1 (no prescaling) */

#define TIMER_STOP()        ( TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12)) )


void uart_putchar(char c, FILE * stream)
{
    uart_send_byte(c);
}

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void change_pwm_cnt(void);

uint8_t  val = 0;
int step = -10;

uint16_t  val_1 = 0;
int step_1 = -10;

// void enable_int0(void);
// void disable_int0(void);

// // interrupt handler for button
// ISR(INT0_vect)
// {
//     disable_int0();
//     // incr/decr led value
//     change_pwm_cnt();
//     // toggle the led
//  PORTB ^= (1 << PB5);
//     timer_register(TIMER_2, 1, enable_int0);
// }   

// void enable_int0(void)
// {
//     // clear int flag
//     EIFR |= (1 << INTF0);
//     // enable int0
//     EIMSK |= (1 << INT0);
// } 

// void disable_int0(void)
// {
//     EIMSK &= ~(1 << INT0);
// } 



void rand_change_pwm_cnt(void)
{
    OCR0A = rand() % 255;
    // timer_register(TIMER_1, 10, rand_change_pwm_cnt);
}


void change_pwm_cnt(void)
{
    OCR0A = val;
    if ((OCR0A >= 250) || (OCR0A <= 0))
    {
        step *= -1;
    }
    if ((val <= 70) && (step == -10))
    {
        step = -9;
    }
    if ((val <= 50) && (step == -9))
    {
        step = -7;
    }
    if ((val <= 30) && (step == -7))
    {
        step = -5;
    }
    if ((val <= 20) && (step == -5))
    {
        step = -3;
    }
    if ((val <= 7) && (step == -3))
    {
        step = -1;
    }

    if ((val >= 7) && (step == 1))
    {
        step = 3;
    }
    if ((val >= 20) && (step == 3))
    {
        step = 5;
    }
    if ((val >= 30) && (step == 5))
    {
        step = 7;
    }
    if ((val >= 50) && (step == 7))
    {
        step = 9;
    }
    if ((val >= 70) && (step == 9))
    {
        step = 10;
    }
    val += step;
}


void change_pwm_cnt_1(void)
{
    TCNT1 = val_1;
    if ((TCNT1 >= 65530) || (TCNT1 <= 0))
    {
        step_1 *= -1;
    }
    val_1 += step_1;
}

void int0_init(void)
{
    // set PORTB pin 5 for output (led)
    // DDRB |= 1 << PB5;
    // PORTB &= ~(1 << PB5);
    // // config INT0 interrupt "rising edge"
    // EICRA |= (1 << ISC01);
    // EICRA &= ~(1 << ISC00);
    // enable global interrupts
    SREG |= (1 << 7);
    // // set PD2 (INT0) to input
    // DDRD &= ~(1 << PD2);
    // // connect internal resistor
    // PORTD |= (1 << PD2);
    // // External Interrupt Request Enable
    // EIMSK |= (1 << INT0);
}


void set_pwm_to_pd6(void)
{
    // set PWM (enable)
    TCCR0A |= (1 << COM0A1);
    TCCR0A &= ~(1 << COM0A0);
    // choice PWM mode (fast PWM)
    TCCR0B &= ~(1 << WGM02);
    TCCR0A |= (1 << WGM00);
    TCCR0A |= (1 << WGM01);
    // set prescaling (clk/8)
    TCCR0B |= (1 << CS01);
    TCCR0B &= ~(1 << CS00);
    TCCR0B &= ~(1 << CS02);
    // set compare bits (max = 255)
    OCR0A = 123;
    // set pin OC0A (PD6) to output
    DDRD |= (1 << PD6);
}


// Timer/Counter (TCNT1), Output Compare Registers (OCRA/B)
// The TCNT1, OCR1A/B, and ICR1 are 16-bit registers
// ICR1 or OCR1A
// TCCR1A

void set_tc1_pwm(void)
{
    // SET: Clear OC1A on Compare Match, set OC1A at BOTTOM 
    TCCR1A &= ~(1 << COM1A0);
    TCCR1A |= (1 << COM1A1);
    // SET: Fast PWM (OCR1A)
    TCCR1A |= (1 << WGM10);
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << WGM13);
    //SET: freq prescaler
    TIMER_START();
    TCNT1L = 16;
    TCNT1H = 16;
    // set pin OC1A (PB1) to output
    DDRB |= (1 << PB1);
}



void enable_interrupt_0(void)
{
    // mask interrupt for PCINT0(PB0)
    PCMSK0 |= 1<<PCINT0;
    // enable pullup
    // PORTB |= 1<<PB0;
    // Pin Change Interrupt Enable 0
    PCICR |= 1<<PCIE0;


}

// ISR(PCINT0_vect)
// {
//     if (PINB & (1<<PB0))
//     {
//         // timer_register(TIMER_4, 1, write_motion);
//         printf("1\r\n");
//         PORTB |= (1<<PB1);
//         inc_intensity();
//     }
//     else
//     {
//         PORTB &= ~(1<<PB1);
//         printf("0\r\n");
//         dec_intensity();
//     }
// }   

void print_text(void)
{
    // printf("%d\r\n", TCNT1);
    // printf("%d\r\n", TCNT1L);
    // printf("%d\r\n", TCNT1H);
    printf("%s\r\n", "test");
    printf("TCNT1: %d\r\n", TCNT1);
    printf("TCNT1L: %d\r\n", TCNT1L);
    printf("TCNT1H: %d\r\n", TCNT1H);
}





int main (void)
{

    // // set pin 9 (PB1) to output
    // DDRB |= (1 << PB1);

    // enable_interrupt_0();

    stdout = &uart_str;
    set_pwm_to_pd6();
    set_tc1_pwm();
    // int0_init();
    // PORTB |= (1 << PB5);

    uart_init();
    asm("sei");
    timer_init();

    TCNT1 = 22;

    timer_register(TIMER_1, 100, print_text);
    timer_register(TIMER_2, 1, change_pwm_cnt);
    timer_register(TIMER_3, 1, change_pwm_cnt_1);



    // OCR0A = 0;
    // int inc = 1;
    // // set_time(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dow, uint8_t date, uint8_t month, uint8_t year);
    // set_time(10, 8, 0, 5, 10, 4, 20);

    // timer_register(TIMER_1, 20, set_freq_timer);

    // timer_register(TIMER_1, 10, rand_change_pwm_cnt);
    // timer_register(TIMER_2, 10, write_time_to_buff);
    // timer_register(TIMER_3, 10, ds3231_read);



    // write_time_to_buff();
    // max7219_update();

    /* MAIN LOOP */
    for (;;) {
        timer_handler();
        // ds3231_handler(send_time);
        // max7219_handler();
    }

    return 0;
}