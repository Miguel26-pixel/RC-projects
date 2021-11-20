#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "../../../include/data_link_layer/link_layer.h"
#include "../../../include/application_layer/packet.h"
#include "../../../include/application_layer/utils.h"
#include "../../../include/gui/gui.h"

#define PACKET_SIZE 5192
#define BUFFER_SIZE 2048

int main(int argc, char **argv) {

    if (argc < 3 || argc > 4) {
        fprintf(stderr, RED"Usage:\temitter SerialPort FilePath [FileName]\n\tex: emitter /dev/ttyS1\n"RESET);
        exit(1);
    }

    init_al_logs();

    LOG_AL_EVENT("[emitter]: started: using serial port: %s\n", argv[1]);

    LOG_AL_EVENT("[emitter]: checking input file\n");
    print_progress_bar(0, "CHECKING INPUT FILE", false);
    int fd2 = open(argv[2], O_RDONLY);
    if (fd2 < 0) {
        LOG_AL_ERROR("[emitter]: checking input file: error\n");
        print_progress_bar(0, "INPUT FILE NOT OK", true);
        exit(-1);
    } else {
        LOG_AL_EVENT("[emitter]: input file ok\n");
        print_progress_bar(0, "INPUT FILE OK", false);
    }

    LOG_AL_EVENT("[emitter]: establishing connection\n");
    print_progress_bar(0, "ESTABLISHING CONNECTION", false);
    int fd = ll_open(argv[1], true);
    if (fd < 0) {
        LOG_AL_ERROR("[emitter]: establishing connection: error\n");
        print_progress_bar(0, "CONNECTION NOT ESTABLISHED", true);
        exit(-1);
    } else {
        LOG_AL_EVENT("[emitter]: connection established\n");
        print_progress_bar(0, "CONNECTION ESTABLISHED", false);
    }

    control_packet_t packet;
    packet.file_size = get_file_size(argv[2]);
    packet.file_name = "transmitted";

    unsigned char pa[PACKET_SIZE];
    unsigned char buf[BUFFER_SIZE];
    size_t n;

    LOG_AL_EVENT("[emitter]: sending file info\n");
    print_progress_bar(0, "SENDING FILE INFO", false);
    n = assemble_control_packet(packet, true, pa, sizeof(pa));
    if (n < 0 || ll_write(fd, pa, n) < 0) {
        LOG_AL_ERROR("[emitter]: sending file info: error")
        print_progress_bar(0, "ERROR SENDING FILE INFO", true);
        exit(-1);
    } else {
        LOG_AL_EVENT("[emitter]: sent file info\n");
        print_progress_bar(0, "SENT FILE INFO", false);
    }

    size_t no_packets = packet.file_size / BUFFER_SIZE;
    if (no_packets * BUFFER_SIZE < packet.file_size) ++no_packets;

    for (size_t i = 0; i < no_packets; ++i) {
        n = read(fd2, buf, sizeof(buf));
        if (n <= 0) {
            print_progress_bar(0, "ERROR READING FROM FILE", true);
            LOG_AL_ERROR("[emitter]: reading from file: error\n");
            exit(-1);
        } else {
            LOG_AL_EVENT("[emitter]: read %zu bytes from file\n", n);
        }

        n = assemble_data_packet(buf, n, pa, sizeof(pa), i % 256);
        if (n < 0 || ll_write(fd, pa, n) < 0) {
            LOG_AL_ERROR("[emitter]: sending data packet\n");
            print_progress_bar(0, "ERROR SENDING", true);
            exit(-1);
        } else {
            LOG_AL_EVENT("[emitter]: sent packet %zu/%zu\n", i, no_packets);
            print_progress_bar(((double) i / no_packets), "SENDING", false);
        }
    }

    LOG_AL_EVENT("[emitter]: assembling end control packet\n");
    n = assemble_control_packet(packet, false, pa, sizeof(pa));
    if (n < 0 || ll_write(fd, pa, n) < 0) {
        LOG_AL_ERROR("[emitter]: sending end control packet: error\n");
        print_progress_bar(0, "ERROR SENDING", true);
        exit(-1);
    } else {
        LOG_AL_EVENT("[emitter]: sent end control packet\n");
        print_progress_bar(1, "SENT", false);
    }

    LOG_AL_EVENT("[emitter]: disconnecting\n");
    print_progress_bar(1, "TERMINATING", false);
    if (ll_close(fd, true) < 0) {
        LOG_AL_ERROR("[emitter]: disconnecting: error\n");
        print_progress_bar(1, "ERROR TERMINATING", false);
        exit(-1);
    } else {
        LOG_AL_EVENT("[emitter]: disconnected\n");
        print_progress_bar(1, "TERMINATED", false);
    }

    return 0;
}
