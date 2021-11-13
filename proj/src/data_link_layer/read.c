#include "include/read.h"

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

int read_supervision_message(unsigned char address, unsigned char control) {

    typedef enum {
        READ_START_FLAG, READ_ADDRESS, READ_CONTROL, READ_BCC, READ_END_FLAG
    } state_t;

    state_t s = READ_START_FLAG;
    unsigned char b;
    while (true) {
        // COMBACK: Failures?
        if (read(fd, &b, 1) < 0) return -1;
        else alarm(0);
        if (s == READ_START_FLAG && b == F) {
            s = READ_ADDRESS;
        } else if (s == READ_ADDRESS && b == address) {
            s = READ_CONTROL;
        } else if (s == READ_CONTROL && b == control) {
            s = READ_BCC;
        } else if (s == READ_BCC && b == (unsigned char) (address ^ control)) {
            s = READ_END_FLAG;
        } else if (s == READ_END_FLAG && b == F) {
            return 0;
        } else {
            s == READ_START_FLAG;
        }
    }
    return 0;
}
