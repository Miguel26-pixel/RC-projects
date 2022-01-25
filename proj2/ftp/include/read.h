#ifndef FEUP_RC_TL2_READ_H
#define FEUP_RC_TL2_READ_H
#include <stdio.h>
int readPasvResponse(int fd, char **ip, int *port);
int downloadFile(int fd, char *path, int size);
int extractResponseCode(const char *line, int *code);
int extractFileSize(char *line, int *size);
int read_response(int fd, char *buf);
int getResponseCode(int fd, int *code);
#endif //FEUP_RC_TL2_READ_H
