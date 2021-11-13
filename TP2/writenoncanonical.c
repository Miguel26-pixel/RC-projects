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

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define MAX_ATTEMPTS 3

#define F   0x7E
#define AER 0x03
#define ARE 0x01
#define SET 0x03
#define UA  0x07

volatile int STOP=FALSE;
int fd;
int interrupt;

int send_set() {
    int res;
    char m[] = {F, AER, SET, AER ^ SET, F};

    res = write(fd, m, 5);  

    printf("%d bytes written\n", res);
}

int read_ua() {

    int res;
    char a, c, m;
    while (!((res = read(fd, &m, 1)) > 0 || interrupt == 1)) { }
    if (res <= 0) return 1;
    if (m != F) puts("ERROR FLAG");

    printf("res - %d m - %c\n",res,m);
    
    alarm(0);

    res = read(fd, &a, 1); 
    printf("res - %d m - %c\n",res,a);
    if (a != ARE) puts("ERROR A"); 
      
    res = read(fd, &c, 1);
    printf("res - %d m - %c\n",res,c);
    if (c != UA) puts("ERROR C"); 
      
    res = read(fd, &m, 1); 
    printf("res - %d m - %c\n",res,m);
    if (m != (char) (a ^ c)) puts("ERROR BCC");
    
    res = read(fd, &m, 1);
    printf("res - %d m - %c\n",res,m);
    if (m != F) puts("ERROR FLAG"); 
    
    return 0;
}

void sigalrm_hadler(int _) {
    printf("handler reached\n");
    interrupt = 1;
}


int main(int argc, char** argv)
{
    int res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0, interrupt_count = 0;
    
    if ( (argc < 2) ) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) {perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure settt\n");


    int done = 1;
    while(interrupt_count < MAX_ATTEMPTS && done != 0) {
        send_set();
        interrupt=0;
        printf("HERE1\n");
        signal(SIGALRM, sigalrm_hadler);
        alarm(3);
        if((done = read_ua()) != 0) interrupt_count++; else break;
    }
    
    if (interrupt_count == MAX_ATTEMPTS) puts("INTERRUPTED - REACHED MAX TRIES");
    
    sleep(1);
    
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
    return 0;
}
