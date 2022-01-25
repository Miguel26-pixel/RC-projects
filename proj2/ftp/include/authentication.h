#ifndef FEUP_RC_TL2_AUTHENTICATION_H
#define FEUP_RC_TL2_AUTHENTICATION_H

#include "parse.h"

int logIn(struct parse_info *info, int fd);
int connectToSocket(const char *ip, int port);

#endif //FEUP_RC_TL2_AUTHENTICATION_H
