#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "../data_link_layer/include/link_layer.h"
#include "../data_link_layer/include/errnos.h"
#include "include/packet.h"
#include "include/utils.h"

#define PACKET_SIZE 5192
#define BUFFER_SIZE 2048

int main(int argc, char **argv) {
    unsigned char no = 0;
    if (argc < 3 || argc > 4) {
        fprintf(stderr, RED"Usage:\temitter SerialPort FilePath [FileName]\n\tex: emitter /dev/ttyS1\n"RESET);
        exit(1);
    }

    printf(YELLOW"[emitter]: started: using serial port: %s\n"RESET, argv[1]);

    int fd2 = open(argv[2], O_RDONLY);
    if (fd2 < 0) {
        exit(-1);
    }

    int fd = ll_open(argv[1], true);
    if (fd < 0) {
        exit(-1);
    }

    control_packet_t packet;
    packet.file_size = get_file_size(argv[2]);
    packet.file_name = "transmitted";

    unsigned char pa[PACKET_SIZE];
    unsigned char buf[BUFFER_SIZE];
    size_t n;

    n = assemble_control_packet(packet, true, pa, sizeof(pa));
    if (n < 0) {
        exit(-1);
    }

    if (ll_write(fd, pa, n) < 0) {
        exit(-1);
    }

    size_t no_packets = packet.file_size / BUFFER_SIZE;
    if (no_packets * BUFFER_SIZE < packet.file_size) ++no_packets;

    for (size_t i = 0; i < no_packets; ++i) {
        n = read(fd2, buf, sizeof(buf));
        if (n <= 0) {
            exit(-1);
        }

        n = assemble_data_packet(buf, n, pa, sizeof(pa), i % 256);
        if (n < 0) {
            exit(-1);
        }

        if (ll_write(fd, pa, n) < 0) {
            exit(-1);
        }
    }

    n = assemble_control_packet(packet, false, pa, sizeof(pa));
    if (n < 0) {
        exit(-1);
    }

    if (ll_write(fd, pa, n) < 0) {
        exit(-1);
    }

    if (ll_close(fd, true) < 0) {
        exit(-1);
    }

    return 0;
}
