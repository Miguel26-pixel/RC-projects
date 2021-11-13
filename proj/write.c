#include "write.h"

int send_set() {
    interrupt = 0;
    int res;
    unsigned char m;

    m = F;
    res = write(fd, &m, 1);   

    m = AER;
    res = write(fd, &m, 1);
     
    m = SET;
    res = write(fd, &m, 1); 

    m = AER ^ SET;
    res = write(fd, &m, 1); 

    m = F;
    res = write(fd, &m, 1);

    printf("%d bytes written\n", res);
}

int send_SU(unsigned char c) {
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
