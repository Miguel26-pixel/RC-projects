#include "../../include/data_link_layer/port.h"

#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "../../include/errors/error_nos.h"
#include "../../include/gui/gui.h"

#define BAUDRATE B38400

static struct termios old_configuration;

int open_serial_port(const char *path) {
    if (path == NULL) return NULL_POINTER_ERROR;

    struct termios newtio;

    LOG_DRIVER_EVENT("[driver]: opening serial port\n")
    int fd = open(path, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        LOG_DRIVER_ERROR("[driver]: opening serial port: error %s\n", strerror(errno))
        return IO_ERROR;
    } else {
        LOG_DRIVER_EVENT("[driver]: opened serial port\n")
    }

    LOG_DRIVER_EVENT("[driver]: getting serial port configuration\n")
    if (tcgetattr(fd, &old_configuration) == -1) {
        LOG_DRIVER_ERROR("[driver]: getting serial port configuration: error %s\n", strerror(errno))
        LOG_DRIVER_EVENT("[driver]: closing serial port\n")
        if (close(fd) < 0) {
            LOG_DRIVER_EVENT("[driver]: closing serial port: error: %s\n", strerror(errno))
        } else {
            LOG_DRIVER_EVENT("[driver]: closed serial port\n")
        }
        return CONFIGURATION_ERROR;
    } else {
        LOG_DRIVER_EVENT("[driver]: got serial port configuration\n")
    }

    memset(&newtio, 0, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    newtio.c_lflag = 0;

    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 1;

    tcflush(fd, TCIOFLUSH);

    LOG_DRIVER_EVENT("[driver]: configuring serial port\n")
    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        LOG_DRIVER_ERROR("[driver]: configuring serial port: error: %s\n", strerror(errno))
        LOG_DRIVER_EVENT("[driver]: closing serial port\n")
        if (close(fd) < 0) {
            LOG_DRIVER_EVENT("[driver]: closed serial port: error: %s\n", strerror(errno))
        } else {
            LOG_DRIVER_EVENT("[driver]: closed serial port\n")
        }
        return CONFIGURATION_ERROR;
    } else {
        LOG_DRIVER_EVENT("[driver]: configured serial port\n")
    }

    return fd;
}

int close_serial_port(int fd) {
    int ret = SUCCESS;
    sleep(1);
    LOG_DRIVER_EVENT("[driver]: restoring serial port configuration\n")
    if (tcsetattr(fd, TCSANOW, &old_configuration) == -1) {
        LOG_DRIVER_ERROR("[driver]: restoring serial port configuration: error: %s\n", strerror(errno))
        ret = CONFIGURATION_ERROR;
    } else {
        LOG_DRIVER_EVENT("[driver]: restored serial port configuration\n")
    }

    LOG_DRIVER_EVENT("[driver]: closing serial port\n")
    if (close(fd) < 0) {
        LOG_DRIVER_EVENT("[driver]: closed serial port: error: %s\n", strerror(errno))
        if (ret == SUCCESS) ret = IO_ERROR;
    } else {
        LOG_DRIVER_EVENT("[driver]: closed serial port\n")
    }

    return ret;
}
