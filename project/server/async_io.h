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

void getChar(void);

void putChar(char c);

void info(void);