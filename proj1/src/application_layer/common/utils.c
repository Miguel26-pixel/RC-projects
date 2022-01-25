#include "../../../include/application_layer/utils.h"
#include <sys/stat.h>

size_t get_file_size(const char *file_path) {
    size_t fs;
    struct stat st;
    if (stat(file_path, &st) < 0) { return -1; }
    else { fs = st.st_size; }

    return fs;
}
