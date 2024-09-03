#include "async_io.h"
#include <util/atomic.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/sleep.h>
#include "uart.h"
#include "scheduler.h"
#include "atomport_asm.h"

#define READ 0
#define WRITE 1

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

 TCBList read_wait =
 {
    .first=NULL,
    .last=NULL,
    .size=0
 };

 TCBList write_wait =
 {
    .first=NULL,
    .last=NULL,
    .size=0
 };

uint8_t sender_init = 0;
TCB send_tcb;
uint8_t send_stack[64];
void send(void);
void send_fn(uint32_t thread_arg __attribute__((unused)));
void async_io_init(void);

void io_wait(uint8_t io);
TCBList* io_wait_queue(uint8_t io);
void io_wake_up(uint8_t io);

char getChar(void)
{
    char c = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        while(reading.digested == reading.saved)
        {
            TCBList_print(io_wait_queue(READ));
            io_wait(READ);
        }
        printf("DOR0: %x\n", DOR_check);
        c = reading.buffer[reading.digested];
        printf("%c\n", c);
        reading.buffer[reading.digested++] = 'd';
    }
    return c;
}

ISR(USART0_RX_vect)
{
    if(reading.saved != (reading.digested - 1))
    {
        DOR_check |= (UCSR0A & (1<<DOR0)) << ((shifter++)%7); //make a bitmap to check for Data OverRun in last readings
        reading.buffer[reading.saved++] = UDR0;
        if(io_wait_queue(READ)->size > 0) io_wake_up(READ);
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
            TCBList_print(io_wait_queue(WRITE));
            io_wait(WRITE);
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
            
            if(io_wait_queue(WRITE)->size > 0) io_wake_up(WRITE);
        }
        else 
        {
            NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE)
            {   
                SMCR |= 0x01;
                sleep_cpu();
            }
        }     }
}

void send_fn(uint32_t thread_arg __attribute__((unused))){
  while(1) {
    send();
  }
}

void async_io_init()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        TCB_create(&send_tcb, send_stack + 63, send_fn, 0);
        TCBList_enqueue(&running_queue, &send_tcb);
        printf("Send initialized\n");
    }
}

void info(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        printf("Writing -> Saved: %d, digested: %d,     buf: %s\n", writing.saved, writing.digested, writing.buffer);
        printf("Reading -> Saved: %d, digested: %d,     buf: %s\n", reading.saved, reading.digested, reading.buffer);
    }
}

TCBList* io_wait_queue(uint8_t io)
{
    TCBList* wait_queue = io? (&write_wait) : (&read_wait);
    return wait_queue;
}

void io_wait(uint8_t io)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        TCBList* wait_queue = io? (&write_wait) : (&read_wait);
        TCB* old_tcb=current_tcb;
        TCBList_enqueue(wait_queue, current_tcb);
        current_tcb=TCBList_dequeue(&running_queue);
        printf("WAIT %d\n", io);
        archContextSwitch(old_tcb, current_tcb);
        printf("YEP\n");
    }
}

void io_wake_up(uint8_t io)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        TCBList* wait_queue = io? (&write_wait) : (&read_wait);
        TCB* old_tcb=current_tcb;
        TCBList_enqueue(&running_queue, current_tcb);
        current_tcb=TCBList_dequeue(wait_queue);
        printf("WAKE UP %d\n", io);
        archContextSwitch(old_tcb, current_tcb);
        printf("SIU\n");
    }
} 