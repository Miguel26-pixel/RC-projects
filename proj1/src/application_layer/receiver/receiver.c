#include "../../../include/application_layer/receiver.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

#include "../../../include/data_link_layer/link_layer.h"
#include "../../../include/errors/error_nos.h"
#include "../../../include/gui/gui.h"

int main(int argc, char **argv) {
    srand(time(NULL));
    if (argc < 2 || argc > 2) {
        fprintf(stderr, RED"Usage:\treceiver SerialPort\n\tex: receiver /dev/ttyS1\n"RESET);
        exit(1);
    }
    init_al_logs();

    LOG_AL_EVENT("[receiver]: started: using serial port: %s\n", argv[1])

    return receiver(argv[1]);
}

int receiver(const char *port) {
    int fd = establish_connection_receiver(port);

    int r;
    control_packet_t start_packet, end_packet;
    memset(&start_packet, 0, sizeof(start_packet));
    memset(&end_packet, 0, sizeof(end_packet));

    unsigned char packet[PACKET_SIZE];
    unsigned char data[BUFFER_SIZE];

    if ((r = get_file_info(fd, packet) < 0)) return r;

    if ((r = check_file_info(&start_packet, packet) < 0)) return r;

    int fd2 = create_result_file(start_packet.file_name);
    if (fd2 < 0) return fd2;

    size_t no_bytes_written = read_data(fd, start_packet.file_size, &end_packet, packet, data, fd2);
    if (no_bytes_written < 0) return no_bytes_written;

    check_file_integrity(&start_packet, &end_packet, no_bytes_written);

    free(start_packet.file_name);
    free(end_packet.file_name);

    if ((r = terminate_connection_receiver(fd) < 0)) return r;

    return SUCCESS;
}

size_t
read_data(int fd, size_t file_size, control_packet_t *end_packet, unsigned char *packet, unsigned char *data, int fd2) {
    size_t no_bytes_received = 0;

    size_t i = 0;
    ssize_t n;
    ssize_t r;
    while (true) {
        LOG_AL_EVENT("[receiver]: reading packet %zu\n", i)
        r = ll_read(fd, packet, sizeof(packet));

        LOG_AL_EVENT("[receiver]: processing packet %zu\n", i)
        if (r == EOF_DISCONNECT) {
            LOG_AL_EVENT("[receiver]: got last packet\n")
            break;
        } else if (r < 0) {
            LOG_AL_ERROR("[receiver]: error reading %zd\n", r)
            continue;
        } else if (r >= 0) {
            if (packet[0] == C_DATA) {
                LOG_AL_EVENT("[receiver]: processing data packet\n")
                n = process_data_packet(packet, r, data, i % 256, sizeof(data));
                if (n < 0) {
                    LOG_AL_ERROR("[receiver]: processing data packet: error\n")
                    return -7;
                } else {
                    LOG_AL_EVENT("[receiver]: processed data packet\n")
                    LOG_AL_EVENT("[receiver]: writing data into result file\n")
                    if (write(fd2, data, n) < 0) {
                        LOG_AL_ERROR("[receiver]: writing data into result file: error\n")
                        return -8;
                    }
                    no_bytes_received += n;
                }
            } else {
                LOG_AL_EVENT("[receiver]: processing control packet\n")
                if (process_control_packet(packet, r, end_packet, false) < 0) {
                    LOG_AL_ERROR("[receiver]: processing control packet: error\n")
                    return -9;
                } else {
                    LOG_AL_EVENT("[receiver]: processed control packet\n")
                }
            }
            ++i;
            LOG_AL_EVENT("[receiver]: receiving packet %zu\n", i)
            print_progress_bar((double) no_bytes_received / file_size, "RECEIVING", false);
        }
    }

    LOG_AL_EVENT("[receiver]: received\n")
    print_progress_bar(1, "RECEIVED", false);

    fsync(fd2);
    close(fd2);
    return no_bytes_received;
}

int check_file_integrity(control_packet_t *start_packet, control_packet_t *end_packet, size_t no_bytes_written) {
    LOG_AL_EVENT("[receiver]: checking integrity\n")
    print_progress_bar(1, "CHECKING INTEGRITY", false);
    if (start_packet->file_size != end_packet->file_size ||
        (start_packet->file_name != NULL && end_packet->file_name != NULL &&
         strcmp(start_packet->file_name, end_packet->file_name) != 0)) {
        LOG_AL_ERROR("[receiver]: integrity not ok\n")
        // return -10;
    } else {
        LOG_AL_EVENT("[receiver]: integrity ok\n")
        print_progress_bar(1, "INTEGRITY OK", false);
    }
    return 0;
}

int terminate_connection_receiver(int fd) {
    LOG_AL_EVENT("[receiver]: terminating\n")
    print_progress_bar(1, "TERMINATING", false);
    if (ll_close(fd, false) < 0) {
        LOG_AL_ERROR("[receiver]: terminating: error\n")
        return -11;
    } else {
        LOG_AL_EVENT("[receiver]: terminated\n")
        print_progress_bar(1, "TERMINATED", false);
    }
    return 0;
}

int create_result_file(const char *path) {
    LOG_AL_EVENT("[receiver]: creating result file\n")
    print_progress_bar(0, "CREATING RESULT FILE", false);
    int fd2 = open(path, O_WRONLY | O_CREAT, 0644);
    if (fd2 < 0) {
        LOG_AL_ERROR("[receiver]: creating result file: error: %s\n", strerror(errno))
        return -5;
    } else {
        LOG_AL_EVENT("[receiver]: result file ok\n")
        print_progress_bar(0, "RESULT FILE OK", false);
    }
    return fd2;
}

ssize_t check_file_info(control_packet_t *start_packet, const unsigned char *packet) {
    ssize_t r = 0;

    LOG_AL_EVENT("[receiver]: checking file info\n")
    print_progress_bar(0, "CHECKING FILE INFO", false);
    LOG_AL_EVENT("[receiver]: processing start packet\n")
    if (process_control_packet(packet, r, start_packet, true) < 0) {
        LOG_AL_ERROR("[receiver]: processing control packet: error\n")
        return -4;
    } else {
        LOG_AL_EVENT("[receiver]: start packet ok\n")
        print_progress_bar(0, "FILE INFO OK", false);
    }
    return r;
}

ssize_t get_file_info(int fd, unsigned char *packet) {
    LOG_AL_EVENT("getting file info\n")
    print_progress_bar(0, "GETTING FILE INFO", false);
    ssize_t r = ll_read(fd, packet, sizeof(packet));
    if (r < 0) {
        LOG_AL_ERROR("[receiver]: getting file info: error\n")
        return -3;
    } else {
        LOG_AL_EVENT("[receiver]: got file info\n")
        print_progress_bar(0, "GOT FILE INFO", false);
    }
    return r;
}

int establish_connection_receiver(const char *path) {
    LOG_AL_EVENT("[receiver]: establishing connection\n")
    print_progress_bar(0, "ESTABLISHING CONNECTION", false);
    int fd = ll_open(path, false);
    if (fd < 0) {
        LOG_AL_ERROR("[receiver]: establishing connection: error\n")
        return -2;
    } else {
        LOG_AL_EVENT("[receiver]: established connection\n")
        print_progress_bar(0, "CONNECTION ESTABLISHED", false);
    }
    return fd;
}
