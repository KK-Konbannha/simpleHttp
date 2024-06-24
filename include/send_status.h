#include "lib.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#ifndef SENDSTATUS_H
#define SENDSTATUS_H

void send_status(int sock, info_type *info, return_info_t *return_info);

void _send_200(int sock, info_type *info, return_info_t *return_info);

void _send_301(int sock, char *location, info_type *info);
void _send_302(int sock, char *location, info_type *info);
void _send_303(int sock, char *location, info_type *info);

void _send_400(int sock, info_type *info);
void _send_401(int sock, info_type *info);
void _send_403(int sock, info_type *info);
void _send_404(int sock, info_type *info);
void _send_408(int sock, info_type *info);
void _send_414(int sock, info_type *info);
void _send_418(int sock, info_type *info);

void _send_500(int sock, info_type *info);
void _send_501(int sock, info_type *info);

#endif
