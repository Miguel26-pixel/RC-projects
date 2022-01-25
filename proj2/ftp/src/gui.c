#include "../include/gui.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_CHARS_PERCENTAGE 27
#define MAX_CHARS_DESCRIPTION 35

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

void print_progress_bar(double p, const char *d, bool is_error, const int *code) {
    static size_t last_c = 0;
    const char *color = is_error ? RED : GREEN;
    size_t no_chars = p * MAX_CHARS_PERCENTAGE;
    if (p != 0 && p != 1 && !is_error && no_chars == last_c) {
        return;
    } else {
        last_c = no_chars;
    }

    printf("%s", CLEAR_SCREEN);
    printf("%s", HEADER1);

    size_t no_blank = MAX_CHARS_DESCRIPTION - strlen(d);
    if (code) no_blank -= 3;
    for (size_t i = 0; i < no_blank / 2; ++i) printf(" ");
    printf("%s", color);
    printf("%s", d);
    if (code) printf("%d", *code);
    printf("%s", RESET);
    for (size_t i = 0; i < no_blank - no_blank / 2; ++i) printf(" ");
    printf("%s", FOOTER1);
    printf("%s", color);
    for (size_t i = 0; i < no_chars; ++i) printf("%s", "█");
    printf("%s", RESET);
    for (size_t i = no_chars; i < MAX_CHARS_PERCENTAGE; ++i) printf(" ");
    printf("%s", FOOTER2);

    if(p == 0 || p == 1) usleep(1e5);
}