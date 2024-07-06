#pragma once
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#define SERIAL_NAME 
#define BAUDRATE B19200
#define BUF_SISE 1024

int serial_open();

void set_serial_communication_interface(int fd);