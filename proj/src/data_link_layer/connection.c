#include "include/connection.h"
#include "include/port.h"
#include "include/alarm.h"

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

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

#define ESC11 0x7D
#define ESC12 0x5E
#define REP1 0x7E

#define ESC21 0x7D
#define ESC22 0x5D
#define REP2 0x7D

static bool n = false;

ssize_t read_supervision_message(int fd, unsigned char *address, unsigned char *control) {
    if (address == NULL || control == NULL) {
        return -1;
    }

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
    int i;
    for (i = 1; i <= MAX_ATTEMPTS; ++i) {
        send_supervision_message(fd, ADDRESS_EMITTER_RECEIVER, SET);
        printf("[connecting]: attempt: %d\n", i);
        alarm(TIMEOUT);
        unsigned char a, c;
        // COMBACK: The state machine does not validate the address and the control. Think about a better way.
        if (read_supervision_message(fd, &a, &c) >= 0 && a == ADDRESS_RECEIVER_EMITTER && c == UA) {
            return 0;
        }
    }
    return -1;
}

int connect_to_emitter(int fd) {
    unsigned char a, c;
    if (read_supervision_message(fd, &a, &c) < 0 || a != ADDRESS_EMITTER_RECEIVER || c != SET) {
        return -1;
    }

    if (send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, UA) < 0) {
        return -1;
    }

    return 0;
}

int disconnect_from_receiver(int fd) {
    if (send_supervision_message(fd, ADDRESS_EMITTER_RECEIVER, DISC) < 0) {
        return -1;
    }

    unsigned char a, c;
    if (read_supervision_message(fd, &a, &c) < 0 || a != ADDRESS_RECEIVER_EMITTER || c != DISC) {
        return -1;
    }

    if (send_supervision_message(fd, ADDRESS_EMITTER_RECEIVER, UA) < 0) {
        return -1;
    }

    return 0;
}

int disconnect_from_emitter(int fd) {
    if (send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, DISC) < 0) {
        return -1;
    }
    unsigned char a, c;
    if (read_supervision_message(fd, &a, &c) < 0 || a != ADDRESS_EMITTER_RECEIVER || c != UA) {
        return -1;
    }

    return 0;
}

ssize_t send_information(int fd, const unsigned char *data, size_t nb, bool no) {
    if (data == NULL) {
        return -1;
    }

    unsigned char header[] = {FLAG, ADDRESS_EMITTER_RECEIVER, CI(no),
                              (unsigned char) (ADDRESS_EMITTER_RECEIVER ^ CI(no))};

    if (write(fd, header, sizeof(header)) < 0) {
        return -1;
    }


    ssize_t res = 0, v;
    for (size_t i = 0; i < nb; ++i) {
        if (data[i] == REP1) {
            unsigned char m[] = {ESC11, ESC12};
            v = write(fd, m, 2);
            if (v < 0) {
                return -1;
            }
            res += v;
        } else if (data[i] == REP2) {
            char m[] = {ESC21, ESC22};
            v = write(fd, m, 2);
            if (v < 0) {
                return -1;
            }
            res += v;
        } else {
            v = write(fd, data + i, 1);
            if (v < 0) {
                return -1;
            }
            res += v;
        }
    }


    unsigned char bcc2;
    calculateBCC(data, &bcc2, nb);

    if (bcc2 == REP1) {
        unsigned char footer[] = {ESC11, ESC12, FLAG};
        if (write(fd, footer, sizeof(footer)) < 0) {
            return -1;
        }
    } else if (bcc2 == REP2) {
        unsigned char footer[] = {ESC21, ESC22, FLAG};
        if (write(fd, footer, sizeof(footer)) < 0) {
            return -1;
        }
    } else {
        unsigned char footer[] = {bcc2, FLAG};
        if (write(fd, footer, sizeof(footer)) < 0) {
            return -1;
        }
    }

    return res;
}

int calculateBCC(const unsigned char *data, unsigned char *bcc2, size_t size) {
    if (data == NULL || bcc2 == NULL) {
        return -1;
    }
    *bcc2 = 0;
    for (int j = 0; j < size - 2; ++j) *bcc2 = (unsigned char) (*bcc2 ^ data[j]);
    return 0;
}

ssize_t read_information(int fd, unsigned char *data, size_t size, bool no) {
    if (data == NULL) {
        return -1;
    }

    typedef enum {
        READ_FLAG_START, READ_ADDRESS, READ_CONTROL, READ_BCC1, READ_DATA, READ_BCC2
    } state_t;

    state_t s = READ_FLAG_START;

    unsigned char b, c, bcc2;
    unsigned char buf[256];
    unsigned int i = 0;
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
        } else if (s == READ_CONTROL && (b == CI(no) || b == DISC)) {
            c = b;
            s = READ_BCC1;
        } else if (s == READ_BCC1 && b == (unsigned char) (ADDRESS_EMITTER_RECEIVER ^ c)) {
            if (c == CI(no)) s = READ_DATA;
            else if (c == DISC) {
                if (read(fd, &b, 1) < 0) { return -1; }
                else { return -5; } 
            }
        } else if (s == READ_DATA) {
            if (i > size) return -2;
            if (b == ESC11) {
                char b2;
                if (read(fd, &b2, 1) < 0) {}
                if (b2 == ESC12) {
                    data[i] = REP1;
                    ++i;
                } else {
                    data[i] = b;
                    ++i;
                }
            } else if (b == ESC21) {
                char b2;
                if (read(fd, &b2, 1) < 0) {}
                if (b2 == ESC22) {
                    data[i] = REP2;
                    ++i;
                } else {
                    data[i] = b;
                    ++i;
                }
            } else {
                data[i] = b;
                ++i;
            }
            if (b == FLAG) {
                s = READ_BCC2;
            }
        } else if (s == READ_BCC2) {
            calculateBCC(data, &bcc2, i - 2);
            if (data[i - 2] == bcc2) {
                break;
            } else {
                return -3;
            }
        } else {
            s = READ_FLAG_START;
        }
    }
    size_t ds = (i - 2) <= size ? i - 2 : size;
    return (ssize_t) ds;
}

int ll_open(const char *path, bool is_emitter) {
    if (path == NULL) {
        return -1;
    }

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
        return -1;
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

    if (close_serial_port(fd) < 0) {
        fprintf(stderr, RED"[%s]: closing serial port: error: %s"RESET, source, strerror(errno));
        return -1;
    } else {
        printf("[%s]: closing serial port: success\n"RESET, source);
    }

    return 0;
}

ssize_t ll_read(int fd, void *data, size_t nb) {
    if (data == NULL) {
        return -1;
    }

    printf(YELLOW"[receiver]: reading message (R = %d)\n"RESET, n);

    int force_error = (rand() % 2 == 0);
    ssize_t r = read_information(fd, data, nb, n);
    if (r == -5) {
        return -2;
    } else if (r < 0 || force_error) {
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
    if (data == NULL) {
        return -1;
    }
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
    return -1;
}
