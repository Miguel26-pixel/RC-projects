#ifndef FEUP_RC_LINK_LAYER_H
#define FEUP_RC_LINK_LAYER_H

#include <sys/types.h>
#include <stdbool.h>

#define RED "\x1b[1;31m"
#define YELLOW "\x1b[1;33m"
#define RESET "\x1b[1;0m"

int ll_open(const char *path, bool is_emitter);
int ll_close(int fd, bool is_emitter);
ssize_t ll_read(int fd, void *data, size_t nb);
ssize_t ll_write(int fd, const void *data, size_t nb);

#endif //FEUP_RC_LINK_LAYER_H
