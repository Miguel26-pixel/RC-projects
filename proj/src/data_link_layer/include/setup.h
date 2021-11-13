#include <termios.h>

int openSerialPort(int argc, char **argv, struct termios *oldtio);

int closeSerialPort(struct termios *oldtio);

int connectToReader(void);

int connectToWriter(void);
