#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define F 0x7E
#define AER 0x03
#define ARE 0x01
#define SET 0x03
#define UA 0x07

#define C0 0x00
#define C1 0x40

#define RR0 0x05
#define RR1 0x85

#define REJ0 0x01
#define REJ1 0x81


volatile int STOP=FALSE;

int readSET(int fd) {
    int res;
    unsigned char m,a, c;
    res = read(fd, &m, 1);

    if (m != F) puts("ERROR FLAG"); 
    res = read(fd, &a, 1); 

    if (a != AER) puts("ERROR A");  
    res = read(fd, &c, 1); 

    if (c != SET) puts("ERROR C");
    res = read(fd, &m, 1);
 
    if (m != (char)(a ^ c)) puts("ERROR BCC");   
    res = read(fd, &m, 1);

    if (m != F) puts("ERROR FLAG");
    
    printf("everything okeie\n");
}

int readI(int fd) {
    int res;
    char m, a, c;
    unsigned char buf[255];
    res = read(fd, &m, 1);

    if (m != F) puts("ERROR FLAG"); 
    res = read(fd, &a, 1); 

    if (a != AER) puts("ERROR A");  
    res = read(fd, &c, 1); 

    if (c != C0 ){ if (c != C1) puts("ERROR Casd"); }
    res = read(fd, &m, 1);
 
    if (m != (char)(a ^ c)) puts("ERROR BCC");     

    int count = 0;
    unsigned char bcc2 = 0;
    do { 
       res = read(fd, &m, 1);
       buf[count] = m;
       count++;
    } while (m != F);

    for (int i= 0; i < count - 2 ; i++) {
        bcc2 = bcc2 ^ buf[i];
    } 

    if (bcc2 != buf[count - 2]) puts("ERROR BCC2");

    if (buf[count - 1] != F) puts("ERROR F2");
    
    printf("READ I DONE\n");

    if (c == C0) { return 0; } else { return 1; }
}

int writeSU(int fd, char c) {
    int res;
    unsigned char m; 
    m = F;
    res = write(fd, &m, 1);   
    m = ARE;
    res = write(fd, &m, 1); 
    m = c;
    res = write(fd, &m, 1); 
    m = ARE ^ c;
    res = write(fd, &m, 1); 
    m = F;
    res = write(fd, &m, 1);
    
    printf("SEND ANSWER DONE\n");
}

int main(int argc, char** argv)
{
    int fd, res;
    unsigned char m;
    struct termios oldtio,newtio;
    unsigned char buf[255];

    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS4", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS10", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS11", argv[1])!=0))) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
  
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {perror(argv[1]); exit(-1); }

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

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */



  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) próximo(s) caracter(es)
  */



    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

	  readSET(fd);
    
    writeSU(fd,UA);

    int r = readI(fd);

    if (r == 0) {
        printf("ANSWER = RR1\n");
        writeSU(fd,RR1);
    } else {
        printf("ANSWER = RR0\n");
        writeSU(fd,RR0);
    }

    
    
    
    //res = write(fd,buff,255);

  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */

	sleep(1);

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
