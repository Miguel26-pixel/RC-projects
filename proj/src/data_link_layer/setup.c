#include "include/setup.h"
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BAUDRATE B38400

static struct termios old_configuration;

int open_serial_port(const char *path) {
    struct termios newtio;
    int fd = open(path, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror(path);
        return -1;
    }

    if (tcgetattr(fd, &old_configuration) == -1) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }
    memset(&newtio, 0, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 1;

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}

int close_serial_port(int fd) {
    sleep(1);
    if (tcsetattr(fd, TCSANOW, &old_configuration) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    return close(fd);
}