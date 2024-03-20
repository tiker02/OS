#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include "../avr_common/uart.h"

int main(void)
{
    printf_init();
    
    DDRA = 0x0F //00001111 (4input/4output)
    PORTA = 0xFF

    while(1)
    {
        const uint8_t mask = 0x01;
        const uint8_t filter = 0x10;

        for(int r = 0; r < 4; r++)
        {        
            PORTA &= ~(mask<<r);
            _delay_ms(100);
            int new_row1 = PINA;
            int change = prev_row1 ^ new_row1;
            for(int c = 0; c < 4; c++)
            {
                if(change & (filter<<c))
                {
                    printf("Key: %d, %d.  Event: ", r, c);
                    (new_row & (filter << c))? printf("Released\n") : printf("Pressed\n");
                }
            }

        

    }
}