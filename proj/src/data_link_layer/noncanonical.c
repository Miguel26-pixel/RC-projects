#include "include/setup.h"
#include "include/connection.h"

#include <sys/types.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

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

int fd;

int main(int argc, char **argv) {
    int res;
    unsigned char m;
    struct termios oldtio;
    unsigned char buf[255];

    open_serial_port(argv[1], &oldtio);

    connect_to_writer();
    bool n = false;
    int nt = 9;
    while (true && nt) {
        memset(buf, 0, sizeof(buf));
        read_information(buf, 0);
        printf("READ: %s\n", (char *) buf);
        send_supervision_message(ARE, n == 0 ? RR1 : RR0);
        n = !n;
        --nt;
    }

    close_serial_port(&oldtio);

    return 0;
}
