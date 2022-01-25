#define _GNU_SOURCE

#include "../include/download.h"
#include "../include/authentication.h"
#include "../include/get_ip.h"
#include "../include/parse.h"
#include "../include/read.h"
#include "../include/write.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define FTP_PORT 21
#include "../include/gui.h"

int download(char *cmd) {

    print_progress_bar(1, "Done", false, NULL);

    return 0;
}
