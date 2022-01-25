#ifndef FEUP_RC_TL2_PARSE_H
#define FEUP_RC_TL2_PARSE_H

struct parse_info {
    const char *scheme;
    const char *username;
    const char *password;
    const char *hostname;
    const char *url_path;
};

void printParseInfo(const struct parse_info *info);
int parseURL(char *url, struct parse_info *info);
int parseScheme(char *url, struct parse_info *info, char **rest);
int parseLogIn(char *url, struct parse_info *info, char **rest);
int parsePassword(char *url, struct parse_info *info, char **rest);
int parseHostname(char *url, struct parse_info *info, char **rest);
int parsePath(const char *url, struct parse_info *info);
int testParser();

#endif // FEUP_RC_TL2_PARSE_H
