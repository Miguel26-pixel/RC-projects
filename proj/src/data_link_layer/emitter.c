#include "include/connection.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    if (argc < 2 || argc > 2) {
        fprintf(stderr, RED"Usage:\temitter SerialPort\n\tex: emitter /dev/ttyS1\n"RESET);
        exit(1);
    }

    printf(YELLOW"[emitter]: started: using serial port: %s\n"RESET, argv[1]);

    int fd = ll_open(argv[1], true);
    if (fd < 0) {
        exit(-1);
    }

    const char *message[] = {
            "Esta",
            "mensagem",
            "tem",
            "várias",
            "partes",
            "espero",
            "que",
            "cheguem",
            "todas.",
            "\n"
    };

    size_t message_length = sizeof(message) / sizeof(message[0]);
    for (int i = 0; i < message_length; ++i) {
        printf("MESSAGE: %d/%zu\n", i + 1, message_length);
        if (ll_write(fd, message[i], strlen(message[i]) + 1) < 0) {
            fprintf(stderr, RED"[emitter]: max attemps reached: aborting\n"RESET);
            exit(-1);
        }
    }

    if (ll_close(fd, true) < 0) {
        exit(-1);
    }

    return 0;
}
