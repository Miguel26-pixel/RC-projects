#ifndef FEUP_RCOM_CONNECTION_H
#define FEUP_RCOM_CONNECTION_H

#include <sys/types.h>

int read_supervision_message(unsigned char address, unsigned char control);
int send_supervision_message(unsigned char address, unsigned char control);
int connect_to_reader(void);
int connect_to_writer(void);
int send_i(const unsigned char *d, size_t nb, unsigned n);
int read_rr(int n);
int readI();

#endif //FEUP_RCOM_CONNECTION_H
