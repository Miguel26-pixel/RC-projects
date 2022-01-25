#include "../include/parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printParseInfo(const struct parse_info *info) {
    printf("scheme: \t%s\n", info->scheme);
    printf("username: \t%s\n", info->username);
    printf("password: \t%s\n", info->password);
    printf("hostname: \t%s\n", info->hostname);
    printf("url_path: \t%s\n", info->url_path);
}

int parseURL(char *url, struct parse_info *info) {
    memset(info, 0, sizeof(*info));
    parseScheme(url, info, &url);
    parseLogIn(url, info, &url);
    parseHostname(url, info, &url);
    parsePath(url, info);
    return 0;
}

int parseScheme(char *url, struct parse_info *info, char **rest) {
    // Look for string of the form <scheme>://
    char *s = strsep(&url, ":");
    // If the prefix is not found, or we are missing "//", we can't continue.
    if (!url || !strlen(s) || strncmp("//", url, 2) != 0) {
        return -1;
    }
    // Everything good so far
    info->scheme = s;

    // Skip the possible repeated "/" characters
    while (*url == '/') ++url;
    *rest = url;
    return 0;
}

int parseLogIn(char *url, struct parse_info *info, char **rest) {
    // Look for a string of the form "username:"
    char *s = strsep(&url, ":");

    // If the string is not found, this field is ignored.
    // However, no password can exist either.
    if (!url || !strlen(s)) {
        url = s;
        if (strchr(url, '@') != NULL) {
            fprintf(stdout, "A password was specified without a username.\n");
            return -1;
        }
    } else {
        info->username = s;
        parsePassword(url, info, &url);
    }
    *rest = url;
    return 0;
}

int parsePassword(char *url, struct parse_info *info, char **rest) {
    // If we have a username, we need a password.
    // Look for a string of the form "password@"
    char *s = strsep(&url, "@");

    // If the string is not found, we can't continue.
    if (!url || !strlen(s)) {
        fprintf(stdout, "A username was specified without a password.\n");
        exit(-1);
    }
    info->password = s;
    *rest = url;
    return 0;
}

int parseHostname(char *url, struct parse_info *info, char **rest) {
    // Look for a string of the form "hostname/"
    char *s = strsep(&url, "/");

    // If the string is not found, we can't continue.
    if (!url || !strlen(s)) {
        fprintf(stdout, "A hostname has not been specified.\n");
        return -1;
    } else {
        info->hostname = s;
    }
    *rest = url;
    return 0;
}

int parsePath(const char *url, struct parse_info *info) {
    // The remainder of the string is the path.
    if (strlen(url)) {
        info->url_path = url;
    } else {
        fprintf(stdout, "A path has not been specified.\n");
        return -1;
    }
    return 0;
}