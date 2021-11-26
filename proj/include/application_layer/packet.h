#ifndef PROJ_SRC_APPLICATION_LAYER_INCLUDE_PACKET_H_
#define PROJ_SRC_APPLICATION_LAYER_INCLUDE_PACKET_H_

#include <sys/types.h>
#include <stdbool.h>

#define C_DATA 1
#define C_START 2
#define C_END 3

#define T_FILE_SIZE 0
#define T_FILE_NAME 1

#define PACKET_SIZE 2048
#define BUFFER_SIZE 1024

typedef struct {
    char *file_name;
    size_t file_size;
} control_packet_t;

int process_control_packet(const unsigned char *bytes, size_t nb, control_packet_t *packet, bool is_start);
size_t process_data_packet(const unsigned char *bytes, size_t nb, unsigned char *dest, unsigned char no, size_t nbd);
size_t assemble_control_packet(control_packet_t packet, bool is_start, unsigned char *dest, size_t dnb);
size_t assemble_data_packet(void *src, size_t nb, unsigned char *dest, size_t dnb, unsigned char no);

#endif  // PROJ_SRC_APPLICATION_LAYER_INCLUDE_PACKET_H_
