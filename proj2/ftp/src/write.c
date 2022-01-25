#include "../include/write.h"

#include <string.h>
#include <unistd.h>

int sendCommand(int fd, const char *command, const char *argument) {
    if (command) write(fd, command, strlen(command));
    write(fd, " ", 1);
    if (argument) write(fd, argument, strlen(argument));
    write(fd, "\n", 1);
    return 0;
}