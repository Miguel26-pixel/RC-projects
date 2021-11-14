#include "include/alarm.h"
#include "include/setup.h"
#include "include/connection.h"

#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int fd;

#define MAX_ATTEMPTS 3
#define TIMEOUT 3

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
    int tries = 1;
    size_t message_length = sizeof(message) / sizeof(message[0]);
    while (i < message_length && tries <= MAX_ATTEMPTS) {
        printf("MESSAGE: %d/%zu\n", i + 1, message_length);
        tries = 1;
        while (tries <= MAX_ATTEMPTS) {
            alarm(0);
            printf("TRY: %d/%d\n", tries, MAX_ATTEMPTS);
            printf(YELLOW"[emitter]: sending message (R = %d): message: %s\n"RESET, n, message[i]);

            if (send_information(message[i], strlen(message[i]) + 1, n) < 0) {
                fprintf(stderr, RED"[emitter]: sending message: error\n"RESET);
                continue;
            } else {
                printf("[emitter]: sending message: success\n"RESET);
            }

            alarm(TIMEOUT);

            unsigned char a, c;
            if (read_supervision_message(&a, &c) < 0 || a != ADDRESS_RECEIVER_EMITTER) {
                ++tries;
                fprintf(stderr, RED"[emitter]: reading confirmation: error\n"RESET);
            } else {
                alarm(0);
                if (c == RR(!n)) {
                    printf("[emitter]: reading confirmation: success\n"RESET);
                    n = !n;
                    ++i;
                } else if (c == REJ(n)) {
                    fprintf(stderr, RED"[emitter]: reading confirmation: error: message rejected\n"RESET);
                    continue;
                }
                break;
            }
        }
    }

    if (tries == MAX_ATTEMPTS) {
        fprintf(stderr, RED"[emitter]: sending message: error: timeout\n"RESET);
    }

    if (disconnect_from_receiver() < 0) {
        fprintf(stderr, RED"[emitter]: disconnecting: error: %s"RESET, strerror(errno));
    } else {
        printf("[emitter]: disconnecting: success\n"RESET);
    }

    if (close_serial_port(&old_configuration) < 0) {
        fprintf(stderr, RED"[emitter]: closing serial port: error: %s"RESET, strerror(errno));
    } else {
        printf("[emitter]: closing serial port: success\n"RESET);
        exit(-1);
    }

    return 0;
}
