#include "include/link_layer.h"

#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "include/connection.h"
#include "include/port.h"
#include "include/alarm.h"
#include "include/errnos.h"

extern bool n;

int ll_open(const char *path, bool is_emitter) {
    if (path == NULL) return NULL_POINTER_ERROR;

    const char *source, *destination;
    if (is_emitter) {
        destination = "receiver";
        source = "emitter";
    } else {
        source = "receiver";
        destination = "emitter";
    }

    int fd = open_serial_port(path);
    if (fd < 0) {
        fprintf(stderr, RED"[%s]: opening serial port: error: %s"RESET, source, strerror(errno));
        return fd;
    } else {
        printf("[%s]: opening serial port: success\n"RESET, source);
    }

    setup_alarm();
    printf("[%s]: configuring alarm: success\n"RESET, source);

    printf(YELLOW"[%s]: connecting to %s\n"RESET, source, destination);
    int r = is_emitter ? connect_to_receiver(fd) : connect_to_emitter(fd);
    if (r < 0) {
        fprintf(stderr, RED"[%s]: connecting to %s: error\n"RESET, source, destination);
        if (close_serial_port(fd) < 0) {
            fprintf(stderr, RED"[%s]: closing serial port: error: %s"RESET, source, strerror(errno));
        } else {
            printf("[%s]: closing serial port: success\n"RESET, source);
        }
        return -1;
    } else {
        printf("[%s]: connecting to %s: success\n"RESET, source, destination);
    }
    return fd;
}

int ll_close(int fd, bool is_emitter) {
    const char *source;
    int r;

    if (is_emitter) {
        source = "emitter";
        r = disconnect_from_receiver(fd);
    } else {
        source = "receiver";
        r = disconnect_from_emitter(fd);
    }
    if (r < 0) {
        fprintf(stderr, RED"[%s]: disconnecting: error: %s"RESET, source, strerror(errno));
    } else {
        printf("[%s]: disconnecting: success\n"RESET, source);
    }

    r = close_serial_port(fd);
    if (r < 0) {
        fprintf(stderr, RED"[%s]: closing serial port: error: %s"RESET, source, strerror(errno));
        return r;
    } else {
        printf("[%s]: closing serial port: success\n"RESET, source);
    }

    return SUCCESS;
}

ssize_t ll_read(int fd, void *data, size_t nb) {
    if (data == NULL) return NULL_POINTER_ERROR;

    printf(YELLOW"[receiver]: reading message (R = %d)\n"RESET, n);

    ssize_t r = read_information(fd, data, nb, n);
    if (r == EOF_DISCONNECT) {
        printf("[receiver]: received disconnect\n"RESET);
        return EOF_DISCONNECT;
    } else if (r == OUT_OF_ORDER) {
        n = !n;
    } else if (r < 0) {
        fprintf(stderr, RED"[receiver]: reading message: error\n"RESET);
        if (send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, REJ(n)) < 0) {
            fprintf(stderr, RED"[receiver]: sending confirmation: error\n"RESET);
        } else {
            printf("[receiver]: sending reject: success\n"RESET);
        }
        return r;
    } else {
        printf("[receiver]: read message: %s\n"RESET, (char *) data);
    }

    if (send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, RR(!n)) < 0) {
        fprintf(stderr, RED"[receiver]: sending confirmation: error\n"RESET);
    } else {
        printf("[receiver]: sending confirmation: success\n"RESET);
        n = !n;
    }
    return r;
}

ssize_t ll_write(int fd, const void *data, size_t nb) {
    if (data == NULL) return NULL_POINTER_ERROR;

    int tries = 1;
    while (tries <= MAX_ATTEMPTS) {
        alarm(0);
        printf("TRY: %d/%d\n", tries, MAX_ATTEMPTS);
        printf(YELLOW"[emitter]: sending message (R = %d): message: %s\n"RESET, n, (char *) data);

        ssize_t s = send_information(fd, data, nb + 1, n);
        if (s < 0) {
            fprintf(stderr, RED"[emitter]: sending message: error\n"RESET);
            continue;
        } else {
            printf("[emitter]: sending message: success\n"RESET);
        }

        alarm(TIMEOUT);

        unsigned char a, c;
        if (read_supervision_message(fd, &a, &c) < 0 || a != ADDRESS_RECEIVER_EMITTER) {
            ++tries;
            fprintf(stderr, RED"[emitter]: reading confirmation: error\n"RESET);
        } else {
            alarm(0);
            if (c == RR(!n)) {
                printf("[emitter]: reading confirmation: success\n"RESET);
                n = !n;
                return s;
            } else if (c == REJ(n)) {
                fprintf(stderr, RED"[emitter]: reading confirmation: error: message rejected\n"RESET);
            }
        }
    }
    return TOO_MANY_ATTEMPTS;
}