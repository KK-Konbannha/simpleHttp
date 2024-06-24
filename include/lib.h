#include <ctype.h>
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
  int content_length;
  int keep_alive;
  char body[8192];
  int body_size;
} info_type;

typedef struct {
  int code;
  int is_head;
  char type[128];
  int size;
  char *body;
  char *name;
  char new_path[256];
} return_info_t;

typedef struct {
  int sock_fd;
  int timeout;
  info_type info;
  return_info_t return_info;
} client_info;

typedef struct Node {
  client_info *data;
  struct Node *next;
  struct Node *prev;
} Node;

int tcp_listen(int port);
void init_info(info_type *info, int keep_alive);
void init_return_info(return_info_t *return_info);
void set_nonblocking(int sock);
void get_file_info(FILE *fp, return_info_t *info);

void to_lower(char *str);

Node *create_node(void *data);
void insert_node_at_tail(Node *head, Node *node);
void delete_node_by_sock_fd(Node *head, int sock_fd);
void get_current_node_num(Node *head);

#endif
