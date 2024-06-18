#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <wait.h>

#ifndef PROCESS_LOOP_H
#define PROCESS_LOOP_H

void process_loop(int sock_listen);

#endif
