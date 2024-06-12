#include "exp1lib.h"

#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

void accept_get(int sock, char *buf, int remaining_size, info_type *info,
                int is_head);
void accept_post(int sock, char *buf, int remaining_size, info_type *info);
int parse_header(char *field, info_type *info);
int parse_request(char *status, info_type *pinfo);
void http_reply(int sock, info_type *info, int is_head);

#endif
