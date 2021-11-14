#include <termios.h>

int open_serial_port(const char *path, struct termios *oldtio);

int close_serial_port(struct termios *oldtio);

int connect_to_receiver(void);

int connect_to_writer(void);
