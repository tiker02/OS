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
    uint8_t rows[] = { 0xFF, 0xFF}; //these 16 bits represent the current state row by row, each represented with 4 bytes
    uint8_t out = 0x00, change = 0x00;
    while(1)
    {
        for(int r = 0; r < 4; r++)
        {        
            PORTA &= ~(mask<<r);
            _delay_ms(100);
            out = (PINA>>4);
            change = (rows[(r>>1)] >>  4*(~r&0x01)) ^ out;
            change &= 0x0F;
            //printf("PORTA: %x, PINA: %x, out: %x, row: %x, shift: %x, prev out: %x, change: %x\n",PORTA, PINA, out, rows[(r>>1)], 4*(~r&0x01),  rows[(r>>1)] >>  4*(~r&0x01), change);
            
            if(change != 0)
            {
                for(int c = 0; c < 4; c++)
                {
                    if((change&(mask<<c)))
                    {
                        printf("Key %x event: ", 4*r + c);
                        if(out & (mask<<c)) printf("released\n");
                        else printf("pressed\n");
                    }
                }
                rows[r>>1] = (rows[r>>1] & 0xF0>>4*(~r&0x01)) | (out<<4*(~r&0x01));
                //printf("new_row: %x\n\n", rows[r>>1]);
            }
            PORTA |= 0x0F; 
        }
        _delay_ms(200);
    }
}