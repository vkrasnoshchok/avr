​
#include <avr/io.h>
#include <avr/interrupt.h>
​
#include "axis_control.h"
#include "led_dbg.h"
#include "pin_magic.h"
​
//#define TIMER_START()       ( TCCR1B |= (1 << CS10) | (1 << CS12) ) /* clk I/O /1024 (from prescaler) */
//#define TIMER_START()       ( TCCR1B |= (1 << CS12) )               /* clk I/O /256  (from prescaler) */
//#define TIMER_START()       ( TCCR1B |= (1 << CS10) | (1 << CS11) ) /* clk I/O /64 (from prescaler) */
//#define TIMER_START()       ( TCCR1B |= (1 << CS11) )               /* clk I/O /8 (from prescaler) */
#define TIMER_START()       ( TCCR1B |= (1 << CS10) )                 /* clk I/O /1 (no prescaling) */
​
#define TIMER_STOP()        ( TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12)) )
​
#define AXIS_ENABLE_ON()    ( PORTB &= ~(1 << PB0))
#define AXIS_ENABLE_OFF()   ( PORTB |=  (1 << PB0))
​
#define AXIS_TOOGLE_STATE(port, pin) ( *port ^= (1 << pin) )
​
static struct axis_ports_info_t
{
    volatile uint8_t *const p_port;
    uint8_t  step_pin;
    uint8_t  dir_pin;
    uint32_t steps;
} m_axis_ports = { .p_port = &PORTD };
​
ISR(TIMER1_COMPA_vect)
{
    static uint32_t counter = 0;
    if(counter++ < m_axis_ports.steps * 2)
    {
#if DEBUG_MODE == 1
        PORTB ^= (1 << PB5);
#endif /* DEBUG_MODE */
        AXIS_TOOGLE_STATE(m_axis_ports.p_port, m_axis_ports.step_pin);
    }
    else
    {
#if DEBUG_MODE == 1
        PORTB &= ~(1 << PB5);
#endif /* DEBUG_MODE */
        counter = 0;
        TIMER_STOP();
        TCNT1 = 0;
        AXIS_ENABLE_OFF();
    }
}
​
static void axis_timer_period_set(uint16_t value)
{
    OCR1AH = value >> 8;
    OCR1AL = value & 0x00FF;
}
​
error_t axis_control_move(cnc_control_axis_move_t *axis_info)
{
    switch(axis_info->axis)
    {
        case AXIS_X:
            m_axis_ports.step_pin = AXIS_X_STEP_PIN;
            m_axis_ports.dir_pin  = AXIS_X_DIR_PIN;
        break;
        case AXIS_Y:
            m_axis_ports.step_pin = AXIS_Y_STEP_PIN;
            m_axis_ports.dir_pin  = AXIS_Y_DIR_PIN;
        break;
        case AXIS_Z:
            m_axis_ports.step_pin = AXIS_Z_STEP_PIN;
            m_axis_ports.dir_pin  = AXIS_Z_DIR_PIN;
        break;
        default:
            return ERROR_INVALID_PARAM;
    }
​
    m_axis_ports.steps = axis_info->steps;
​
    if(axis_info->dir)
    {
        *m_axis_ports.p_port |= (1 << m_axis_ports.dir_pin);
    }
    else
    {
        *m_axis_ports.p_port &= ~(1 << m_axis_ports.dir_pin);
    }
​
    axis_timer_period_set(axis_info->speed);
    TIMER_START();
​
    AXIS_ENABLE_ON();
    return ERROR_SUCCESS;
}
​
void axis_control_init()
{
    DDRD |= (1<<PD2)            /** Step X port to output */
         |  (1<<PD3)            /** Step Y port to output */
         |  (1<<PD4)            /** Step Z port to output */
         |  (1<<PD5)            /** Direction X port to output */
         |  (1<<PD6)            /** Direction Y port to output */
         |  (1<<PD7);           /** Direction Z port to output */
​
    DDRB |= (1<<PB0);           /** Enable port to output */
    AXIS_ENABLE_OFF();
​
​
    /** init timer1 */
    TCCR1B |= (1 << WGM12);     /** Timer/Counter Mode of operation CTC */
    TIMSK1 |= (1 << OCIE1A);    /** Output Compare A Match Interrupt Enable */
​
    TCNT1L = 0;
    TCNT1H = 0;
}
​
​
Collap