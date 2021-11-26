#include "../../include/data_link_layer/connection.h"

#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "../../include/errors/error_nos.h"
#include "../../include/gui/gui.h"

bool n = false;

size_t unstuff_bytes(const unsigned char *bytes, size_t nb, unsigned char *dest, size_t nbd) {
    size_t j = 0;
    for (int i = 0; i < nb; ++i) {
        if (j == nbd) return BUFFER_OVERFLOW;
        if (bytes[i] == ESC11 && bytes[i + 1] == ESC12) {
            dest[j] = REP1;
            ++i;
        } else if (bytes[i] == ESC21 && bytes[i + 1] == ESC22) {
            dest[j] = REP2;
            ++i;
        } else {
            dest[j] = bytes[i];
        }
        ++j;
    }
    return j;
}

size_t stuff_bytes(const unsigned char *bytes, size_t nb, unsigned char *dest, size_t nbd) {
    size_t j = 0;
    for (int i = 0; i < nb; ++i) {
        if (j == nbd) return BUFFER_OVERFLOW;
        if (bytes[i] == REP1) {
            dest[j++] = ESC11;
            dest[j++] = ESC12;
        } else if (bytes[i] == REP2) {
            dest[j++] = ESC21;
            dest[j++] = ESC22;
        } else {
            dest[j++] = bytes[i];
        }
    }
    return j;
}

ssize_t read_frame(int fd, unsigned char *dest, size_t nbd) {
    bool stop = false;
    unsigned char bytes[BUF_SIZE];
    size_t i = 0;
    while (!stop) {
        if (read(fd, bytes + i, 1) < 0) {
            if (errno == EINTR) {
                return TIMED_OUT;
            } else {
                continue;
            }
        } else {
            alarm(0);
        }

        if (bytes[i] == FLAG) {
            if (bytes[i - 1] == FLAG) continue;
            else if (i != 0) stop = true;
        }

        ++i;
    }
    i = unstuff_bytes(bytes, i, dest, nbd);
    return (ssize_t) i;
}

ssize_t supervision_message(const unsigned char *m, unsigned char *a, unsigned char *c, size_t nb) {
    // COMBACK: ptr
    typedef enum {
        READ_START_FLAG, READ_ADDRESS, READ_CONTROL, READ_BCC, READ_END_FLAG
    } state_t;

    state_t s = READ_START_FLAG;

    size_t i = 0;
    while (true) {
        if (i == nb) return BUFFER_OVERFLOW;
        if (s == READ_START_FLAG && m[i] == FLAG) {
            s = READ_ADDRESS;
            // COMBACK
        } else if (s == READ_ADDRESS && true) {
            *a = m[i];
            s = READ_CONTROL;
            // COMBACK
        } else if (s == READ_CONTROL && true) {
            *c = m[i];
            s = READ_BCC;
        } else if (s == READ_BCC) {
            if (m[i] == (unsigned char) (*a ^ *c)) {
                s = READ_END_FLAG;
            } else {
                return PARITY_ERROR;
            }
        } else if (s == READ_END_FLAG && m[i] == FLAG) {
            return SUCCESS;
        } else {
            return -1;
        }
        ++i;
    }
}

ssize_t information_message(const unsigned char *m, size_t nb, unsigned char *d, size_t nbd) {
    typedef enum {
        READ_START_FLAG, READ_ADDRESS, READ_CONTROL, READ_BCC1, READ_DATA, CHECK_CONTROL, READ_BCC2, READ_END_FLAG
    } state_t;

    state_t s = READ_START_FLAG;
    size_t i = 0, j = 0;
    unsigned char a, c;
    while (true) {
        // if (i == nb) pause();// return BUFFER_OVERFLOW;
        if (s == READ_START_FLAG) {
            if (m[i] == FLAG) s = READ_ADDRESS;
            //COMBACK
        } else if (s == READ_ADDRESS && true) {
            a = m[i];
            s = READ_CONTROL;
        } else if (s == READ_CONTROL && true) {
            c = m[i];
            if (c == DISC) return EOF_DISCONNECT;
            s = READ_BCC1;
        } else if (s == READ_BCC1) {
            if (m[i] == (unsigned char) (a ^ c)) s = CHECK_CONTROL;
            else return PARITY_ERROR;
        } else if (s == CHECK_CONTROL) {
            if (c == DISC) return EOF_DISCONNECT;
            else if (c == CI(!n)) return OUT_OF_ORDER;
            else if (c == CI(n)) s = READ_DATA;
            else return WRONG_HEADER;
            --i;
        } else if (s == READ_DATA) {
            if (i == nb - 2) {
                s = READ_BCC2;
                --i;
            } else {
                if (j == nbd) return BUFFER_OVERFLOW;
                else d[j++] = m[i];
            }
        } else if (s == READ_BCC2) {
            unsigned char bcc2;
            calculateBCC(d, &bcc2, j);
            if (m[i] == bcc2) s = READ_END_FLAG;
            else return PARITY_ERROR;
        } else if (s == READ_END_FLAG && m[i] == FLAG) {
            return j;
        } else {
            return -1;
        }
        ++i;
    }
}

