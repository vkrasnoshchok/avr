#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "config.h"
#include "UART.h"

uint8_t pwm_count = 0x0;
uint8_t pwm_level = 10;
uint16_t counter = 0;
uint8_t i = 1;
uint8_t tmp_byte_tmp;


void uart_putchar(char c, FILE * stream)
{
    uart_send_byte(c);
}

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);



void pb0_to_out(void)
{
    // set PORTB pin 5 for output (led)
    DDRB |= 1 << PB0;
}


void prog_pwm_pb0(void)
{
    if (pwm_count > pwm_level)
    {
        PORTB &= ~(1 << PB0);
    }
    else
    {
        PORTB |= (1 << PB0);
    }

}

void toggle_led(void)
{
    // toggle the led
    PORTB ^= (1 << PB0);
}

void print_text(void)
{
    printf("\r\n%s\r\n", "test 10sec");
    // tmp
    // tmp_byte_tmp = eeprom_read_byte(1);
    // eeprom_write_byte(1, ++tmp_byte_tmp);
}


void print_eeprom_bytes(void)
{
    // uint8_t tmp_byte;
    // for (int i=0; i<5; i++)
    // {
    //     tmp_byte = eeprom_read_byte(i);
    //     printf("i[%d] - %d\r\n", i, tmp_byte);
    // }
    // printf("\r\n");
    // for (int i=1023; i<1028; i++)
    // {
    //     tmp_byte = eeprom_read_byte(i);
    //     printf("i[%d] - %d\r\n", i, tmp_byte);
    // }
    printf("123\r\n");
}



int main (void)
{
    // eeprom_write_byte(1, 123);

    stdout = &uart_str;
    uart_init();

    asm("sei");
    pb0_to_out();

    timer_init();
    timer_register(TIMER_1, 100, toggle_led);
    timer_register(TIMER_2, 200, print_text);
    // timer_register(TIMER_3, 1000, print_eeprom_bytes);


    /* MAIN LOOP */
    for (;;) {
        timer_handler();
        // if ((pwm_level==0) ||  (pwm_level==0xFF))
        // {
        //     i *= -1;
        // }
        // if (counter > 500)
        // {
        //     counter = 0;
        //     pwm_level+=i;
        // }
        
        // // for(uint32_t i = 0; i <1000000UL; ++i)
        // //     asm("nop");
        // // // toggle the led
        // // PORTB ^= (1 << PB0);

        // prog_pwm_pb0();


        // counter++;
        // pwm_count++;

    }

}
