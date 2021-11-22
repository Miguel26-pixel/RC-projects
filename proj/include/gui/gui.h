#ifndef FEUP_RCOM_GUI_H
#define FEUP_RCOM_GUI_H

#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

extern FILE *al_events;
extern FILE *al_errors;
extern FILE *ll_events;
extern FILE *ll_errors;
extern FILE *driver_events;
extern FILE *driver_errors;

#define RED "\x1b[1;31m"
#define YELLOW "\x1b[1;33m"
#define GREEN "\x1b[1;32m"
#define RESET "\x1b[1;0m"
#define CLEAR_SCREEN "\033c"

#define LOG_AL_EVENT(...) \
    if (al_events != NULL) { \
        fprintf(al_events, "%ld;", time(0)); fprintf(al_events, __VA_ARGS__); \
        }
#define LOG_AL_ERROR(...) \
    if (al_errors != NULL) { \
        fprintf(al_errors, "%ld;", time(0)); fprintf(al_errors, __VA_ARGS__); \
        }
#define LOG_LL_EVENT(...) \
    if (ll_events != NULL) { \
        fprintf(ll_events, "%ld;", time(0)); fprintf(ll_events, __VA_ARGS__); \
        }
#define LOG_LL_ERROR(...) \
    if (ll_errors != NULL) { \
        fprintf(ll_errors, "%ld;", time(0)); fprintf(ll_errors,__VA_ARGS__); \
        }
#define LOG_DRIVER_EVENT(...) \
    if (driver_events != NULL) { \
        fprintf(driver_events, "%ld;", time(0)); fprintf(driver_events,__VA_ARGS__); \
        }
#define LOG_DRIVER_ERROR(...) \
    if (driver_errors != NULL) { \
        fprintf(driver_errors, "%ld;", time(0)); fprintf(driver_errors,__VA_ARGS__); \
        }

void print_progress_bar(double p, const char *d, bool error);

int init_al_logs(void);

int init_ll_logs(void);

int init_driver_logs(void);

#endif //FEUP_RCOM_GUI_H
