#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include "../avr_common/uart.h"

int main(void)
{
    printf_init();
    
    DDRA = 0x0F; //00001111 (4input/4output)
    PORTA = 0xFF;
    
    const uint8_t mask = 0x01;
    uint8_t rows[] = { 0xFF, 0xFF}; //these 16 bits represent the curresnt state row by row, each represented with 4 bytes
    while(1)
    {
        for(int r = 0; r < 4; r++)
        {        
            PORTA &= ~(mask<<r);
            _delay_ms(100);
            uint8_t out = (PINA>>4);
            uint8_t change = (rows[(r>>1)] << (~r&1)) ^ out;
            
            if(change  != 0)
                for(int c = 0; c < 4; c++)
                {
                    if((change&(mask<<c)))
                    {
                        printf("Key %x event: ", 4*r + c);
                        if(out & (mask<<c)) printf("released\n");
                        else printf("pressed\n");
                    }
                }
        }
    }
}