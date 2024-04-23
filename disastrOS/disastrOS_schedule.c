#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"

// replaces the running process with the next one in the ready list
void internal_schedule() {
  if (running) {
    disastrOS_debug("PREEMPT - %d ->", running->pid);
   }
  // at least one process should be in the ready queue
  // if not, no preemption occurs
  
  if (ready_list.first){
    PCB* next_process=(PCB*) List_detach(&ready_list, ready_list.first);
    running->status=Ready;
    List_insert(&ready_list, ready_list.last, (ListItem*) running);
    next_process->status=Running;
    running=next_process;
  }
  //disastrOS_printStatus();
 
  if (running) {
    disastrOS_debug(" %d\n", running->pid);
  }
}

void internal_revert_schedule()
{
  List_revert(&ready_list);
  internal_schedule();
}

PCB* find_highest_priority(ListHead* pcb_list)
{
  ListItem* item = pcb_list->first;
  PCB* highest_priority_pcb = (PCB*) item;
  int highest_priority = highest_priority_pcb->current_priority;
  while(item != 0)
  {
    int item_priority = ((PCB*) item)->current_priority;
    if(item_priority > highest_priority)
    {
      highest_priority = item_priority;
      highest_priority_pcb = (PCB*) item;
    }
    item = item->next;
  }
  return highest_priority_pcb;
}

void internal_priority_schedule()
{
  if (running) {
    disastrOS_debug("PREEMPT - %d ->", running->pid);
  }


  if(ready_list.first)
  {  
    PCB* next_process= find_highest_priority(&ready_list);
    List_detach(&ready_list,(ListItem*) next_process);
    running->status = Ready;
    if(!(--running->current_priority)){
      running->current_priority = running->process_priority;
    }
    List_insert(&ready_list, ready_list.last, (ListItem*) running);
    next_process->status = Running;
    running = next_process;
  }

  disastrOS_printStatus();

  if (running) {
    disastrOS_debug(" %d\n", running->pid);
  }
}


