#include <stdio.h>

#ifndef EXP1LIB_H
#define EXP1LIB_H

typedef struct {
  char method[8];
  char path[256];
  char version[10];
  char real_path[512];
  char type[128];
  char auth[1024];
  int code;
  int size;
  char date[64];
} info_type;

typedef struct {
  char type[128];
  int size;
  char *body;
} return_info_t;

int exp1_tcp_listen(int port);

int http_session(int sock);

void get_file_info(FILE *fp, return_info_t *info);
void check_file(info_type *info);
void send_file(int sock, char *filename);

#endif
