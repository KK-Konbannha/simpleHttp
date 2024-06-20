#include "lib.h"
#include <errno.h>
#include <stdio.h>
#include <sys/epoll.h>

#ifndef HTTP_SESSION_H
#define HTTP_SESSION_H

int analyze_request(char *request_token, info_type *info,
                    return_info_t *return_info, int auth);
int http_session(int sock, info_type *info, return_info_t *return_info,
                 int auth, int is_non_blocking);

#endif
