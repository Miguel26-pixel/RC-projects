#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "include/setup.h"

#define F 0x7E
#define AER 0x03
#define ARE 0x01
#define SET 0x03
#define UA 0x07

#define CI0 0x00
#define CI1 0x40

#define RR0 0x05
#define RR1 0x85

#define REJ0 0x01
#define REJ1 0x81


volatile int STOP = FALSE;

int fd;
int interrupt;

int readI(int fd) {
    int res;
    char m, a, c;
    unsigned char buf[255];
    res = read(fd, &m, 1);

    if (m != F) puts("ERROR FLAG");
    res = read(fd, &a, 1);

    if (a != AER) puts("ERROR A");
    res = read(fd, &c, 1);

    if (c != CI0) { if (c != CI1) puts("ERROR Casd"); }
    res = read(fd, &m, 1);

    if (m != (char) (a ^ c)) puts("ERROR BCC");

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

int main(int argc, char **argv) {
    int res;
    unsigned char m;
    struct termios oldtio, newtio;
    unsigned char buf[255];

    openSerialPort(argc, argv, &oldtio);

    connectToWriter(UA);

    int r = readI(fd);

    if (r == 0) {
        printf("ANSWER = RR1\n");
        send_SU(RR1);
    } else {
        printf("ANSWER = RR0\n");
        send_SU(RR0);
    }

    closeSerialPort(&oldtio);

    return 0;
}
