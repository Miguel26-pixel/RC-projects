#include "include/write.h"

#include <stdio.h>
#include <unistd.h>

int send_set(void) {
    interrupt = 0;
    ssize_t res;
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

    printf("%zd bytes written\n", res);
}

int send_supervision_message(unsigned char c) {
    ssize_t res;
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
