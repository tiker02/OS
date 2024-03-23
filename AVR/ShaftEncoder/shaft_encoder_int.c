#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "../avr_common/uart.h"

volatile uint8_t ext_int = 0;
volatile uint8_t time_int = 0;
volatile uint8_t current_state = 0xFF;
volatile uint8_t prev_state = 0xFF;
uint16_t uart_out_period = 100; //ms

ISR(PCINT0_vect)
{
    prev_state = current_state;
    current_state = PINB;
    ext_int = 1;
}

ISR(TIMER5_COMPA_vect) {
  time_int=1;
}


int main(void)
{
    printf_init();

    const uint8_t pin_mask = (0xFF); //11111111

    //REMEMBER: for each entry of the table, the last 4 bits of i represent a transition, 
    //where 00001 for example is the transition from state 00 to state 01. 
    //The corresponding value is the effect expected on the counter during the transition.
    char transition_table[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0}; 

    //pin set input, pullup resistor enabled
    DDRB &= ~pin_mask; 
    PORTB |= pin_mask;
    
    //External interrup on PCINT7:0 enabled
    PCICR |= (1 << PCIE0);
    PCMSK0 = pin_mask;

    //timer configuration: 
    //-prescale to 1024          ->
    //-compare set for 100 ms    -> 15.62 counts each ms
    TCCR5A = 0;
    TCCR5B = (1 << WGM52) | (1 << CS50) | (1 << CS52);
    uint16_t ocrval=(uint16_t)(15.62*uart_out_period);
    OCR5A = ocrval;

    cli();
    TIMSK5 |= (1<<OCIE5A);


    //idle mode enabled  will try using power save in future, but using timer2 is needed   
    SMCR = 0x01;

    char counters[]  = {0, 0, 0, 0};
    uint8_t transition;
    current_state = PINB;
    sei();
    while(1)
    {
        
        cli();
        if(ext_int)
        {
            for(char s = 0; s < 4; s++)
            {
                transition = ((current_state >> 2*s)&0x03) | (((prev_state >> 2*s)<<2)&0x0c);
                
                counters[s] += transition_table[transition]; 
                printf("Encoder %x, Transition %x, counter %d\n", s, ((current_state >> 2*s)&0x03) | (((prev_state >> 2*s)<<2)&0x0c), counters[s]);

            }  
            printf("\n\n");  
            ext_int = 0;        
        }

        if(time_int)
        {
            for(char s = 0; s < 4; s++)
            {
                printf("Encoder %x: Current state %x, Counter: %x\n\n\n", s, ((current_state >> 2*s)&0x03), counters[s]);
            }  
            time_int = 0;  
        }
        
        sei(); 
        sleep_cpu(); 
             
    } 
}