#define F   0x7E
#define AER 0x03
#define ARE 0x01
#define SET 0x03
#define UA  0x07

#define CI0 0x00
#define CI1 0x40

#define RR0 0x05
#define RR1 0x85

#define REJ0 0x01
#define REJ1 0x81

extern int fd, interrupt;

int send_set(void);

int send_supervision_message(unsigned char c);