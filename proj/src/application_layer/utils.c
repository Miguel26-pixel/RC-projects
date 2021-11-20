#include "include/utils.h"
#include <sys/stat.h>
#include <stdio.h>

size_t get_file_size(char *file_path) {
    size_t fs;
    struct stat st;
    if (stat(file_path, &st) < 0) {
        perror("stat");
        return -1;
    } else {
        fs = st.st_size;
    }

    return fs;
}