#include "include/connection.h"
#include "include/setup.h"
#include "include/alarm.h"

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define MAX_ATTEMPTS 3
#define TIMEOUT 3

#define FLAG 0x7E
#define ADDRESS_EMITTER_RECEIVER 0x03
#define ADDRESS_RECEIVER_EMITTER 0x01

#define SET 0x03
#define UA  0x07
#define DISC 0x0B
#define CI(n) ((n) << 6)
#define RR(n) (0x05 | ((n) << 7))
#define REJ(n) (0x01 | ((n) << 7))

static bool n = false;

ssize_t read_supervision_message(int fd, unsigned char *address, unsigned char *control) {

    typedef enum {
        READ_START_FLAG, READ_ADDRESS, READ_CONTROL, READ_BCC, READ_END_FLAG
    } state_t;

    state_t s = READ_START_FLAG;
    unsigned char b;
    while (true) {
        // COMBACK: Failures?
        if (read(fd, &b, 1) < 0) return -1;
        else alarm(0);
        if (s == READ_START_FLAG && b == FLAG) {
            s = READ_ADDRESS;
        } else if (s == READ_ADDRESS) {
            *address = b;
            s = READ_CONTROL;
        } else if (s == READ_CONTROL) {
            *control = b;
            s = READ_BCC;
        } else if (s == READ_BCC && b == (unsigned char) (*address ^ *control)) {
            s = READ_END_FLAG;
        } else if (s == READ_END_FLAG && b == FLAG) {
            return 0;
        } else {
            s = READ_START_FLAG;
        }
    }
}

ssize_t send_supervision_message(int fd, unsigned char address, unsigned char control) {
    unsigned char message[] = {FLAG, address, control, address ^ control, FLAG};
    return write(fd, message, sizeof(message));
}

int connect_to_receiver(int fd) {
    int interrupt_count = 1;

    while (interrupt_count <= MAX_ATTEMPTS) {
        send_supervision_message(fd, ADDRESS_EMITTER_RECEIVER, SET);
        printf("[connecting]: attempt: %d\n", interrupt_count);
        alarm(TIMEOUT);
        unsigned char a, c;
        if (read_supervision_message(fd, &a, &c) < 0 || a != ADDRESS_RECEIVER_EMITTER || c != UA) interrupt_count++;
        else return 0;
    }

    return -1;
}

int connect_to_writer(int fd) {
    unsigned char a, c;
    if (read_supervision_message(fd, &a, &c) < 0 || a != ADDRESS_EMITTER_RECEIVER || c != SET) return -1;
    if (send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, UA) < 0) return -1;
    return 0;
}

int disconnect_from_receiver(int fd) {
    send_supervision_message(fd, ADDRESS_EMITTER_RECEIVER, DISC);
    unsigned char a, c;
    if (read_supervision_message(fd, &a, &c) < 0 || a != ADDRESS_RECEIVER_EMITTER || c != DISC) return -1;
    if (send_supervision_message(fd, ADDRESS_EMITTER_RECEIVER, UA) < 0) return -1;
    return 0;
}

int disconnect_from_writer(int fd) {
    if (send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, DISC) < 0) return -1;
    return 0;
}

ssize_t send_information(int fd, const unsigned char *data, size_t nb, bool n) {
    unsigned char c = (unsigned char) (n << 6);
    unsigned char header[] = {FLAG, ADDRESS_EMITTER_RECEIVER, c, (unsigned char) ADDRESS_EMITTER_RECEIVER ^ c};

    if (write(fd, header, sizeof(header)) < 0) return -1;

    ssize_t res = write(fd, data, nb);
    if (res < 0) return -1;

    unsigned char bcc2;
    calculateBCC(data, &bcc2, nb);

    unsigned char footer[] = {bcc2, FLAG};
    if (write(fd, footer, sizeof(footer)) < 0) return -1;

    return res;
}

int calculateBCC(const unsigned char *data, unsigned char *bcc2, size_t size) {
    if (data == NULL || bcc2 == NULL) return -1;
    *bcc2 = 0;
    for (int j = 0; j < size - 2; ++j) *bcc2 = (unsigned char) (*bcc2 ^ data[j]);
    return 0;
}

