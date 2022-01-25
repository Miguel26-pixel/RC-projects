#ifndef PROJ_SRC_DATA_LINK_LAYER_INCLUDE_PORT_H_
#define PROJ_SRC_DATA_LINK_LAYER_INCLUDE_PORT_H_

#include <termios.h>

int open_serial_port(const char *path);
int close_serial_port(int fd);

#endif  // PROJ_SRC_DATA_LINK_LAYER_INCLUDE_PORT_H_
