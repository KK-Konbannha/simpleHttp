#include "lib.h"
#include <stdio.h>

#ifndef HTTP_SESSION_H
#define HTTP_SESSION_H

int analyze_request(char *request_token, info_type *info,
                    return_info_t *return_info);
int http_session(int sock, info_type *info);

#endif
