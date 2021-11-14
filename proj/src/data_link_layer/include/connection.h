#ifndef FEUP_RCOM_CONNECTION_H
#define FEUP_RCOM_CONNECTION_H

#include <sys/types.h>
#include <stdbool.h>

#define FLAG 0x7E
#define ADDRESS_EMITTER_RECEIVER 0x03
#define ADDRESS_RECEIVER_EMITTER 0x01
#define SET 0x03
#define UA 0x07

#define SET 0x03
#define UA  0x07
#define CI(n) (n << 6)
#define RR(n) (0x05 | (n << 7))
#define REJ(n) (0x01 | (n << 7))

#define RED "\x1b[1;31m"
#define YELLOW "\x1b[1;33m"
#define RESET "\x1b[1;0m"

int read_supervision_message(unsigned char address, unsigned char control);
ssize_t send_supervision_message(unsigned char address, unsigned char control);

int connect_to_receiver(void);
int connect_to_writer(void);

int send_i(const unsigned char *d, size_t nb, unsigned n);
int read_information(unsigned char *dest, size_t size, bool n);

#endif //FEUP_RCOM_CONNECTION_H
