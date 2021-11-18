#include "include/port.h"

#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "include/errnos.h"

#define BAUDRATE B38400

static struct termios old_configuration;

int open_serial_port(const char *path) {
    if(path == NULL) {
        return NULL_POINTER_ERROR;
    }

    struct termios newtio;
    int fd = open(path, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror(path);
        return IO_ERROR;
    }

    if (tcgetattr(fd, &old_configuration) == -1) {
        perror("tcgetattr");
        close(fd);
        return CONFIGURATION_ERROR;
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
        return CONFIGURATION_ERROR;
    }

    return fd;
}

int close_serial_port(int fd) {
    sleep(1);
    if (tcsetattr(fd, TCSANOW, &old_configuration) == -1) {
        perror("tcsetattr");
        return CONFIGURATION_ERROR;
    }

    return close(fd);
}
