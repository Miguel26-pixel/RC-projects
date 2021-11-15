#ifndef FEUP_RC_CONNECTION_H
#define FEUP_RC_CONNECTION_H

#include <sys/types.h>
#include <stdbool.h>

#define RED "\x1b[1;31m"
#define YELLOW "\x1b[1;33m"
#define RESET "\x1b[1;0m"

ssize_t read_supervision_message(int fd, unsigned char *address, unsigned char *control);
ssize_t send_supervision_message(int fd, unsigned char address, unsigned char control);

int connect_to_receiver(int fd);
int connect_to_emitter(int fd);

int disconnect_from_receiver(int fd);
int disconnect_from_emitter(int fd);

int calculateBCC(const unsigned char *data, unsigned char *bcc2, size_t size);

ssize_t send_information(int fd, const unsigned char *data, size_t nb, bool no);
ssize_t read_information(int fd, unsigned char *data, size_t size, bool no);

int ll_open(const char *path, bool is_emitter);
int ll_close(int fd, bool is_emitter);
ssize_t ll_read(int fd, void *data, size_t nb);
ssize_t ll_write(int fd, const void *data, size_t nb);

#endif //FEUP_RC_CONNECTION_H
