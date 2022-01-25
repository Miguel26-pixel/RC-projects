#include "../include/authentication.h"
#include "../include/gui.h"
#include "../include/read.h"
#include "../include/write.h"
#include <stdio.h>
int logIn(struct parse_info *info, int fd) {
    int r;
    if (info->username) {
        r = sendCommand(fd, "user", info->username);
        if (r < 0) {
            return -1;
        } else {
            print_progress_bar(0, "Sent username", false, NULL);
            int code;
            if (getResponseCode(fd, &code) || code != 331) {
                print_progress_bar(0, "Failed with code: ", true, &code);
                return -1;
            }
        }
    }

    if (info->password) {
        r = sendCommand(fd, "pass", info->password);
        if (r < 0) {
            return -1;
        } else {
            int code;
            print_progress_bar(0, "Sent password", false, NULL);
            if (getResponseCode(fd, &code) || code != 230) {
                print_progress_bar(0, "Failed with code: ", true, &code);
                return -1;
            }
        }
    }
    return 0;
}