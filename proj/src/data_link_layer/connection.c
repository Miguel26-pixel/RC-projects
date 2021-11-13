#include "include/connection.h"

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>


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
extern int interrupt;

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

int send_supervision_message(unsigned char address, unsigned char control) {
    ssize_t res;
    unsigned char message[] = {F, address, control, address ^ control, F};
    res = write(fd, message, sizeof(message));

    printf("SEND ANSWER DONE\n");
    return 0;
}

int connect_to_reader(void) {
    int done = 1, interrupt_count = 0;

    while (interrupt_count < MAX_ATTEMPTS && done != 0) {
        send_supervision_message(AER, SET);
        printf("Attempt - %d\n", interrupt_count);
        alarm(TIMEOUT);
        if ((done = read_supervision_message(ARE, UA)) != 0) interrupt_count++; else break;
    }

    if (interrupt_count == MAX_ATTEMPTS) {
        puts("INTERRUPTED - REACHED MAX TRIES");
        return 1;
    }

    return 0;
}

int connect_to_writer(void) {
    read_supervision_message(AER, SET);
    send_supervision_message(ARE, UA);
    return 0;
}

int send_i(const unsigned char *d, size_t nb, unsigned n) {
    interrupt = 0;
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

    printf("%zd bytes written\n", res);
    return 0;
}


int read_rr(int n) {
    ssize_t res;
    unsigned char a, c, m;
    res = read(fd, &m, 1);
    if (res <= 0) {
        interrupt = 1;
        return 1;
    }
    if (m != F) puts("ERROR FLAG");

    alarm(0);

    res = read(fd, &a, 1);
    if (a != ARE) puts("ERROR A");

    res = read(fd, &c, 1);;
    if (c != (n == 0 ? RR0 : RR1)) puts("ERROR C");

    res = read(fd, &m, 1);

    if (m != (a ^ c))
        printf("ERROR BCC: a - %x, c - %x, xor - %x, m - %x, bool - %d\n", a, c, (unsigned char) a ^ c, m,
               (unsigned char) (a ^ c) == (unsigned char) m);

    res = read(fd, &m, 1);
    if (m != F) puts("ERROR FLAG");

    return 0;
}

int readI() {
    ssize_t res;
    unsigned char m, a, c;
    unsigned char buf[255];
    res = read(fd, &m, 1);

    if (m != F) puts("ERROR FLAG");
    res = read(fd, &a, 1);

    if (a != AER) puts("ERROR A");
    res = read(fd, &c, 1);

    if (c != CI0) { if (c != CI1) puts("ERROR Casd"); }
    res = read(fd, &m, 1);

    if (m != (unsigned char) (a ^ c)) puts("ERROR BCC");

    int count = 0;
    unsigned char bcc2 = 0;
    do {
        res = read(fd, &m, 1);
        buf[count] = m;
        count++;
    } while (m != F);

    for (int i = 0; i < count - 2; i++) {
        bcc2 = bcc2 ^ buf[i];
    }

    if (bcc2 != buf[count - 2]) puts("ERROR BCC2");

    if (buf[count - 1] != F) puts("ERROR F2");

    printf("READ I DONE\n");

    if (c == CI0) { return 0; } else { return 1; }
}