ssize_t read_information(int fd, unsigned char *data, size_t size, bool n) {
    typedef enum {
        READ_FLAG_START, READ_ADDRESS, READ_CONTROL, READ_BCC1, READ_DATA, READ_BCC2
    } state_t;

    state_t s = READ_FLAG_START;

    unsigned char b, c, bcc2;
    unsigned char buf[256];
    unsigned int i = 0;
    bool done = false;
    while (true) {
        //COMBACK: Better to reorganize the loop to avoid reading sometimes.
        if (s < READ_BCC2) {
            if (read(fd, &b, 1) < 0) return -1;
            else alarm(0);
        }

        if (s == READ_FLAG_START && b == FLAG) {
            s = READ_ADDRESS;
        } else if (s == READ_ADDRESS && b == ADDRESS_EMITTER_RECEIVER) {
            s = READ_CONTROL;
        } else if (s == READ_CONTROL && (b == CI(n) || b == DISC)) {
            c = b;
            s = READ_BCC1;
        } else if (s == READ_BCC1 && b == (unsigned char) (ADDRESS_EMITTER_RECEIVER ^ c)) {
            if (c == CI(n)) s = READ_DATA;
            else if (c == DISC) return -5;
        } else if (s == READ_DATA) {
            if (i > size) return -2;
            data[i] = b;
            ++i;
            if (b == FLAG) {
                done = true;
                s = READ_BCC2;
                if (calculateBCC(data, &bcc2, i - 2) < 0) {}
            }
        } else if (s == READ_BCC2) {
            if (data[i - 2] == bcc2) break;
            else return -3;
        } else {
            s = READ_FLAG_START;
            done = false;
        }
    }
    size_t ds = (i - 2) <= size ? i - 2 : size;
    memcpy(buf, data, i - 2);
    return (ssize_t) ds;
}

int ll_open(const char *path, bool isEmitter) {
    if (path == NULL) return -1;

    const char *s = isEmitter ? "emitter" : "receiver";
    const char *d = isEmitter ? "receiver" : "emitter";

    int fd = open_serial_port(path);
    if (fd < 0) {
        fprintf(stderr, RED"[%s]: opening serial port: error: %s"RESET, s, strerror(errno));
        return -1;
    } else {
        printf("[%s]: opening serial port: success\n"RESET, s);
    }

    setup_alarm();
    printf("[%s]: configuring alarm: success\n"RESET, s);

    printf(YELLOW"[%s]: connecting to %s\n"RESET, s, d);
    int r = isEmitter ? connect_to_receiver(fd) : connect_to_writer(fd);
    if (r < 0) {
        fprintf(stderr, RED"[%s]: connecting to %s: error\n"RESET, s, d);
        if (close_serial_port(0) < 0) {
            fprintf(stderr, RED"[%s]: closing serial port: error: %s"RESET, s, strerror(errno));
        } else {
            printf("[%s]: closing serial port: success\n"RESET, s);
        }
        return -1;
    } else {
        printf("[%s]: connecting to %s: success\n"RESET, s, d);
    }
    printf("FD IS: %d\n", fd);
    return fd;
}

int ll_close(int fd, bool isEmitter) {
    int r = isEmitter ? disconnect_from_receiver(fd) : disconnect_from_writer(fd);
    if (r < 0) {
        fprintf(stderr, RED"[emitter]: disconnecting: error: %s"RESET, strerror(errno));
    } else {
        printf("[emitter]: disconnecting: success\n"RESET);
    }

    if (close_serial_port(0) < 0) {
        fprintf(stderr, RED"[emitter]: closing serial port: error: %s"RESET, strerror(errno));
        return -1;
    } else {
        printf("[emitter]: closing serial port: success\n"RESET);
    }
    return 0;
}

ssize_t ll_read(int fd, void *data, size_t nb) {
    printf(YELLOW"[receiver]: reading message (R = %d)\n"RESET, n);

    ssize_t r = read_information(fd, data, nb, n);
    if (r == -5) return -2;
    else if (r < 0) {
        fprintf(stderr, RED"[receiver]: reading message: error\n"RESET);
        if (send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, REJ(n)) < 0) {
            fprintf(stderr, RED"[receiver]: sending confirmation: error\n"RESET);
        } else {
            printf("[receiver]: sending reject: success\n"RESET);
        }
        return -1;
    } else {
        printf("[receiver]: read message: %s\n"RESET, (char *) data);
    }

    if (send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, RR(!n)) < 0) {
        fprintf(stderr, RED"[receiver]: sending confirmation: error\n"RESET);
        return -1;
    } else {
        printf("[receiver]: sending confirmation: success\n"RESET);
        n = !n;
        return r;
    }
}

ssize_t ll_write(int fd, const void *data, size_t nb) {
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
                continue;
            }
            break;
        }
    }
    if (tries == MAX_ATTEMPTS) {
        fprintf(stderr, RED"[emitter]: sending message: error: timeout\n"RESET);
        return -1;
    }
    return -1;
}
