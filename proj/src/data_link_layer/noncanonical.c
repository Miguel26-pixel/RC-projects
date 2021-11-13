#include "include/setup.h"
#include "include/connection.h"

#include <sys/types.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

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

volatile int STOP = false;

int fd;

int main(int argc, char **argv) {
    int res;
    unsigned char m;
    struct termios oldtio;
    unsigned char buf[255];

    open_serial_port(argv[1], &oldtio);

    connect_to_writer();

    int r = readI();

    if (r == 0) {
        printf("ANSWER = RR1\n");
        send_supervision_message(ARE, RR1);
    } else {
        printf("ANSWER = RR0\n");
        send_supervision_message(ARE, RR0);
    }

    close_serial_port(&oldtio);

    return 0;
}
