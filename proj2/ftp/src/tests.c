#include "../include/tests.h"
#include "../include/parse.h"
#include <stdio.h>

int testParser() {
    struct parse_info pi;
    char url1[] = "ftp://////////user:password@ftp.up.pt////path/////anotherpath///";
    char url2[] = "ftp://////////ftp.up.pt////path/////anotherpath///";
    char url3[] = "ftp://////////user:@ftp.up.pt////path/////anotherpath///";
    char url4[] = "ftp://////////user@ftp.up.pt////path/////anotherpath///";
    char url5[] = "ftp://////////password@ftp.up.pt////path/////anotherpath///";
    char url6[] = "ftp://////////user:password@////path/////anotherpath///";
    char url7[] = "ftp://////////////path/////anotherpath///";

    char *urls[] = {url1, url2, url3, url4, url5, url6, url7};

    for (int i = 0; i < sizeof(urls) / sizeof(urls[0]); ++i) {
        if (!parseURL(urls[i], &pi)) printParseInfo(&pi);
        else
            puts("Failed.");
        printf("\n");
    }
    return 0;
}