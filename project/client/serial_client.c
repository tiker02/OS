#include "serial_client.h"
#include <stdio.h>

int serial_open()
{
    int serial_fd = open(SERIAL_NAME, O_RDWR | O_NOCTTY | O_SYNC);
    if(serial_fd < 0) perror("Error occurred while tryng to open serial communication with server \n");
    return serial_fd;
}

void set_serial_communication_interface(int fd)
{
    struct termios tty = {0};
    cfsetispeed(&tty, BAUDRATE);
    cfsetospeed(&tty, BAUDRATE);
    cfmakeraw(&tty);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 5;

    if (tcsetattr (fd, TCSANOW, &tty) != 0) perror("Error occurred in setting serial interface attributes\n");
}

int main()
{
    int serial_fd = serial_open();
    set_serial_communication_interface(serial_fd);

    pid_t pid =fork();

    if(pid == -1){
        perror("Error occurred while forking\n");
        exit(1);
    } 
    
    if(pid == 0)
    {
        char read_buf[BUF_SISE];

        while(1)
        {
            int rd = read(serial_fd, read_buf, BUF_SISE);
            if(rd == -1){
                perror("Error occcurred while reading\n");
                _exit(1);
            } 
            printf("%s", read_buf);
        }

        _exit(0);
    }

    char write_char = 0;
    while(write_char != '\n')
    {
        write_char = getchar();
        int wr = write(serial_fd, &write_char, 1);
        if(wr == -1){
            perror("Error occurred while writing character\n");
            exit(1);
        } 
    }

    return;

}