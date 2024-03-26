#include <ucontext.h>
#include <stdio.h>

#define STACK_SIZE 16384
#define CONTEXT_NUMBER 10
const int num_iterations=100;


ucontext_t main_context, contexts[CONTEXT_NUMBER];
int current_context = 0, next_context = 1;

void f(){
  printf("Context%d started\n", current_context%CONTEXT_NUMBER);
  for (int i=0; i<num_iterations; i++) {
    printf("Context%d: %d\n", current_context%CONTEXT_NUMBER, i);
    swapcontext(&contexts[(current_context++)%CONTEXT_NUMBER], &contexts[(next_context++)%CONTEXT_NUMBER]);
  }
  setcontext(&main_context);
}

int main(){
    char context_stacks[CONTEXT_NUMBER][STACK_SIZE];

    for(int i = 0; i < CONTEXT_NUMBER; i++)
    {
        getcontext(&contexts[i]);

        // set the stack of f1 to the right place
        contexts[i].uc_stack.ss_sp = context_stacks[i];
        contexts[i].uc_stack.ss_size = STACK_SIZE;
        contexts[i].uc_stack.ss_flags = 0;
        contexts[i].uc_link=&main_context;

        // create a trampoline for the first function
        makecontext(&contexts[i], f, 0, 0);
    }

  swapcontext(&main_context, &contexts[0]); // we will jump back here
  printf("exiting\n");
}
