#include "../lib.h"
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef EPOLL_LOOP_H
#define EPOLL_LOOP_H

#define MAX_EVENTS 1200

typedef struct {
  int sock_fd;
  info_type info;
} client_info;

void epoll_loop(int sock_listen);

#endif
