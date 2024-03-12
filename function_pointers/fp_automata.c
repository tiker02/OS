#include<stdio.h>
int automata_state = 0;
char x = 0;

typedef void (*state_fn_ptr)(char);

void s1(char in)
{
    if(in == 0xAA) 
        automata_state = 1;
}

void s2(char in)
{
    if(in == 0x55)
        automata_state = 2;
    else 
        automata_state = 0;
}

void s3(char in)
{
    x = in;
    automata_state = 3;
}

void s4(char in)
{
    if(--x <= 0) 
        automata_state = 4;
    printf("%c", in);
}

void s5(char in)
{
    if(in == 0xAA) 
        automata_state = 1;
    else 
        automata_state = 0;
}


int main(int argc, char* argv[])
{
    state_fn_ptr states[5];
    states[0] = s1;
    states[1] = s2;
    states[2] = s3;
    states[3] = s4;
    states[4] = s5;
    
    int i = 0;
    while(argv[1][i])
    {
        (*states[automata_state])(argv[1][i++]);
    }
    
}