#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "read.h"
#include "write.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define MAX_ATTEMPTS 3
#define TIMEOUT 3

extern int fd;

int open_serial_port(int argc, char** argv, struct termios * oldtio);

int close_serial_port (struct termios * oldtio);

int connect_to_receiver();

int connect_to_writer();
