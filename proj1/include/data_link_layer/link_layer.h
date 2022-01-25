#ifndef  PROJ_SRC_DATA_LINK_LAYER_INCLUDE_LINK_LAYER_H_
#define  PROJ_SRC_DATA_LINK_LAYER_INCLUDE_LINK_LAYER_H_

#include <sys/types.h>
#include <stdbool.h>

int ll_open(const char *path, bool is_emitter);
int ll_close(int fd, bool is_emitter);
ssize_t ll_read(int fd, void *data, size_t nb);
ssize_t ll_write(int fd, const void *data, size_t nb);

#endif  // PROJ_SRC_DATA_LINK_LAYER_INCLUDE_LINK_LAYER_H_
