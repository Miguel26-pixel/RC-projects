#ifndef FEUP_RCOM_GUI_H
#define FEUP_RCOM_GUI_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>


#define RED "\x1b[1;31m"
#define YELLOW "\x1b[1;33m"
#define GREEN "\x1b[1;32m"
#define RESET "\x1b[1;0m"
#define CLEAR_SCREEN "\033c"

void print_progress_bar(double p, const char *d, bool is_error, const int *code);

#endif