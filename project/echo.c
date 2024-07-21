#include <stdio.h>
#include <avr/io.h>
#include "../avr_common/uart.h"


int main(void)
{
    printf_init();
    char rchar = 0;
    while(1)
    {
        rchar = usart_getchar();
        usart_putchar(rchar);
    }
}