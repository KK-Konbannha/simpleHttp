#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#ifndef EXP1LIB_H
#define EXP1LIB_H

typedef struct {
  char method[8];
  char path[256];
  char version[10];
  char type[128];
  char auth[1024];
  int code;
  int content_length;
  int keep_alive;
  char body[8192];
  int body_size;
} info_type;

typedef struct {
  int code;
  char type[128];
  int size;
  char *body;
  char *name;
} return_info_t;

int tcp_listen(int port);
void init_info(info_type *info, int keep_alive);
void set_nonblocking(int sock);
void get_file_info(FILE *fp, return_info_t *info);

#endif
