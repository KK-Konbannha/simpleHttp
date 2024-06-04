#include "exp1lib.h"

void accept_get(int sock, char *buf, int remaining_size, info_type *info,
                int is_head);
int parse_header(char *field, info_type *info);
int parse_request(char *status, info_type *pinfo);
void http_reply(int sock, info_type *info, int is_head);
