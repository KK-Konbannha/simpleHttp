#include "lib.h"
#include <time.h>

#ifndef HANDLERS_H
#define HANDLERS_H

void handle_index(int sock, info_type *info, return_info_t *return_info);
void handle_test(int sock, info_type *info, return_info_t *return_info);
void handle_test_cgi(int sock, info_type *info, return_info_t *return_info);
void handle_new(int sock, info_type *info, return_info_t *return_info);
void handle_done(int sock, info_type *info, return_info_t *return_info);
void handle_api(int sock, info_type *info, return_info_t *return_info);
void handle_static(int sock, info_type *info, return_info_t *return_info);
void handle_form(int sock, info_type *info, return_info_t *return_info);

#endif
