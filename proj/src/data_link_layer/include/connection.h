#ifndef FEUP_RCOM_CONNECTION_H
#define FEUP_RCOM_CONNECTION_H

#include <sys/types.h>
#include <stdbool.h>

int read_supervision_message(unsigned char address, unsigned char control);
ssize_t send_supervision_message(unsigned char address, unsigned char control);
int connect_to_receiver(void);
int connect_to_writer(void);
int send_i(const unsigned char *d, size_t nb, unsigned n);
int read_information(unsigned char *dest, size_t size, bool n);

#endif //FEUP_RCOM_CONNECTION_H
