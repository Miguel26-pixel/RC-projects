#include "gui.h"
#include <stdio.h>
#include <string.h>

#define MAX_CHARS_PERCENTAGE 27
#define MAX_CHARS_DESCRIPTION 35
#define GREEN "\x1b[1;32m"
#define RED "\x1b[1;31m"
#define RESET "\x1b[1;0m"

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
    printf("\033c%s", HEADER1);
    size_t no_blank = MAX_CHARS_DESCRIPTION - strlen(d);
    for (size_t i = 0; i < no_blank / 2; ++i) printf(" ");
    printf("%s", d);
    for (size_t i = 0; i < no_blank - no_blank / 2; ++i) printf(" ");
    printf("%s", FOOTER1);
    printf("%s", color);
    for (size_t i = 0; i < no_chars; ++i) printf("%s", "█");
    printf("%s", RESET);
    for (size_t i = no_chars; i < MAX_CHARS_PERCENTAGE; ++i) printf(" ");
    printf("%s", FOOTER2);
}
