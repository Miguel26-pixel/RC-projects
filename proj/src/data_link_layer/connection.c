#include "../../include/data_link_layer/connection.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "../../include/errors/errnos.h"

bool n = false;

ssize_t read_supervision_message(int fd, unsigned char *address, unsigned char *control) {
    if (address == NULL || control == NULL) return NULL_POINTER_ERROR;

    typedef enum {
        READ_START_FLAG, READ_ADDRESS, READ_CONTROL, READ_BCC, READ_END_FLAG
    } state_t;

    state_t s = READ_START_FLAG;
    unsigned char b;
    while (true) {
        if (read(fd, &b, 1) < 0) {
            if (errno == EINTR) return TIMED_OUT;
            else continue;
        } else {
            alarm(0);
        }

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
            return SUCCESS;
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
        // printf("[connecting]: attempt: %d\n", i);
        alarm(TIMEOUT);
        unsigned char a, c;
        // COMBACK: The state machine does not validate the address and the control. Think about a better way.
        if (read_supervision_message(fd, &a, &c) >= 0 && a == ADDRESS_RECEIVER_EMITTER && c == UA) {
            return SUCCESS;
        }
    }
    return TOO_MANY_ATTEMPTS;
}

int connect_to_emitter(int fd) {
    unsigned char a, c;
    ssize_t r;
    r = read_supervision_message(fd, &a, &c);
    if (r < 0) return (int) r;
    else if (a != ADDRESS_EMITTER_RECEIVER || c != SET) return INVALID_RESPONSE;

    r = send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, UA);
    if (r < 0) return (int) r;

    return SUCCESS;
}

int disconnect_from_receiver(int fd) {
    ssize_t r;
    r = send_supervision_message(fd, ADDRESS_EMITTER_RECEIVER, DISC);
    if (r < 0) return (int) r;

    unsigned char a, c;
    r = read_supervision_message(fd, &a, &c);
    if (r < 0) return (int) r;
    else if (a != ADDRESS_RECEIVER_EMITTER || c != DISC) return INVALID_RESPONSE;

    r = send_supervision_message(fd, ADDRESS_EMITTER_RECEIVER, UA);
    if (r < 0) return (int) r;

    return SUCCESS;
}

int disconnect_from_emitter(int fd) {
    ssize_t r;
    r = send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, DISC);
    if (r < 0) return (int) r;
    unsigned char a, c;
    while (true) {
        r = read_supervision_message(fd, &a, &c);
        if (r < 0) return (int) r;
        else if (a == ADDRESS_EMITTER_RECEIVER && c == UA) break;
    }

    return SUCCESS;
}

ssize_t send_information(int fd, const unsigned char *data, size_t nb, bool no) {
    if (data == NULL) return NULL_POINTER_ERROR;

    unsigned char header[] = {FLAG, ADDRESS_EMITTER_RECEIVER, CI(no),
                              (unsigned char) (ADDRESS_EMITTER_RECEIVER ^ CI(no))};

    if (write(fd, header, sizeof(header)) < 0) return IO_ERROR;

    size_t k;
    unsigned char a[3];
    ssize_t res = 0, v;
    for (size_t i = 0; i < nb; ++i) {
        k = 0;
        if (data[i] == REP1) {
            a[k++] = ESC11;
            a[k++] = ESC12;
        } else if (data[i] == REP2) {
            a[k++] = ESC21;
            a[k++] = ESC22;
        } else {
            a[k++] = data[i];
        }
        v = write(fd, a, k);
        if (v < 0) {
            if (errno == EINTR) return TIMED_OUT;
            else return IO_ERROR;
        }
        res += v;
    }

    unsigned char bcc2;
    calculateBCC(data, &bcc2, nb);

    k = 0;
    if (bcc2 == REP1) {
        a[k++] = ESC11;
        a[k++] = ESC12;
    } else if (bcc2 == REP2) {
        a[k++] = ESC21;
        a[k++] = ESC22;
    } else {
        a[k++] = bcc2;
    }
    a[k++] = FLAG;
    if (write(fd, a, k) < 0) {
        if (errno == EINTR) return TIMED_OUT;
        else return IO_ERROR;
    }
    return res;
}

int calculateBCC(const unsigned char *data, unsigned char *bcc2, size_t size) {
    if (data == NULL || bcc2 == NULL) return NULL_POINTER_ERROR;

    *bcc2 = 0;
    for (int j = 0; j < size; ++j) *bcc2 = (unsigned char) (*bcc2 ^ data[j]);
    return SUCCESS;
}

ssize_t read_information(int fd, unsigned char *data, size_t size, bool no) {
    if (data == NULL) return NULL_POINTER_ERROR;

    typedef enum {
        READ_FLAG_START, READ_ADDRESS, READ_CONTROL, READ_BCC1, READ_DATA, READ_BCC2
    } state_t;

    state_t s = READ_FLAG_START;
    unsigned char b, c, bcc2;
    unsigned int i = 0;
    while (true) {
        // COMBACK: Better to reorganize the loop to avoid reading sometimes.
        if (s < READ_BCC2) {
            if (read(fd, &b, 1) < 0) {
                if (errno == EINTR) return TIMED_OUT;
                else continue;
            } else {
                alarm(0);
            }
        }

        if (s == READ_FLAG_START && b == FLAG) {
            s = READ_ADDRESS;
        } else if (s == READ_ADDRESS) {
            s = READ_CONTROL;
        } else if (s == READ_CONTROL) {
            c = b;
            s = READ_BCC1;
        } else if (s == READ_BCC1 && b == (unsigned char) (ADDRESS_EMITTER_RECEIVER ^ c)) {
            if (c == CI(no) || c == CI(!no)) {
                s = READ_DATA;
            } else {
                if (read(fd, &b, 1) < 0) {
                    if (errno == EINTR) return TIMED_OUT;
                    else return IO_ERROR;
                } else {
                    if (c == DISC) return EOF_DISCONNECT;
                    else s = READ_FLAG_START;
                }
            }
        } else if (s == READ_DATA) {
            if (i > size) return BUFFER_OVERFLOW;

            if (b == ESC11 || b == ESC21) {
                char b2;
                if (read(fd, &b2, 1) < 0) {
                    if (errno == EINTR) return TIMED_OUT;
                    else return IO_ERROR;
                }
                if (b == ESC11 && b2 == ESC12) {
                    data[i++] = REP1;
                } else if (b == ESC21 && b2 == ESC22) {
                    data[i++] = REP2;
                } else {
                    data[i++] = b;
                    data[i++] = b2;
                }
            } else {
                data[i++] = b;
            }

            if (b == FLAG) s = READ_BCC2;
        } else if (s == READ_BCC2) {
            if (c == CI(!n)) return OUT_OF_ORDER;
            calculateBCC(data, &bcc2, i - 2);
            if (data[i - 2] == bcc2) break;
            else return PARITY_ERROR;
        } else {
            s = READ_FLAG_START;
        }
    }

    size_t ds = (i - 3) <= size ? i - 3 : size;
    return (ssize_t) ds;
}
