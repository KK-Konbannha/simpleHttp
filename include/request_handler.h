#include "lib.h"

#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

int accept_get(char *buf, int remaining_size, info_type *info,
               return_info_t *return_info, int auth);
int accept_post(char *buf, int remaining_size, info_type *info,
                return_info_t *return_info, int auth);
int parse_header(char *field, info_type *info, int *is_find_keep_alive);
int parse_request(char *status, info_type *info, return_info_t *return_info);

#endif
