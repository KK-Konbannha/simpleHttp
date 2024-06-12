#include <stdio.h>

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
  char *body;
} info_type;

typedef struct {
  char type[128];
  int size;
  char *body;
  char *name;
} return_info_t;

int exp1_tcp_listen(int port);
int http_session(int sock);
void get_file_info(FILE *fp, return_info_t *info);

#endif
