#include "../include/connect.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int connectToSocket(const char *ip, int port) {
    struct sockaddr_in server_addr;

    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    int r = connect(fd, (const struct sockaddr *) &server_addr, sizeof(server_addr));
    if (r < 0) {
        perror("connect");
        return -2;
    }

    return fd;
}