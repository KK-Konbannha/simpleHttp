#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef EPOLL_LOOP_H
#define EPOLL_LOOP_H

#define MAX_EVENTS 1200

void epoll_loop(int sock_listen);

#endif
