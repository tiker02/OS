#include "async_io.h"
#include <util/atomic.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"


io_structure writing = 
{
    .saved = 0,
    .digested = 0
};

void putChar(char c)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        while(writing.saved == (writing.digested - 1))
        {
            NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE)
            {
                writing.buffer[256] = 0; //pure debug
                _delay_ms(1000);
            }
        }
        writing.buffer[writing.saved++] = c;
        printf("Here we go!\n");
        UCSR0B |= (1<<UDRIE0);  
        printf("Soooo...\n");  
    }
}

void putChar_test_01(void){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        if(writing.digested != writing.saved) {
            writing.buffer[writing.digested++] = 'd';
        }
    }
}

ISR(USART0_UDRE_vect)
{
    if(writing.digested != writing.saved && (UCSR0A & (1<<(UDRE0)))) {
        UDR0 = writing.buffer[writing.digested];
        writing.buffer[writing.digested++] = 'd';
        printf("Allora...\n");
    }
    else 
    {
        //UCSR0B &= ~(1<<UDRIE0);
        printf("quindi\n");
    }
    
    reti();
}

ISR(BADISR_vect)
{
    reti();
}

void info(void)
{
    printf("Saved: %d, digested: %d,     buf: %s\n", writing.saved, writing.digested, writing.buffer);
}