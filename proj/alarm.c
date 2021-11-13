#include "alarm.h"

extern int interrupt;

int setupAlarm() {
    struct sigaction action;
	action.sa_handler = sigalrm_hadler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGALRM, &action, NULL);
}

void sigalrm_hadler(int _) {
    interrupt = 1;
}