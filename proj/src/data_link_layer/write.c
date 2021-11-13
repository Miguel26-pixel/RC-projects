#include "include/write.h"

#include <stdio.h>
#include <unistd.h>

int send_supervision_message(unsigned char address, unsigned char control) {
    ssize_t res;
    unsigned char m;
    m = F;
    res = write(fd, &m, 1);
    m = address;
    res = write(fd, &m, 1);
    m = control;
    res = write(fd, &m, 1);
    m = address ^ control;
    res = write(fd, &m, 1);
    m = F;
    res = write(fd, &m, 1);

    printf("SEND ANSWER DONE\n");
    return 0;
}
