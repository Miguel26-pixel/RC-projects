#ifndef  PROJ_SRC_DATA_LINK_LAYER_INCLUDE_CONNECTION_H_
#define  PROJ_SRC_DATA_LINK_LAYER_INCLUDE_CONNECTION_H_

#include <sys/types.h>
#include <stdbool.h>

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

#define BUF_SIZE 5192

size_t unstuff_bytes(const unsigned char *bytes, size_t nb, unsigned char *dest, size_t nbd);
size_t stuff_bytes(const unsigned char *bytes, size_t nb, unsigned char *dest, size_t nbd);

ssize_t read_frame(int fd, unsigned char *dest, size_t nbd);
ssize_t supervision_message(const unsigned char *m, unsigned char *a, unsigned char *c, size_t nb);
ssize_t information_message(const unsigned char *m, size_t nb, unsigned char *d, size_t nbd);

ssize_t send_supervision_message(int fd, unsigned char address, unsigned char control);

int connect_to_receiver(int fd);
int connect_to_emitter(int fd);

int disconnect_from_receiver(int fd);
int disconnect_from_emitter(int fd);

ssize_t send_information(int fd, const unsigned char *data, size_t nb, bool no);

int calculateBCC(const unsigned char *data, unsigned char *bcc2, size_t size);

#endif  // PROJ_SRC_DATA_LINK_LAYER_INCLUDE_CONNECTION_H_
