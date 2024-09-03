#pragma once
#include "tcb.h"
#include "tcb_list.h"
#include <stdint.h>
#define BUF_SIZE 257

typedef struct io_structure
{
    char buffer[BUF_SIZE];
    uint8_t saved;
    uint8_t digested;
} io_structure;

//strictly required before using getChar and putChar
void async_io_init(void);

char getChar(void);

void putChar(char c);

void info(void);