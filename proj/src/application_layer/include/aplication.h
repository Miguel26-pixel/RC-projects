#ifndef PROJ_SRC_DATA_LINK_LAYER_INCLUDE_APPLICATION_H_
#define PROJ_SRC_DATA_LINK_LAYER_INCLUDE_APPLICATION_H_

#define CSTART 0x02
#define CEND 0x03
#define TNAME  0x01
#define TSIZE 0x00

#include <stddef.h>

size_t get_file_size(char *file_path);

int send_control_package(int fd, char *file_path, char *file_name);

int read_control_package(int fd);

size_t get_control_package(unsigned char *control_package, char *file_path, char *file_name);

#endif //PROJ_SRC_DATA_LINK_LAYER_INCLUDE_APPLICATION_H_