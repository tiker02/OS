#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "../avr_common/uart.h"

volatile uint8_t int_occurred = 0;
volatile uint8_t current_state = 0xFF;
volatile uint8_t prev_state = 0xFF;

ISR(PCINT0_vect)
{
    prev_state = current_state;
    current_state = PINB;
    int_occurred = 1;
}

int main(void)
{
    printf_init();

    const uint8_t pin_mask = (0xFF); //11111111

    //REMEMBER: for each entry of the table, the last 4 bits of i represent a transition, 
    //where 00001 for example is the transition from state 00 to state 01. 
    //The corresponding value is the effect expected on the counter during the transition.
    char transition_table[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0}; 


    DDRB &= ~pin_mask; 
    PORTB |= pin_mask;
    SMCR = 0x07;

    PCICR |= (1 << PCIE0);
    PCMSK0 = pin_mask;

    char counters[]  = {0, 0, 0, 0};
    uint8_t transition;
    current_state = PINB;
    sei();
    while(1)
    {
        sleep_cpu(); 
        cli();
        if(int_occurred)
        {
            
            for(char s = 0; s < 4; s++)
            {
                transition = ((current_state >> 2*s)&0x03) | (((prev_state >> 2*s)<<2)&0x0c);
                
                counters[s] += transition_table[transition]; 
                printf("Encoder %x, current state %x, Transition %x, counter %d\n", s, current_state, ((current_state >> 2*s)&0x03) | (((prev_state >> 2*s)<<2)&0x0c), counters[s]);

            }  
            printf("\n\n\n");          
        }
        int_occurred = 0;
        sei(); 
             
    } 
}