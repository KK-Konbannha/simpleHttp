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
  int timeout;
  info_type info;
  return_info_t return_info;
} client_info;

typedef struct Node {
  client_info *data;
  struct Node *next;
  struct Node *prev;
} Node;

void epoll_loop(int sock_listen, int auth);

Node *create_node(void *data);
void insert_node_at_tail(Node *head, Node *node);
void delete_node_by_sock_fd(Node *head, int sock_fd);

#endif
