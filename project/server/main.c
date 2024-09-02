#include <avr/interrupt.h>
#include <avr/io.h>
#include <assert.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <stdint.h>
#include <stdio.h>
#include "tcb.h"
#include "tcb_list.h"
#include "uart.h"
#include "atomport_asm.h"
#include "scheduler.h"
#include "async_io.h"
#define THREAD_STACK_SIZE 256
#define IDLE_STACK_SIZE 128



//statically allocated variables where we put our stuff

TCB idle_tcb;
uint8_t idle_stack[IDLE_STACK_SIZE];
void idle_fn(uint32_t thread_arg __attribute__((unused))){
  while(1) {
    _delay_ms(100);
    info();
  }
}

TCB p1_tcb;
uint8_t p1_stack[THREAD_STACK_SIZE];
void p1_fn(uint32_t arg __attribute__((unused))){
  char c = 'c';
  while(1){
    putChar(c);
    _delay_ms(10);
  }
}

TCB p2_tcb;
uint8_t p2_stack[THREAD_STACK_SIZE];
void p2_fn(uint32_t arg __attribute__((unused))){
  while(1){
    putChar('e');
    _delay_ms(10);
  }
}

TCB p3_tcb;
uint8_t p3_stack[THREAD_STACK_SIZE];
void p3_fn(uint32_t arg __attribute__((unused))){
  while(1){
    getChar();
    _delay_ms(10);
  }
}


TCB p4_tcb;
uint8_t p4_stack[THREAD_STACK_SIZE];
void p4_fn(uint32_t arg __attribute__((unused))){
  while(1){
    getChar();
    _delay_ms(10);
  }
}

int main(void){
  // we need printf for debugging
  printf_init();
  
  printf("Initializing: %x\n", MCUSR);

  TCB_create(&idle_tcb,
             idle_stack+IDLE_STACK_SIZE-1,
             idle_fn,
             0);

  TCB_create(&p1_tcb,
             p1_stack+THREAD_STACK_SIZE-1,
             p1_fn,
             0);

  TCB_create(&p2_tcb,
             p2_stack+THREAD_STACK_SIZE-1,
             p2_fn,
             0);

  TCB_create(&p3_tcb,
             p3_stack+THREAD_STACK_SIZE-1,
             p3_fn,
             0);           

  TCB_create(&p4_tcb,
             p4_stack+THREAD_STACK_SIZE-1,
             p4_fn,
             0);           

  printf("TCBs created\n");
  
  TCBList_enqueue(&running_queue, &p1_tcb);
  TCBList_enqueue(&running_queue, &p2_tcb);
  TCBList_enqueue(&running_queue, &p3_tcb);
  TCBList_enqueue(&running_queue, &p4_tcb);
  TCBList_enqueue(&running_queue, &idle_tcb);

  printf("TCBs in running queue\n");

  printf("starting\n");
  async_io_init();
  startSchedule();
}
