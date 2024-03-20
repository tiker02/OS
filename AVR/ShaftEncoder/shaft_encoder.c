#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include "../avr_common/uart.h"

int main(void)
{
    printf_init();

    const uint8_t mask = (0x03); //00000011

    //REMEMBER: for each entry of the table, the last 4 bits of i represent a transition, 
    //where 0011 for example is the transition from state 00 to state 11. 
    //The corresponding value is the effect expected on the counter during the transition.
    char transition_table[] = {0, 1, 0, -1, -1, 0, 1, 0, 0, -1, 0, 1, 1, 0, -1, 0}; 


    DDRB &= ~mask; 
    PORTB |= mask;

    char counter = 0;
    char prev_state = 0;
    while(1)
    {
        char curr_state = PINB & mask;

        char transition = (prev_state << 2) | curr_state;
        counter += transition_table[transition];

        prev_state = curr_state;

        printf("Transition (hex view): %x\nCurrent State: %x\nCounter: %x\n---------------\n", transition, curr_state, counter);
        _delay_ms(2000);
    } 

}