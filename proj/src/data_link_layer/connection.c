#include "include/connection.h"

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>


#define MAX_ATTEMPTS 3
#define TIMEOUT 3

#define F   0x7E
#define AER 0x03
#define ARE 0x01
#define SET 0x03
#define UA  0x07

#define CI0 0x00
#define CI1 0x40

#define RR0 0x05
#define RR1 0x85

#define REJ0 0x01
#define REJ1 0x81

extern int fd;

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
            s = READ_START_FLAG;
        }
    }
}

int send_supervision_message(unsigned char address, unsigned char control) {
    ssize_t res;
    unsigned char message[] = {F, address, control, address ^ control, F};
    res = write(fd, message, sizeof(message));

    return 0;
}

int connect_to_receiver(void) {
    int done = 1, interrupt_count = 0;

    while (interrupt_count < MAX_ATTEMPTS && done != 0) {
        send_supervision_message(AER, SET);
        printf("[connecting]: attempt: %d\n", interrupt_count);
        alarm(TIMEOUT);
        if ((done = read_supervision_message(ARE, UA)) != 0) interrupt_count++; else break;
    }

    if (interrupt_count == MAX_ATTEMPTS) return 1;

    return 0;
}

int connect_to_writer(void) {
    read_supervision_message(AER, SET);
    send_supervision_message(ARE, UA);
    return 0;
}

int send_i(const unsigned char *d, size_t nb, unsigned n) {
    ssize_t res;
    unsigned char m;

    m = F;
    res = write(fd, &m, 1);

    m = AER;
    res = write(fd, &m, 1);

    m = n == 0 ? CI0 : CI1;
    res = write(fd, &m, 1);

    m = (unsigned char) (AER ^ (n == 0 ? CI0 : CI1));
    res = write(fd, &m, 1);

    m = 0;
    for (int i = 0; i < nb; ++i) {
        res = write(fd, d + i, 1);
        m ^= d[i];
    }

    res = write(fd, &m, 1);

    m = F;
    res = write(fd, &m, 1);
    return 0;
}

int read_information(unsigned char *dest, size_t n) {
    typedef enum {
        READ_FLAG_START, READ_ADDRESS, READ_CONTROL, READ_BCC1, READ_DATA, READ_BCC2, READ_FLAG_END
    } state_t;

    state_t s = READ_FLAG_START;

    unsigned char b, c, bcc2;
    unsigned char buf[256];
    unsigned int i = 0;
    bool done = false;
    while (true) {
        //COMBACK: Better to reorganize the loop to avoid reading sometimes.
        if (!done) {
            if (read(fd, &b, 1) < 0) return -1;
            else alarm(0);
        }

        if (s == READ_FLAG_START && b == F) {
            s = READ_ADDRESS;
        } else if (s == READ_ADDRESS && b == AER) {
            s = READ_CONTROL;
            //COMBACK: Use n parameter to validate this field
        } else if (s == READ_CONTROL && (b == CI0 || b == CI1)) {
            c = b;
            s = READ_BCC1;
        } else if (s == READ_BCC1 && b == (unsigned char) (AER ^ c)) {
            s = READ_DATA;
        } else if (s == READ_DATA) {
            // COMBACK: Buffer overflow
            dest[i] = b;
            ++i;
            if (b == F) {
                done = true;
                s = READ_BCC2;
                // COMBACK: Most likely a function
                bcc2 = 0;
                for (int j = 0; j < i - 2; ++j) bcc2 = (unsigned char) (bcc2 ^ dest[j]);
            }
            //COMBACK: Implement forgiving validation: if header is valid, send reject.
        } else if (s == READ_BCC2 && dest[i - 2] == bcc2) {
            s = READ_FLAG_END;
        } else if (s == READ_FLAG_END && dest[i - 1] == F) {
            memcpy(buf, dest, i - 2);
            return 0;
        } else {
            s = READ_FLAG_START;
        }
    }
}