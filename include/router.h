#include "lib.h"

#ifndef ROUTER_H
#define ROUTER_H

void route_request(int sock, info_type *info, return_info_t *return_info);
void route_get_request(int sock, info_type *info, return_info_t *return_info);
void route_post_request(int sock, info_type *info, return_info_t *return_info);

#endif
