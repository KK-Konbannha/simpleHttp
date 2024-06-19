#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef THREAD_LOOP_H
#define THREAD_LOOP_H

void *thread_func(void *arg);
void thread_loop(int sock_listen, int auth);

#endif
