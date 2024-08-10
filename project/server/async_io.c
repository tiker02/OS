#include"async_io.h" 
#include"atomport_asm.h"
#include"scheduler.h"
#include<util/atomic.h>
#include<stdio.h>

io_structure read_structure = {
    .offset = 0,
    .difference = 0
};
io_structure write_structure = {
    .offset = 0,
    .difference = 0
};

TCBList read_waiting_queue = {
    .first = NULL,
    .last = NULL,
    .size = 0
};

TCBList write_waiting_queue = {
    .first = NULL,
    .last = NULL,
    .size = 0
};



void wait(TCBList* io_waiting_queue)
{
    TCB* wait_tcb = current_tcb;
    TCBList_enqueue(io_waiting_queue, wait_tcb);

    current_tcb = TCBList_dequeue(&running_queue);
    archContextSwitch(wait_tcb, current_tcb);
}

char getChar()
{
    char c = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        while(! (read_structure.difference > 0))
        {
            NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE){
            wait(&read_waiting_queue);
            }
        }
        c = read_structure.buffer[(read_structure.offset++)%BUF_SIZE];
        read_structure.difference--;
    }
    return c;
}

void putChar(char c)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        info();
        while(! (write_structure.difference > 0))
        {
            NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE){
            wait(&write_waiting_queue);
            }
        }
        write_structure.buffer[(write_structure.offset+write_structure.difference)%BUF_SIZE] = c;
        write_structure.difference++;
        info();
    }
}

ISR(USART0_RX_vect)
{
    cli();
    //char c = UDR0;
    //printf("%c", c);
    printf("UAH\n");
    if(read_structure.difference < BUF_SIZE - 1)
    {
        read_structure.buffer[(read_structure.offset + read_structure.difference)%BUF_SIZE] = UDR0;
        read_structure.difference++;
        wake_up(&read_waiting_queue);
    }
    sei();
}

ISR(USART0_TX_vect)
{
    cli();
    printf("UOH");
    if(write_structure.difference < BUF_SIZE - 1)
    {
        UDR0 = write_structure.buffer[((write_structure.offset)%BUF_SIZE)];
        write_structure.difference--;
        wake_up(&write_waiting_queue);
    }
    else UDR0 = 's';
    sei();
}

void wake_up(TCBList* io_waiting_queue)
{
    TCB* old_tcb = current_tcb;
    TCBList_enqueue(&running_queue, old_tcb);

    current_tcb = TCBList_dequeue(io_waiting_queue);
    archContextSwitch(old_tcb, current_tcb);
}


/////////////////////////////
//DEBUG AND TESTING

void info(void)
{
    printf("read: offset: %d difference: %d\n", read_structure.offset, read_structure.difference);
    printf("%s\n", read_structure.buffer);
    TCBList_print(&read_waiting_queue);

    printf("write: offset: %d difference: %d\n", write_structure.offset, write_structure.difference);
    printf("%s\n", write_structure.buffer);
    TCBList_print(&write_waiting_queue);
}