ssize_t send_supervision_message(int fd, unsigned char address, unsigned char control) {
    unsigned char message[] = {FLAG, address, control, address ^ control, FLAG};
    return write(fd, message, sizeof(message));
}

int connect_to_receiver(int fd) {
    int i;
    unsigned char bytes[BUF_SIZE];
    for (i = 1; i <= MAX_ATTEMPTS; ++i) {
        send_supervision_message(fd, ADDRESS_EMITTER_RECEIVER, SET);
        LOG_LL_EVENT("[connecting]: attempt: %d\n", i)
        alarm(TIMEOUT);
        unsigned char a, c;
        if (read_frame(fd, bytes, sizeof(bytes)) < 0) {}
        else if (supervision_message(bytes, &a, &c, sizeof(bytes)) < 0) {}
        else if (a != ADDRESS_RECEIVER_EMITTER || c != UA) {}
        else { return SUCCESS; }
    }
    return TOO_MANY_ATTEMPTS;
}

int connect_to_emitter(int fd) {
    unsigned char a, c;
    ssize_t r;
    unsigned char bytes[BUF_SIZE];

    if ((r = read_frame(fd, bytes, sizeof(bytes))) < 0) { return r; }
    else if ((r = supervision_message(bytes, &a, &c, sizeof(bytes))) < 0) { return r; }
    else if (a != ADDRESS_EMITTER_RECEIVER || c != SET) { return INVALID_RESPONSE; }
    else if ((r = send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, UA)) < 0) { return r; }
    else { return SUCCESS; }
}

int disconnect_from_receiver(int fd) {
    ssize_t r;
    unsigned char a, c;
    unsigned char bytes[BUF_SIZE];

    if ((r = send_supervision_message(fd, ADDRESS_EMITTER_RECEIVER, DISC)) < 0) { return r; }
    else if ((r = read_frame(fd, bytes, sizeof(bytes))) < 0) { return r; }
    else if ((r = supervision_message(bytes, &a, &c, sizeof(bytes))) < 0) { return r; }
    else if (a != ADDRESS_RECEIVER_EMITTER || c != DISC) { return INVALID_RESPONSE; }
    else if ((r = send_supervision_message(fd, ADDRESS_EMITTER_RECEIVER, UA)) < 0) { return r; }
    else { return SUCCESS; }
}

int disconnect_from_emitter(int fd) {
    ssize_t r;
    unsigned char a, c;
    unsigned char bytes[BUF_SIZE];
    if ((r = send_supervision_message(fd, ADDRESS_RECEIVER_EMITTER, DISC)) < 0) { return r; }
    while (true) {
        if ((r = read_frame(fd, bytes, sizeof(bytes))) < 0) { return r; }
        else if ((r = supervision_message(bytes, &a, &c, 0)) < 0) { return r; }
        else if (a == ADDRESS_EMITTER_RECEIVER && c == UA) { break; }
    }

    return SUCCESS;
}

ssize_t send_information(int fd, const unsigned char *data, size_t nb, bool no) {
    if (data == NULL) return NULL_POINTER_ERROR;

    unsigned char header[] = {FLAG, ADDRESS_EMITTER_RECEIVER, CI(no),
                              (unsigned char) (ADDRESS_EMITTER_RECEIVER ^ CI(no))};

    if (write(fd, header, sizeof(header)) < 0) return IO_ERROR;

    ssize_t res;
    size_t nbd;
    unsigned char bytes[BUF_SIZE];

    nbd = stuff_bytes(data, nb, bytes, sizeof(bytes));
    if ((res = write(fd, bytes, nbd)) < 0) return IO_ERROR;

    unsigned char a[3];
    unsigned char bcc2;
    calculateBCC(data, &bcc2, nb);
    nbd = stuff_bytes(&bcc2, 1, a, sizeof(a));
    a[nbd++] = FLAG;

    if (write(fd, a, nbd) < 0) return IO_ERROR;

    return res;
}

int calculateBCC(const unsigned char *data, unsigned char *bcc2, size_t size) {
    if (data == NULL || bcc2 == NULL) return NULL_POINTER_ERROR;

    *bcc2 = 0;
    for (int j = 0; j < size; ++j) *bcc2 = (unsigned char) (*bcc2 ^ data[j]);
    return SUCCESS;
}
