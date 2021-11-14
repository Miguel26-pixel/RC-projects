#include "include/connection.h"

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ATTEMPTS 3
#define TIMEOUT 3

extern int fd;

ssize_t read_supervision_message(unsigned char *address, unsigned char *control) {

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
        } else if (s == READ_ADDRESS) {
            *address = b;
            s = READ_CONTROL;
        } else if (s == READ_CONTROL) {
            *control = b;
            s = READ_BCC;
        } else if (s == READ_BCC && b == (unsigned char) (*address ^ *control)) {
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
        unsigned char a, c;
        if (read_supervision_message(&a, &c) < 0 || a != ADDRESS_RECEIVER_EMITTER || c != UA) interrupt_count++;
        else return 0;
    }

    return -1;
}

int connect_to_writer(void) {
    unsigned char a, c;
    if (read_supervision_message(&a, &c) < 0 || a != ADDRESS_EMITTER_RECEIVER || c != SET) return -1;
    if (send_supervision_message(ADDRESS_RECEIVER_EMITTER, UA) < 0) return -1;
    return 0;
}

int disconnect_from_receiver(void) {
    send_supervision_message(ADDRESS_EMITTER_RECEIVER, DISC);
    unsigned char a, c;
    if (read_supervision_message(&a, &c) < 0 || a != ADDRESS_RECEIVER_EMITTER || c != DISC) return -1;
    if (send_supervision_message(ADDRESS_EMITTER_RECEIVER, UA) < 0) return -1;
    return 0;
}

int disconnect_from_writer(void) {
    if (send_supervision_message(ADDRESS_RECEIVER_EMITTER, DISC) < 0) return -1;
    return 0;
}

ssize_t send_information(const unsigned char *data, size_t nb, bool n) {
    unsigned char c = (unsigned char) (n << 6);
    unsigned char header[] = {FLAG, ADDRESS_EMITTER_RECEIVER, c, (unsigned char) ADDRESS_EMITTER_RECEIVER ^ c};

    if (write(fd, header, sizeof(header)) < 0) return -1;

    ssize_t res = write(fd, data, nb);
    if (res < 0) return -1;

    unsigned char bcc2;
    calculateBCC(data, &bcc2, nb);

    unsigned char footer[] = {bcc2, FLAG};
    if (write(fd, footer, sizeof(footer)) < 0) return -1;

    return res;
}

int calculateBCC(const unsigned char *data, unsigned char *bcc2, size_t size) {
    if (data == NULL || bcc2 == NULL) return -1;
    *bcc2 = 0;
    for (int j = 0; j < size - 2; ++j) *bcc2 = (unsigned char) (*bcc2 ^ data[j]);
    return 0;
}

ssize_t read_information(unsigned char *data, size_t size, bool n) {
    typedef enum {
        READ_FLAG_START, READ_ADDRESS, READ_CONTROL, READ_BCC1, READ_DATA, READ_BCC2
    } state_t;

    state_t s = READ_FLAG_START;

    unsigned char b, c, bcc2;
    unsigned char buf[256];
    unsigned int i = 0;
    bool done = false;
    while (true) {
        //COMBACK: Better to reorganize the loop to avoid reading sometimes.
        if (s < READ_BCC2) {
            if (read(fd, &b, 1) < 0) return -1;
            else alarm(0);
        }

        if (s == READ_FLAG_START && b == FLAG) {
            s = READ_ADDRESS;
        } else if (s == READ_ADDRESS && b == ADDRESS_EMITTER_RECEIVER) {
            s = READ_CONTROL;
        } else if (s == READ_CONTROL && (b == CI(n) || b == DISC)) {
            c = b;
            s = READ_BCC1;
        } else if (s == READ_BCC1 && b == (unsigned char) (ADDRESS_EMITTER_RECEIVER ^ c)) {
            if (c == CI(n)) s = READ_DATA;
            else if (c == DISC) return 1;
        } else if (s == READ_DATA) {
            if (i > size) return -2;
            data[i] = b;
            ++i;
            if (b == FLAG) {
                done = true;
                s = READ_BCC2;
                if (calculateBCC(data, &bcc2, i - 2) < 0) {}
            }
        } else if (s == READ_BCC2) {
            if (data[i - 2] == bcc2) break;
            else return -3;
        } else {
            s = READ_FLAG_START;
            done = false;
        }
    }
    size_t ds = (i - 2) <= size ? i - 2 : size;
    memcpy(buf, data, i - 2);
    return (ssize_t) ds;
}