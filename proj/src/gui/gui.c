#include "../../include/gui/gui.h"
#include <stdio.h>
#include <string.h>
#include "../../include/errors/errnos.h"

#define MAX_CHARS_PERCENTAGE 27
#define MAX_CHARS_DESCRIPTION 35
#define GREEN "\x1b[1;32m"
#define RED "\x1b[1;31m"
#define RESET "\x1b[1;0m"
#define CLEAR_SCREEN "\033c"

const char *HEADER1 = "┌─────────────────────────────────────────┐\n"
                      "│█████████████████████████████████████████│\n"
                      "│██┌───────────────────────────────────┐██│\n"
                      "│██│ ┌┐   ┌───┐┌──┐┌───┐    ┌───┐┌───┐ │██│\n"
                      "│██│ ││   │┌──┘└┤├┘│┌─┐│    │┌─┐││┌─┐│ │██│\n"
                      "│██│ ││   │└──┐ ││ ││ └┘    │└─┘│││ └┘ │██│\n"
                      "│██│ ││ ┌┐│┌──┘ ││ ││ ┌┐┌──┐│┌┐┌┘││ ┌┐ │██│\n"
                      "│██│ │└─┘││└──┐┌┤├┐│└─┘│└──┘│││└┐│└─┘│ │██│\n"
                      "│██│ └───┘└───┘└──┘└───┘    └┘└─┘└───┘ │██│\n"
                      "│██│";
const char *FOOTER1 = "│██│\n"
                      "│██│  ┌─────────────────────────────┐  │██│\n"
                      "│██│  │ ";
const char *FOOTER2 = " │  │██│\n"
                      "│██│  └─────────────────────────────┘  │██│\n"
                      "│██└───────────────────────────────────┘██│\n"
                      "│█████████████████████████████████████████│\n"
                      "└─────────────────────────────────────────┘\n";


void print_progress_bar(double p, const char *d, bool is_error) {
    const char *color = is_error ? RED : GREEN;
    size_t no_chars = p * MAX_CHARS_PERCENTAGE;

    printf("%s", CLEAR_SCREEN);
    printf("%s", HEADER1);

    size_t no_blank = MAX_CHARS_DESCRIPTION - strlen(d);
    for (size_t i = 0; i < no_blank / 2; ++i) printf(" ");
    printf("%s", color);
    printf("%s", d);
    printf("%s", RESET);
    for (size_t i = 0; i < no_blank - no_blank / 2; ++i) printf(" ");
    printf("%s", FOOTER1);
    printf("%s", color);
    for (size_t i = 0; i < no_chars; ++i) printf("%s", "█");
    printf("%s", RESET);
    for (size_t i = no_chars; i < MAX_CHARS_PERCENTAGE; ++i) printf(" ");
    printf("%s", FOOTER2);
}

int init_al_logs(void) {
    al_events = fopen("al_events.log", "a");
    al_errors = fopen("al_errors.log", "a");

    if (al_errors == NULL || al_events == NULL) return IO_ERROR;

    return 0;
}

int init_ll_logs(void) {
    ll_events = fopen("ll_events.log", "a");
    ll_errors = fopen("ll_errors.log", "a");

    if (ll_events == NULL || ll_errors == NULL) return IO_ERROR;

    return 0;
}

int init_driver_logs(void) {
    driver_events = fopen("driver_events.log", "a");
    driver_errors = fopen("driver_errors.log", "a");

    if (driver_events == NULL || driver_errors == NULL) return IO_ERROR;

    return 0;
}

