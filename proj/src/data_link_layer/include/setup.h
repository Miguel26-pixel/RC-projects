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

int openSerialPort(int argc, char **argv, struct termios *oldtio);

int closeSerialPort(struct termios *oldtio);

int connectToReader();

int connectToWriter();
