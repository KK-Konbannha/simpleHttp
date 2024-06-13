#include "../include/exp1lib.h"
#include <stdio.h>

#ifndef SENDSTATUS_H
#define SENDSTATUS_H

void send_200(int sock, return_info_t *info);

void send_301(int sock, char *location);
void send_302(int sock, char *location);
void send_303(int sock, char *location);

void send_400(int sock);
void send_401(int sock);
void send_403(int sock);
void send_404(int sock);
void send_414(int sock);
void send_418(int sock);

void send_500(int sock);
void send_501(int sock);

#endif
