#define _GNU_SOURCE

#include "../../include/data_link_layer/alarm.h"

#include <string.h>
#include <signal.h>

int setup_alarm(void) {
    struct sigaction action;
    action.sa_handler = sigalrm_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    return sigaction(SIGALRM, &action, NULL);
}

void sigalrm_handler(__attribute__((unused)) int _) {}
