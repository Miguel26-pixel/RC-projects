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


volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd, res;
    char m;
    struct termios oldtio,newtio;
    char buf[255];

    if ( (argc < 2) ) {
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

	char a, c;
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
    
    m = F;
    res = write(fd, &m, 1);   
    m = ARE;
    res = write(fd, &m, 1); 
    m = UA;
    res = write(fd, &m, 1); 
    m = ARE ^ UA;
    res = write(fd, &m, 1); 
    m = F;
    res = write(fd, &m, 1);
    
    printf("everything okeie\n");
    
    
    
    //res = write(fd,buff,255);

  /* 
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião 
  */

	sleep(1);

    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
