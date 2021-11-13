#include <termios.h>

int open_serial_port(int argc, char **argv, struct termios *oldtio);

int close_serial_port(struct termios *oldtio);

int connect_to_reader(void);

int connect_to_writer(void);
