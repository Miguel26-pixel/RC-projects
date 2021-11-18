#include "include/aplication.h"

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../data_link_layer/include/errnos.h"
#include "../data_link_layer/include/link_layer.h"

size_t get_file_size(char * file_path) {
    size_t fs;
    struct stat st;
    if (stat(file_path, &st) < 0) return -1;
    else fs = st.st_size;

    return fs;
}

size_t get_control_package(unsigned char * control_package, char * file_path, char * file_name) {
    unsigned file_size = get_file_size(file_path);

    control_package[0] = CSTART;
    control_package[1] = TSIZE;
    control_package[2] = sizeof(unsigned);
    memcpy(control_package+3,&file_size,sizeof(unsigned));
    
    if (file_name != NULL) {
        control_package[7] = TNAME;
        control_package[8] = strlen(file_name) + 1;
        memcpy(control_package+9,file_name,strlen(file_name) + 1);
    }

    return 0;
}

int send_control_package(int fd, char * file_path, char * file_name) {
    size_t num_bytes;
    unsigned char * control_package;

    if (file_name == NULL) {
        num_bytes = 3 + sizeof(unsigned);
    } else {
        num_bytes = 3 + sizeof(unsigned) + 2 + strlen(file_name) + 1;
    }

    control_package = (unsigned char *) malloc(num_bytes);

    bzero((void *)control_package,num_bytes);

    get_control_package(control_package,file_path,file_name);

    printf(YELLOW"[emitter]: sending start package\n"RESET);
    
    size_t r = ll_write(fd, control_package, num_bytes);

    for (int i = 0; i < num_bytes; i++) {
        printf("%x\n",control_package[i]);
    }

    if (r >= 0) { printf(YELLOW"[emitter]: sending start package: SUCCESS\n"RESET); }
    else { printf(RED"[emitter]: sending start package: ERROR\n"RESET); }
}

int read_control_package(int fd) {
    
    unsigned char res[20];
    bzero(res,20);
    ll_read(fd, res, sizeof(res));

    for (int i = 0; i < sizeof(res); i++) {
        printf("%x\n",res[i]);
    }
}
