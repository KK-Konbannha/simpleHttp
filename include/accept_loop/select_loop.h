#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef SELECT_LOOP_H
#define SELECT_LOOP_H

#define MAX_CHILD_NUM 1200

void select_loop(int sock_listen);

#endif
