/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "include/alarm.h"
#include "include/setup.h"

#define _GNU_SOURCE

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

volatile int STOP=FALSE;
int fd;
int interrupt;


int send_i(const unsigned char *d, size_t nb, unsigned n) {
    interrupt = 0;
    int res;
    unsigned char m;

    m = F;
    res = write(fd, &m, 1);   

    m = AER;
    res = write(fd, &m, 1);
     
    m = n == 0 ? CI0 : CI1;
    res = write(fd, &m, 1); 

    m = (unsigned char) (AER ^ (n == 0 ? CI0 : CI1));
    res = write(fd, &m, 1); 

    m = 0;
    for (int i = 0; i < nb; ++i) {
        res = write(fd, d + i, 1);
        m ^= d[i];
    }

    res = write(fd, &m, 1);

    m = F;
    res = write(fd, &m, 1);

    printf("%d bytes written\n", res);
    return 0;
}


int read_rr(int n) {
    int res;
    unsigned char a, c, m;
    res = read(fd, &m, 1);
    if (res <= 0) { interrupt = 1; return 1; }
    if (m != F) puts("ERROR FLAG");
    
    alarm(0);

    res = read(fd, &a, 1); 
    if (a != ARE) puts("ERROR A"); 
      
    res = read(fd, &c, 1); ;
    if (c != (n == 0 ? RR0 : RR1)) puts("ERROR C"); 
      
    res = read(fd, &m, 1);
    
    if (m != (a ^ c)) printf("ERROR BCC: a - %x, c - %x, xor - %x, m - %x, bool - %d\n",a,c,(char) a^c,m, (char) (a^c) == (char) m);
    
    res = read(fd, &m, 1);
    if (m != F) puts("ERROR FLAG"); 
    
    return 0;
}



int main(int argc, char** argv)
{
    int res;
    struct termios oldtio;
    unsigned char buf[255];
    int i, sum = 0, speed = 0, interrupt_count = 0;
    
    openSerialPort(argc,argv, &oldtio);

    int done = 1;

	setupAlarm();

	if (connectToReader() != 0) {
		closeSerialPort(&oldtio);
		return 1;
	}
    
    puts("SET DONE");
    
    unsigned char s[] = "HelloWorld";
    send_i(s, sizeof(s), 0);
    puts("I DONE");
    read_rr(1);
    puts("RR1 DONE");
    
	closeSerialPort(&oldtio);
    
	return 0;
}
