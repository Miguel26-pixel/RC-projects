#ifndef FEUP_RC_SETUP_H
#define FEUP_RC_SETUP_H

#include <termios.h>

int open_serial_port(const char *path);
int close_serial_port(int fd);

#endif //FEUP_RC_SETUP_H