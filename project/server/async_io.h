#include"tcb.h"
#include"tcb_list.h"
#define BUF_SIZE 1024



typedef struct io_structure {
    char buffer[BUF_SIZE];
    uint16_t offset;
    uint16_t difference;
} io_structure;

char getChar(void);

void putChar(char c);

void wait(TCBList* io_waiting_list);

void wake_up(TCBList* io_waiting_queue);

void info(void);
