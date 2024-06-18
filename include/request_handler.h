#include "lib.h"

#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

int accept_get(char *buf, int remaining_size, info_type *info,
               return_info_t *return_info, int is_head);
int accept_post(char *buf, int remaining_size, info_type *info,
                return_info_t *return_info);
int parse_header(char *field, info_type *info);
int parse_request(char *status, info_type *pinfo);
void http_reply(int sock, info_type *info, int is_head);

#endif
