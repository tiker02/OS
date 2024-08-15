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
#define THREAD_STACK_SIZE 256
#define SERVERS           3
#define REPEAT            10



//statically allocated variables where we put our stuff

TCB server_threads[SERVERS] ;
uint8_t server_stacks[SERVERS * THREAD_STACK_SIZE];
void server(uint32_t thread_arg __attribute__((unused))){
  while(1) 
  {
    char c = getChar();
    for(int i = 0; i < REPEAT; i++)
    {
      putChar(c);
    }  
  }
}

TCB stupid;
uint8_t stupid_stack[THREAD_STACK_SIZE];
void fn(uint32_t arg __attribute__((unused))){
  while(1)
  {
    printf("Ma allora...\n");
  }
}

TCB init_tcb;
uint8_t init_stack[THREAD_STACK_SIZE];
void init(uint32_t arg __attribute__((unused))){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    for(int i = 0; i < SERVERS; i++)
    {
      TCB_create(server_threads + i, server_stacks + i*THREAD_STACK_SIZE -1, server, 0);
      TCBList_enqueue(&running_queue, server_threads + i);
    }
    TCBList_print(&running_queue);
    printf("Init worked\n");
    while(1){
      SMCR |= 0x01;
      sleep_cpu();
    }
  }
}


int main(void){
  printf_init();

  printf("Initialized program\n");
  printf("Initiated: %p\n", &init_tcb);
  TCB_create(&init_tcb,
             init_stack+THREAD_STACK_SIZE-1,
             init,
             0);

  TCB_create(&stupid, stupid_stack + THREAD_STACK_SIZE-1, fn, 0);

  
  TCBList_enqueue(&running_queue, &init_tcb);
  TCBList_enqueue(&running_queue, &stupid);
  TCBList_print(&running_queue);

  
  startSchedule();
  printf("Post\n");
}
