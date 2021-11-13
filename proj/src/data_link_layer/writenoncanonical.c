/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "include/alarm.h"
#include "include/setup.h"
#include "include/connection.h"

#include <stdbool.h>
#include <stdlib.h>

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

volatile int STOP = false;
int fd;

int main(int argc, char **argv) {
    if (argc < 2 || argc > 2) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

    int res;
    struct termios oldtio;
    unsigned char buf[255];
    int i, sum = 0, speed = 0, interrupt_count = 0;

    open_serial_port(argv[1], &oldtio);

    int done = 1;

    setup_alarm();

    if (connect_to_reader() != 0) {
        close_serial_port(&oldtio);
        return 1;
    }

    puts("SET DONE");

    unsigned char s[] = "HelloWorld";
    send_i(s, sizeof(s), 0);
    puts("I DONE");
    read_rr(1);
    puts("RR1 DONE");

    close_serial_port(&oldtio);

    return 0;
}
