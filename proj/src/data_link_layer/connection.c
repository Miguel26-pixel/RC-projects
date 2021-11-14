#include "include/connection.h"

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>


#define MAX_ATTEMPTS 3
#define TIMEOUT 3

#define FLAG   0x7E
#define ADDRESS_EMITTER_RECEIVER 0x03
#define ADDRESS_RECEIVER_EMITTER 0x01

#define SET 0x03
#define UA  0x07
#define CI(n) (n << 6)
#define RR(n) (0x05 | (n << 7))
#define REJ(n) (0x01 | (n << 7))

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
        if (s == READ_START_FLAG && b == FLAG) {
            s = READ_ADDRESS;
        } else if (s == READ_ADDRESS && b == address) {
            s = READ_CONTROL;
        } else if (s == READ_CONTROL && b == control) {
            s = READ_BCC;
        } else if (s == READ_BCC && b == (unsigned char) (address ^ control)) {
            s = READ_END_FLAG;
        } else if (s == READ_END_FLAG && b == FLAG) {
            return 0;
        } else {
            s = READ_START_FLAG;
        }
    }
}

ssize_t send_supervision_message(unsigned char address, unsigned char control) {
    unsigned char message[] = {FLAG, address, control, address ^ control, FLAG};
    return write(fd, message, sizeof(message));
}

int connect_to_receiver(void) {
    int interrupt_count = 0;

    while (interrupt_count < MAX_ATTEMPTS) {
        send_supervision_message(ADDRESS_EMITTER_RECEIVER, SET);
        printf("[connecting]: attempt: %d\n", interrupt_count);
        alarm(TIMEOUT);
        if (read_supervision_message(ADDRESS_RECEIVER_EMITTER, UA) < 0) interrupt_count++;
        else return 0;
    }

    return -1;
}

int connect_to_writer(void) {
    if (read_supervision_message(ADDRESS_EMITTER_RECEIVER, SET) < 0) return -1;
    if (send_supervision_message(ADDRESS_RECEIVER_EMITTER, UA) < 0) return -1;
    return 0;
}

int send_i(const unsigned char *d, size_t nb, unsigned n) {
    ssize_t res;
    unsigned char c = (unsigned char) (n << 6);
    unsigned char header[] = {FLAG, ADDRESS_EMITTER_RECEIVER, c, (unsigned char) ADDRESS_EMITTER_RECEIVER ^ c};

    if (write(fd, header, sizeof(header)) < 0) return -1;

    if (write(fd, d, nb) < 0) return -1;

    unsigned char bcc2 = 0;
    for (int i = 0; i < nb; ++i) bcc2 = (unsigned char) (bcc2 ^ d[i]);

    unsigned char footer[] = {bcc2, FLAG};
    if (write(fd, footer, sizeof(footer)) < 0) return -1;

    return 0;
}

int read_information(unsigned char *dest, size_t size, bool n) {
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

        if (s == READ_FLAG_START && b == FLAG) {
            s = READ_ADDRESS;
        } else if (s == READ_ADDRESS && b == ADDRESS_EMITTER_RECEIVER) {
            s = READ_CONTROL;
            //COMBACK: Use n parameter to validate this field
        } else if (s == READ_CONTROL && (b == CI(n))) {
            c = b;
            s = READ_BCC1;
        } else if (s == READ_BCC1 && b == (unsigned char) (ADDRESS_EMITTER_RECEIVER ^ c)) {
            s = READ_DATA;
        } else if (s == READ_DATA) {
            // COMBACK: Buffer overflow
            dest[i] = b;
            ++i;
            if (b == FLAG) {
                done = true;
                s = READ_BCC2;
                // COMBACK: Most likely a function
                bcc2 = 0;
                for (int j = 0; j < i - 2; ++j) bcc2 = (unsigned char) (bcc2 ^ dest[j]);
            }
            //COMBACK: Implement forgiving validation: if header is valid, send reject.
        } else if (s == READ_BCC2 && dest[i - 2] == bcc2) {
            s = READ_FLAG_END;
        } else if (s == READ_FLAG_END && dest[i - 1] == FLAG) {
            memcpy(buf, dest, i - 2);
            return 0;
        } else {
            s = READ_FLAG_START;
        }
    }
}