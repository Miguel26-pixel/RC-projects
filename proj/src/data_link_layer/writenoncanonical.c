/*Non-Canonical Input Processing*/

#include <termios.h>
#include <stdio.h>
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

int fd;

int main(int argc, char **argv) {
    if (argc < 2 || argc > 2) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

    struct termios oldtio;
    open_serial_port(argv[1], &oldtio);

    setup_alarm();

    if (connect_to_reader() != 0) {
        close_serial_port(&oldtio);
        return 1;
    }

    const char *strs[] = {"Esta", "mensagem", "tem", "várias", "partes", "espero", "que", "cheguem", "todas."};
    bool n = false;
    int nt = 0;
    while (true && nt < 9) {
        printf("Sending: %s\n", strs[nt]);
        send_i((unsigned char *) strs[nt], strlen(strs[nt]) + 1, n);
        read_supervision_message(ARE, (n == 0) ? RR1 : RR0);
        n = !n;
        ++nt;
    }
    close_serial_port(&oldtio);

    return 0;
}
