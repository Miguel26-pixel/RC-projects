#include "include/write.h"

#include <stdio.h>
#include <unistd.h>

int send_supervision_message(unsigned char address, unsigned char control) {
    ssize_t res;
    unsigned char message[] = {F, address, control, address ^ control, F};
    res = write(fd, message, sizeof(message));

    printf("SEND ANSWER DONE\n");
    return 0;
}
