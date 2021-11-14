#include "include/alarm.h"
#include "include/setup.h"
#include "include/connection.h"

#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

int fd;

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

        if (send_information(message[i], strlen(message[i]) + 1, n) < 0) {
            fprintf(stderr, RED"[emitter]: sending message: error\n"RESET);
        } else {
            printf("[emitter]: sending message: success\n"RESET);
        }

        unsigned char a, c;
        if (read_supervision_message(&a, &c) < 0 || a != ADDRESS_RECEIVER_EMITTER) {
            fprintf(stderr, RED"[emitter]: reading confirmation: error\n"RESET);
        } else {
            if (c == RR(!n)) {
                printf("[emitter]: reading confirmation: success\n"RESET);
                n = !n;
                ++i;
            } else if (c == REJ(n)) {
                fprintf(stderr, RED"[emitter]: reading confirmation: error: message rejected\n"RESET);
                continue;
            }
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
