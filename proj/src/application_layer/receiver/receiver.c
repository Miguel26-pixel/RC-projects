#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

#include "../../../include/data_link_layer/link_layer.h"
#include "../../../include/errors/errnos.h"
#include "../../../include/application_layer/packet.h"
#include "../../../include/gui/gui.h"


int main(int argc, char **argv) {
    if (argc < 2 || argc > 2) {
        fprintf(stderr, RED"Usage:\treceiver SerialPort\n\tex: receiver /dev/ttyS1\n"RESET);
        exit(1);
    }
    init_al_logs();

    LOG_AL_EVENT("[receiver]: started: using serial port: %s\n", argv[1])

    LOG_AL_EVENT("[receiver]: establishing connection\n")
    print_progress_bar(0, "ESTABLISHING CONNECTION", false);
    int fd = ll_open(argv[1], false);
    if (fd < 0) {
        LOG_AL_ERROR("[receiver]: establishing connection: error\n")
        exit(2);
    } else {
        LOG_AL_EVENT("[receiver]: established connection\n")
        print_progress_bar(0, "CONNECTION ESTABLISHED", false);
    }

    ssize_t r;
    control_packet_t start_packet, end_packet;
    memset(&start_packet, 0, sizeof(start_packet));
    memset(&end_packet, 0, sizeof(end_packet));

    LOG_AL_EVENT("getting file info\n")
    print_progress_bar(0, "GETTING FILE INFO", false);
    unsigned char packet[PACKET_SIZE];
    r = ll_read(fd, packet, sizeof(packet));
    if (r < 0) {
        LOG_AL_ERROR("[receiver]: getting file info: error\n")
        exit(3);
    } else {
        LOG_AL_EVENT("[receiver]: got file info\n")
        print_progress_bar(0, "GOT FILE INFO", false);
    }

    LOG_AL_EVENT("[receiver]: checking file info\n")
    print_progress_bar(0, "CHECKING FILE INFO", false);

    LOG_AL_EVENT("[receiver]: processing start packet\n")
    if (process_control_packet(packet, r, &start_packet, true) < 0) {
        LOG_AL_ERROR("[receiver]: processing control packet: error\n")
        exit(4);
    } else {
        LOG_AL_EVENT("[receiver]: start packet ok\n")
        print_progress_bar(0, "FILE INFO OK", false);
    }

    LOG_AL_EVENT("[receiver]: creating result file\n")
    print_progress_bar(0, "CREATING RESULT FILE", false);
    int fd2 = open(start_packet.file_name, O_WRONLY | O_CREAT, 0644);
    if (fd2 < 0) {
        LOG_AL_ERROR("[receiver]: creating result file: error: %s\n", strerror(errno))
        exit(5);
    } else {
        LOG_AL_EVENT("[receiver]: result file ok\n")
        print_progress_bar(0, "RESULT FILE OK", false);
    }

    size_t no_bytes_received = 0;

    size_t i = 0;
    size_t n;

    unsigned char data[BUFFER_SIZE];
    while (true) {
        LOG_AL_EVENT("[receiver]: reading packet %zu\n", i)
        r = ll_read(fd, packet, sizeof(packet));

        LOG_AL_EVENT("[receiver]: processing packet %zu\n", i)
        if (r == EOF_DISCONNECT) {
            LOG_AL_EVENT("[receiver]: got last packet\n")
            break;
        } else if (r < 0) {
            LOG_AL_ERROR("[receiver]: error reading\n")
            exit(6);
        } else if (r >= 0) {
            if (packet[0] == C_DATA) {
                LOG_AL_EVENT("[receiver]: processing data packet\n")
                n = process_data_packet(packet, r, data, i % 256, sizeof(data));
                if (n < 0) {
                    LOG_AL_ERROR("[receiver]: processing data packet: error\n")
                    exit(7);
                } else {
                    LOG_AL_EVENT("[receiver]: processed data packet\n")
                    LOG_AL_EVENT("[receiver]: writing data into result file\n")
                    if (write(fd2, data, n) < 0) {
                        LOG_AL_ERROR("[receiver]: writing data into result file: error\n")
                        exit(8);
                    }
                    no_bytes_received += n;
                }
            } else {
                LOG_AL_EVENT("[receiver]: processing control packet\n")
                if (process_control_packet(packet, r, &end_packet, false) < 0) {
                    LOG_AL_ERROR("[receiver]: processing control packet: error\n")
                    exit(9);
                } else {
                    LOG_AL_EVENT("[receiver]: processed control packet\n")
                }
            }
            ++i;
            LOG_AL_EVENT("[receiver]: receiving packet %zu\n", i)
            print_progress_bar((double) no_bytes_received / start_packet.file_size, "RECEIVING", false);
        }
    }

    LOG_AL_EVENT("[receiver]: received\n")
    print_progress_bar(1, "RECEIVED", false);

    LOG_AL_EVENT("[receiver]: checking integrity\n")
    print_progress_bar(1, "CHECKING INTEGRITY", false);
    if (start_packet.file_size != end_packet.file_size ||
        (start_packet.file_name != NULL && end_packet.file_name != NULL &&
         strcmp(start_packet.file_name, end_packet.file_name) != 0)) {
        LOG_AL_ERROR("[receiver]: integrity not ok\n")
        exit(10);
    } else {
        LOG_AL_EVENT("[receiver]: integrity ok\n")
        print_progress_bar(1, "INTEGRITY OK", false);
    }

    free(start_packet.file_name);
    free(end_packet.file_name);

    LOG_AL_EVENT("[receiver]: terminating\n")
    print_progress_bar(1, "TERMINATING", false);
    if (ll_close(fd, false) < 0) {
        LOG_AL_ERROR("[receiver]: terminating: error\n")
        exit(11);
    } else {
        LOG_AL_EVENT("[receiver]: terminated\n")
        print_progress_bar(1, "TERMINATED", false);
    }

    return SUCCESS;
}
