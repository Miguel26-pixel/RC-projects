#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "include/link_layer.h"
#include "include/errnos.h"

int main(int argc, char **argv) {
    if (argc < 2 || argc > 2) {
        fprintf(stderr, RED"Usage:\treceiver SerialPort\n\tex: receiver /dev/ttyS1\n"RESET);
        exit(1);
    }

    printf("[receiver]: started: using serial port: %s\n"RESET, argv[1]);

    int fd = ll_open(argv[1], false);
    if (fd < 0) exit(-1);

    char res[20][256];
    int i = 0;
    ssize_t r;
    while (true) {
        memset(res[i], 0, sizeof(res[i]));
        r = ll_read(fd, res[i], sizeof(res[i]));
        if (r == EOF_DISCONNECT) break;
        else if (r < 0);
        else if (r >= 0) ++i;
    }

    printf("[receiver]: full message is:"RESET);
    for (int j = 0; j < i; ++j) printf(" %s"RESET, res[j]);
    printf("\n"RESET);

    if (ll_close(fd, false) < 0) exit(-1);
    return 0;
}
