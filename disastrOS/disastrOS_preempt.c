#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"

void internal_preempt() {
  //internal_revert_schedule();
  printf("\n\n PREEMPT CALLED");
  internal_priority_schedule();
}
