#include <avr/interrupt.h>
#include <avr/io.h>
#include <assert.h>
#include <stdio.h>
#include "tcb.h"
#include "tcb_list.h"
#include "atomport_asm.h"
#include "timer.h"

// the (detached) running process
TCB* current_tcb=NULL;

// the running queue
TCBList running_queue={
  .first=NULL,
  .last=NULL,
  .size=0
};


void startSchedule(void){
  printf("Startshedule, %p\n", current_tcb);
  cli();
  current_tcb=TCBList_dequeue(&running_queue);
  printf("Startschedule, %p\n", current_tcb);
  assert(current_tcb);
  timerStart();
  archFirstThreadRestore(current_tcb);
}

void schedule(void) {
  
  TCB* old_tcb=current_tcb;
  // we put back the current thread in the queue
  TCBList_enqueue(&running_queue, current_tcb);
  // we fetch the next;
  current_tcb=TCBList_dequeue(&running_queue);
  printf("Scheduling1: %p, %p\n", old_tcb, current_tcb);
  // we jump to it (useless if it is the only process)
  if (old_tcb!=current_tcb){
    printf("Scheduling2, %p\n", current_tcb);
    archContextSwitch(old_tcb, current_tcb);
  }
}
