#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../data_link_layer/include/link_layer.h"
#include "include/aplication.h"
#include "../data_link_layer/include/errnos.h"

int main(int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, RED"Usage:\temitter SerialPort FilePath [FileName]\n\tex: emitter /dev/ttyS1\n"RESET);
        exit(1);
    }

    printf(YELLOW"[emitter]: started: using serial port: %s\n"RESET, argv[1]);

    int fd = ll_open(argv[1], true);
    if (fd < 0) exit(-1);

    send_control_package(fd, argv[2], (argc == 4) ? argv[3] : NULL);





    /*int fd = ll_open(argv[1], true);
    if (fd < 0) exit(-1);

    const char *message[] = {
            "Esta",
            "mensagem",
            "tem",
            "várias",
            "partes",
            "e",
            "caracteres",
            "especiais",
            "como",
            "\x7e",
            "ou",
            "\x7d",
            "espero",
            "que",
            "cheguem",
            "todas.",
            "\n"
    };

    ssize_t r;
    size_t message_length = sizeof(message) / sizeof(message[0]);
    for (int i = 0; i < message_length; ++i) {
        printf("MESSAGE: %d/%zu\n", i + 1, message_length);
        r = ll_write(fd, message[i], strlen(message[i]) + 1);
        if (r < 0) {
            if (r == TOO_MANY_ATTEMPTS) fprintf(stderr, RED"[emitter]: max attempts reached: aborting\n"RESET);
            exit(-1);
        }
    }

    if (ll_close(fd, true) < 0) exit(-1);*/

    return 0;
}
