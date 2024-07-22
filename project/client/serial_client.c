#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "serial_client.h"

#define DEBUG 1

sem_t terminal_semaphore;
int serial_fd;

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


void* write_serial()
{
    if(DEBUG) fprintf(stderr, "Writing thread started\n");

    char write_char = 0;
    while(write_char != '\n')
    {
        

        int ret = sem_wait(&terminal_semaphore);
        if(ret == -1) { perror("Error occurred on sem_wait@write_serial\n"); exit(EXIT_FAILURE); }
        
        write_char = getchar();
        if(DEBUG) fprintf(stderr, "Character input: %c\n", write_char);

        int wr = write(serial_fd, &write_char, 1);
        if(wr == -1){
            perror("Error occurred while writing character\n");
            exit(1);
        }

        if(DEBUG) fprintf(stderr, "Character written: %c\n", write_char);

        ret = sem_post(&terminal_semaphore);
        if(ret == -1) { perror("Error occurred on sem_post@write_serial\n"); exit(EXIT_FAILURE); }

    }    
    pthread_exit(NULL);
}

void* read_serial()
{
    if(DEBUG) fprintf(stderr, "Reading thread started\n");
    char read_buf[BUF_SIZE];

    FILE* out_file = fopen("./output.txt", "w");
    if(out_file == NULL) { perror("Error occurred while opening output file\n"); exit(EXIT_FAILURE); } 
    if(DEBUG)
    {
        fprintf(stderr, "Output file successfully opened\n");
        fprintf(out_file, "Out:\n");
    }

    int ret, rd = 1;
    while(read_buf[rd - 1] != '\n')
    {
        ret = sem_wait(&terminal_semaphore);
        if(ret == -1) { perror("Error occurred on sem_wait@write_serial\n"); exit(EXIT_FAILURE); }

        if(DEBUG) fprintf(stderr, "Going to read\n");

        rd = read(serial_fd, read_buf, BUF_SIZE);
        if(rd == -1){
            perror("Error occcurred while reading\n");
            exit(EXIT_FAILURE);
        } 
        //if(DEBUG) fprintf(out_file, "Read:\n");
        fwrite(read_buf, sizeof(char), rd, out_file);        
        ret = sem_post(&terminal_semaphore);
        if(ret == -1) { perror("Error occurred on sem_post@write_serial\n"); exit(EXIT_FAILURE); }
    }

    ret = fclose(out_file);
    if(ret != 0) { perror("Error occurred while closing output file\n"); exit(EXIT_FAILURE); } 
    pthread_exit(NULL);
}

int main()
{
    if(DEBUG) fprintf(stderr, "Serial client started\n");

    serial_fd = serial_open();
    set_serial_communication_interface(serial_fd);

    if(DEBUG) fprintf(stderr, "Serial file descriptor initialized: fd: %d\n", serial_fd);

    int ret = sem_init(&terminal_semaphore, 0, 1);
    if(ret == -1) { perror("Error occurred in semaphore initialization\n"); exit(EXIT_FAILURE); }

    pthread_t write_thread, read_thread;
    ret = pthread_create(&write_thread, NULL, &write_serial, NULL);
    if(ret) { perror("Error occurred in writing thread creation\n"); exit(EXIT_FAILURE); }
    ret = pthread_create(&read_thread, NULL, &read_serial, NULL);
    if(ret) { perror("Error occurred in reading thread creation\n"); exit(EXIT_FAILURE); }


    ret = pthread_join(write_thread, NULL);
    if(ret != 0) { perror("Error occurred in writing thread detaching\n"); exit(EXIT_FAILURE);}
    ret = pthread_join(read_thread, NULL);
    if(ret != 0) { perror("Error occurred in reading thread detaching\n"); exit(EXIT_FAILURE);}
    //ret = sem_destroy(&terminal_semaphore);
    //if(ret == -1) { perror("Error occurred in semaphore destroying\n"); exit(EXIT_FAILURE); }

    return 0;
}