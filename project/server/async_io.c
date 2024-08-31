#include "async_io.h"
#include <util/atomic.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

uint8_t DOR_check = 0;
uint8_t shifter = 0;

io_structure writing = 
{
    .saved = 0,
    .digested = 0
};

io_structure reading =
 {
    .saved = 0,
    .digested = 0
 };


void getChar(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        while(reading.digested == reading.saved)
        {
            NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE){
            reading.buffer[256] = 0; //pure debug
            _delay_ms(10);
            }
        }
        printf("DOR0: %x\n", DOR_check);
        printf("%c\n", reading.buffer[reading.digested]);
        reading.buffer[reading.digested++] = 'd';
    }
}
/*
void getChar_test01(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        while(reading.saved != (reading.digested - 1) && (UCSR0A & (1<<RXC0)) )
        {
            reading.buffer[reading.saved++] = UDR0;
        }
    }
}
*/
ISR(USART0_RX_vect)
{
    if(reading.saved != (reading.digested - 1))
    {
        DOR_check |= (UCSR0A & (1<<DOR0)) << ((shifter++)%7); //make a bitmap to check for Data OverRun in last readings
        reading.buffer[reading.saved++] = UDR0;
    }
    else
    {
        char discarded = UDR0;
    }
}

void putChar(char c)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        while(writing.saved == (writing.digested - 1))
        {
            NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE){
            writing.buffer[256] = 0; //pure debug
            _delay_ms(10);
            }
        }
        writing.buffer[writing.saved++] = c;        
    }
}

void send(void){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        if(writing.digested != writing.saved) {
            while ( !(UCSR0A & (1<<UDRE0)) ); 
            UDR0 = writing.buffer[writing.digested];
            writing.buffer[writing.digested++] = 'd';
        }
    }
}

void info(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        printf("Writing -> Saved: %d, digested: %d,     buf: %s\n", writing.saved, writing.digested, writing.buffer);
        printf("Reading -> Saved: %d, digested: %d,     buf: %s\n", reading.saved, reading.digested, reading.buffer);
    }
}