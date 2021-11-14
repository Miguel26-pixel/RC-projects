#ifndef FEUP_RC_SETUP_H
#define FEUP_RC_SETUP_H

#include <termios.h>

int open_serial_port(const char *path, struct termios *old_configuration);
int close_serial_port(const struct termios *old_configuration);

#endif //FEUP_RC_SETUP_H