/*Non-Canonical Input Processing*/

#include <termios.h>
#include <stdio.h>
#include <string.h>

#include "include/alarm.h"
#include "include/setup.h"
#include "include/connection.h"

#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#define FLAG   0x7E
#define ADDRESS_EMITTER_RECEIVER 0x03
#define ADDRESS_RECEIVER_EMITTER 0x01
#define SET 0x03
#define UA  0x07

#define SET 0x03
#define UA  0x07
#define CI(n) (n << 6)
#define RR(n) (0x05 | (n << 7))
#define REJ(n) (0x01 | (n << 7))


int fd;

#define RED "\x1b[1;31m"
#define YELLOW "\x1b[1;33m"
#define RESET "\x1b[1;0m"

int main(int argc, char **argv) {
    if (argc < 2 || argc > 2) {
        fprintf(stderr, RED"Usage:\temitter SerialPort\n\tex: emitter /dev/ttyS1\n"RESET);
        exit(1);
    }

    printf(YELLOW"[emitter]: started: using serial port: %s\n"RESET, argv[1]);

    struct termios old_configuration;
    if (open_serial_port(argv[1], &old_configuration) < 0) {
        fprintf(stderr, RED"[emitter]: opening serial port: error: %s\n"RESET, strerror(errno));
        exit(-1);
    } else {
        printf("[emitter]: opening serial port: success\n"RESET);
    }

    setup_alarm();
    printf("[emitter]: configuring alarm: success\n"RESET);

    printf(YELLOW"[emitter]: connecting to receiver\n"RESET);
    if (connect_to_receiver() < 0) {
        fprintf(stderr, RED"[emitter]: connecting to receiver: error\n"RESET);
        if (close_serial_port(&old_configuration) < 0) {
            fprintf(stderr, RED"[emitter]: closing serial port: error: %s\n"RESET, strerror(errno));
        } else {
            printf("[emitter]: closing serial port: success\n"RESET);
        }
        exit(-1);
    } else {
        printf("[emitter]: connecting to receiver: success\n"RESET);
    }

    const char *message[] = {"Esta", "mensagem", "tem", "várias", "partes", "espero", "que", "cheguem", "todas.", "\n"};

    bool n = false;
    int i = 0;
    while (true && i < 10) {
        printf(YELLOW"[emitter]: sending message (R = %d): message: %s\n"RESET, n, message[i]);

        if (send_i(message[i], strlen(message[i]) + 1, n) < 0) {
            fprintf(stderr, RED"[emitter]: sending message: error\n"RESET);
        } else {
            printf("[emitter]: sending message: success\n"RESET);
        }

        if (read_supervision_message(ADDRESS_RECEIVER_EMITTER, RR(!n)) < 0) {
            fprintf(stderr, RED"[emitter]: reading confirmation: error\n"RESET);
        } else {
            printf("[emitter]: reading confirmation: success\n"RESET);
            n = !n;
            ++i;
        }
    }

    if (close_serial_port(&old_configuration) < 0) {
        fprintf(stderr, RED"[emitter]: closing serial port: error: %s"RESET, strerror(errno));
    } else {
        printf("[emitter]: closing serial port: success\n"RESET);
        exit(-1);
    }

    return 0;
}
