#include "include/alarm.h"
#define _GNU_SOURCE

#include <string.h>
#include <signal.h>


extern int interrupt;

int setup_alarm(void) {
    struct sigaction action;
    action.sa_handler = sigalrm_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGALRM, &action, NULL);
}

void sigalrm_handler(int _) {
    interrupt = 1;
}