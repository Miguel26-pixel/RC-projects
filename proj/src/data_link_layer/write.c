#include "include/write.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

int send_set(void) {
    interrupt = 0;
    int res;
    unsigned char m;

    m = F;
    res = write(fd, &m, 1);

    m = AER;
    res = write(fd, &m, 1);

    m = SET;
    res = write(fd, &m, 1);

    m = AER ^ SET;
    res = write(fd, &m, 1);

    m = F;
    res = write(fd, &m, 1);

    printf("%d bytes written\n", res);
}

int send_supervision_message(unsigned char c) {
    int res;
    unsigned char m;
    m = F;
    res = write(fd, &m, 1);
    m = ARE;
    res = write(fd, &m, 1);
    m = c;
    res = write(fd, &m, 1);
    m = ARE ^ c;
    res = write(fd, &m, 1);
    m = F;
    res = write(fd, &m, 1);

    printf("SEND ANSWER DONE\n");
}
