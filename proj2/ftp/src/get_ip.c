#include "../include/get_ip.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int getIP(const char *hostname, char *ip_address, size_t nb) {
    struct addrinfo hints, *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    int r = getaddrinfo(hostname, NULL, &hints, &result);
    if (r != 0) {
        perror("getaddrinfo");
        return -1;
    }

    void *p = &((struct sockaddr_in *) result->ai_addr)->sin_addr;
    inet_ntop(result->ai_family, p, ip_address, nb);

    freeaddrinfo(result);
    return 0;
}
