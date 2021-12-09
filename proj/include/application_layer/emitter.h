#ifndef PROJ_SRC_APPLICATION_LAYER_INCLUDE_EMITTER_H_
#define PROJ_SRC_APPLICATION_LAYER_INCLUDE_EMITTER_H_

#include "packet.h"

int emitter(const char *file_path, const char *port);

int check_input_file(const char *path);

int establish_connection_emitter(const char *path);

int send_file_info(int fd, control_packet_t *packet, unsigned char *pa, size_t n);

int send_data(int fd2, int fd, unsigned char *pa, unsigned char *buf, size_t n, size_t no_packets);

int terminate_connection_emitter(int fd);

int send_end_packet(int fd, control_packet_t *packet, unsigned char *pa, size_t n);

#endif //PROJ_SRC_APPLICATION_LAYER_INCLUDE_EMITTER_H_
