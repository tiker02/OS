#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <assert.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include "tcb.h"
#include "tcb_list.h"
#include "uart.h"
#include "atomport_asm.h"
#include "scheduler.h"
#include "async_io.h"

char i = '0';

TCB io_task;
uint8_t io_stack[256];

void io(uint32_t thread_arg __attribute__((unused)))
{
    while(1)
    {
        printf("ecco...");
        //putChar(i);
    }
}


TCB counter_task;
uint8_t counter_stack[64];
void count(uint32_t thread_arg __attribute__((unused)))
{
    while(1)
    {
        printf("AOH\n");
        i++;
    }
}

int main(void)
{
    printf_init();
    printf("Starting\n");
    TCB_create(&counter_task, counter_stack + 63, count, 0);
    printf("Counter task succesfully created\n");
    TCB_create(&io_task, io_stack, io, 0);
    printf("IO task succesfully created\n");
    TCBList_enqueue(&running_queue, &counter_task);
    TCBList_enqueue(&running_queue, &io_task);
    printf("Running queue created\n");

    startSchedule();
}
