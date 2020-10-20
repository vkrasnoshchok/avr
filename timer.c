#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "timer.h"

#if !defined(TIMER_NUMBER)
    #error TIMER_NUMBER is not defined
#endif

typedef struct {
    uint8_t timeout;
    uint8_t counter;
    //uint64_t param;
    timer_cb callback;
} timer_descriptor_t;

static timer_descriptor_t timer_descriptor[TIMER_NUMBER];
static volatile uint8_t timer_flag=0;

void timer_init(void)
{
    for (uint8_t i=0; i<TIMER_NUMBER; i++)
    {
        timer_descriptor[i].timeout = 0;
    }
    // TC2 100Hz for 16MHz chip 
    OCR2A = 155;
    TCCR2A = 1 << WGM21;
    TCCR2B |= (1 << CS20) | (1 << CS21) | (1 << CS22);
    TIMSK2 |= 1 << OCIE2A;
}

SIGNAL(TIMER2_COMPA_vect)
{
    timer_flag = 1;
}

uint8_t timer_register(uint8_t timer, uint8_t timeout, timer_cb callback)
{
    if (timer >= TIMER_NUMBER || !callback)
    {
        return 1;
    }
    timer_descriptor[timer].counter = 0;
    timer_descriptor[timer].callback = callback;
    timer_descriptor[timer].timeout = timeout;
    return 0;
}

uint8_t timer_stop(uint8_t timer)
{
    if (timer >= TIMER_NUMBER)
    {
        return 1;
    }
    timer_descriptor[timer].counter = 0;
    timer_descriptor[timer].timeout = 0;
    return 0;
}

uint8_t timer_start(uint8_t timer, uint8_t timeout)
{
    if (timer >= TIMER_NUMBER)
    {
        return 1;
    }
    timer_descriptor[timer].counter = 0;
    timer_descriptor[timer].timeout = timeout;
    return 0;
}

void timer_handler(void)
{
    if (timer_flag)
    {
        timer_flag = 0;
        for (uint8_t i=0; i<TIMER_NUMBER; i++)
        {
            if (timer_descriptor[i].timeout == 0)
            {
                continue;
            }
            timer_descriptor[i].counter++;
            if (timer_descriptor[i].counter == timer_descriptor[i].timeout)
            {
                timer_descriptor[i].counter = 0;
                timer_descriptor[i].callback();
            }
        }
    }
}
