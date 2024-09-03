#include <avr/interrupt.h>
#include <avr/io.h>
#include <assert.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/sleep.h>
#include "tcb.h"
#include "tcb_list.h"
#include "uart.h"
#include "atomport_asm.h"
#include "scheduler.h"
#include "async_io.h"
#define THREAD_STACK_SIZE 256
#define SERVERS           3
#define REPEAT            10



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

TCB init_tcb;
uint8_t init_stack[THREAD_STACK_SIZE];
void init(uint32_t arg __attribute__((unused))){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
  printf("Init start\n");
  for(int i = 0; i < SERVERS; i++)
  {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      TCB_create(&server_threads + i*sizeof(TCB), server_stacks + i*THREAD_STACK_SIZE -1, server, 0);
      TCBList_enqueue(&running_queue, &server_threads + i*sizeof(TCB));
    }
  }
  printf("Init work done\n");
  }
  while(1){
    info();
    SMCR |= 0x01;
    sleep_cpu(); 
  }
}


int main(void){
  printf_init();

  TCB_create(&init_tcb,
             init_stack+THREAD_STACK_SIZE-1,
             init,
             0);
  TCBList_enqueue(&running_queue, &init_tcb);
  
  async_io_init();
  startSchedule();
}
