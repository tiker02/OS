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

void putChar(char c)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE){
            while(writing.saved == (writing.digested - 1))
            {
                writing.buffer[256] = 0; //pure debug
                _delay_ms(10);
            }
        }
        writing.buffer[writing.saved++] = c;        
    }
}

void putChar_test_01(void){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        if(writing.digested != writing.saved) {
            printf("%c\n", writing.buffer[writing.digested]);
            writing.buffer[writing.digested++] = 'd';
        }
    }
}

void info(void)
{
    printf("Saved: %d, digested: %d,     buf: %s", writing.saved, writing.digested, writing.buffer);
}