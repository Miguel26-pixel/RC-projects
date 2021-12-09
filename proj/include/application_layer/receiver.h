#ifndef PROJ_SRC_APPLICATION_LAYER_INCLUDE_RECEIVER_H_
#define PROJ_SRC_APPLICATION_LAYER_INCLUDE_RECEIVER_H_

#include <sys/types.h>
#include "packet.h"

int receiver(const char *port);

int establish_connection_receiver(const char *path);

ssize_t get_file_info(int fd, unsigned char *packet);

ssize_t check_file_info(control_packet_t *start_packet, const unsigned char *packet);

int create_result_file(const char *path);

int terminate_connection_receiver(int fd);

int check_file_integrity(control_packet_t *start_packet, control_packet_t *end_packet, size_t no_bytes_written);

size_t
read_data(int fd, size_t file_size, control_packet_t *end_packet, unsigned char *packet, unsigned char *data, int fd2);

#endif //PROJ_SRC_APPLICATION_LAYER_INCLUDE_RECEIVER_H_
