#include "include/setup.h"
#include "include/connection.h"

#include <termios.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int fd;

int main(int argc, char **argv) {
    if (argc < 2 || argc > 2) {
        fprintf(stderr, RED"Usage:\treceiver SerialPort\n\tex: receiver /dev/ttyS1\n"RESET);
        exit(1);
    }

    printf("[receiver]: started: using serial port: %s\n"RESET, argv[1]);

    struct termios old_configuration;
    if (open_serial_port(argv[1], &old_configuration) < 0) {
        fprintf(stderr, RED"[receiver]: opening serial port: error: %s"RESET, strerror(errno));
        exit(-1);
    } else {
        printf("[receiver]: opening serial port: success\n"RESET);
    }

    // setup_alarm();
    // puts("[receiver]: configuring alarm: success"RESET);

    printf(YELLOW"[receiver]: connecting to emitter\n"RESET);
    if (connect_to_writer() < 0) {
        fprintf(stderr, RED"[receiver]: connecting to emitter: error\n"RESET);
        if (close_serial_port(&old_configuration) < 0) {
            fprintf(stderr, RED"[receiver]: closing serial port: error: %s"RESET, strerror(errno));
        } else {
            printf("[receiver]: closing serial port: success\n"RESET);
        }
        exit(-1);
    } else {
        printf("[receiver]: connecting to emitter: success\n"RESET);
    }

    char res[10][256];
    bool n = false;
    int i = 0;
    while (true && i < 10) {
        bool force_error = (rand() % 2 == 0);
        memset(res[i], 0, sizeof(res[i]));
        printf(YELLOW"[receiver]: reading message (R = %d)\n"RESET, n);

        if (read_information(res[i], sizeof(res[i]), n) < 0 || force_error) {
            fprintf(stderr, RED"[receiver]: reading message: error\n"RESET);
            if (send_supervision_message(ADDRESS_RECEIVER_EMITTER, REJ(n)) < 0) {
                fprintf(stderr, RED"[receiver]: sending confirmation: error\n"RESET);
            } else {
                printf("[receiver]: sending reject: success\n"RESET);
                continue;
            }
        } else {
            printf("[receiver]: read message: %s\n"RESET, res[i]);
        }

        if (send_supervision_message(ADDRESS_RECEIVER_EMITTER, RR(!n)) < 0) {
            fprintf(stderr, RED"[receiver]: sending confirmation: error\n"RESET);
        } else {
            printf("[receiver]: sending confirmation: success\n"RESET);
            n = !n;
            ++i;
        }
    }

    printf("[receiver]: full message is:"RESET);
    for (int j = 0; j < i; ++j) printf(" %s"RESET, res[j]);
    printf(".\n"RESET);

    if (close_serial_port(&old_configuration) < 0) {
        fprintf(stderr, RED"[receiver]: closing serial port: error: %s"RESET, strerror(errno));
    } else {
        printf("[receiver]: closing serial port: success\n"RESET);
        exit(-1);
    }

    return 0;
}
