#include "async_io.h"
#include <util/atomic.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"


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
        printf("%c\n", reading.buffer[reading.digested]);
        reading.buffer[reading.digested++] = 'd';
    }
}

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
    printf("Writing -> Saved: %d, digested: %d,     buf: %s\n", writing.saved, writing.digested, writing.buffer);
    printf("Reading -> Saved: %d, digested: %d,     buf: %s\n", reading.saved, reading.digested, reading.buffer);
}