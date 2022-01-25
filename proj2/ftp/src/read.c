#include "../include/read.h"

#include <fcntl.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/gui.h"

#define MAX_CHARS_PERCENTAGE 27

int readPasvResponse(int fd, char **ip, int *port) {
    char buf[8192];
    read(fd, buf, sizeof(buf));
    int code;
    extractResponseCode(buf, &code);
    if (code != 227) {
        print_progress_bar(0, "Failed with code: ", true, &code);
        return -1;
    }

    char *ip_start = strchr(buf, '(') + 1;
    char *s = strsep(&ip_start, ")");
    for (int i = 0; i < 3; ++i) *strchr(s, ',') = '.';
    *ip = strsep(&s, ",");

    char *p1 = strsep(&s, ",");
    char *p2 = strsep(&s, ",");

    char *e1, *e2;
    int msb = (int) strtol(p1, &e1, 10);
    int lsb = (int) strtol(p2, &e2, 10);

    if (*e1 != '\0' || *e2 != '\0') return -1;
    *port = (msb * 256 + lsb);

    return 0;
}

int downloadFile(int fd, char *path, int size) {
    const char *file_name = basename(path);
    char buf[1024];
    int fd2 = open(file_name, O_CREAT | O_WRONLY, 0644);
    int nb, tb = 0;
    while ((nb = read(fd, buf, sizeof(buf))) > 0) {
        write(fd2, buf, nb);
        tb += nb;
        double p = (double) tb / size;
        print_progress_bar(p, "Downloading", false, NULL);
    }

    if (tb == size) {
        print_progress_bar(1, "Downloaded", false, NULL);
    } else {
        print_progress_bar((double) tb / size, "Error downloading", true, NULL);
    }

    close(fd2);
    return 0;
}

int extractResponseCode(const char *line, int *code) {
    char code_string[4] = "";
    strncpy(code_string, line, 3);
    char *end;
    *code = (int) strtol(code_string, &end, 10);
    return *end == '\0';
}

int extractFileSize(char *line, int *size) {
    strsep(&line, "(");
    if (!line) return -1;
    char *s = strsep(&line, " ");

    char *end;
    *size = (int) strtol(s, &end, 10);
    return *end == '\0';
}

int read_response(int fd, char *buf) {
    if (read(fd, buf, 8192)) {}
    return 0;
}

int getResponseCode(int fd, int *code) {
    char buf[8192] = "";
    if (read_response(fd, buf) < 0) return -1;
    if (extractResponseCode(buf, code) < 0) return -2;
    return 0;
}