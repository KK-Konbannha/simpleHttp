#include "../include/send_status.h"

int tcp_listen(int port) {
  int server_socket, yes = 1;
  struct sockaddr_in server_addr;

  // ソケットの生成
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    perror("socket");
    return -1;
  }

  // ソケットの設定
  bzero((char *)&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0) {
    perror("bind");
    return -1;
  }

  // ソケットの待ち受け
  if (listen(server_socket, 5) < 0) {
    perror("listen");
    return -1;
  }

  return server_socket;
}

void init_info(info_type *info, int keep_alive) {
  strcpy(info->method, "");
  strcpy(info->path, "");
  strcpy(info->version, "");
  strcpy(info->type, "");
  strcpy(info->auth, "");
  info->content_length = 0;
  strcpy(info->body, "");
  info->body_size = 0;
  info->keep_alive = keep_alive;
}

void init_return_info(return_info_t *return_info) {
  return_info->code = 200;
  return_info->is_head = 0;
  strcpy(return_info->type, "");
  return_info->body = NULL;
  return_info->name = NULL;
  return_info->size = 0;
  strcpy(return_info->new_path, "");
}

void set_nonblocking(int sock) {
  int opts = fcntl(sock, F_GETFL);
  if (opts < 0) {
    perror("fcntl(F_GETFL)");
    exit(EXIT_FAILURE);
  }
  opts = (opts | O_NONBLOCK);
  if (fcntl(sock, F_SETFL, opts) < 0) {
    perror("fcntl(F_SETFL)");
    exit(EXIT_FAILURE);
  }
}

void get_file_info(FILE *fp, return_info_t *info) {
  fseek(fp, 0, SEEK_END);
  info->size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  if (info->size <= 0) {
    info->size = 0;
    info->body = NULL;
    return;
  }

  info->body = (char *)malloc(info->size);
  fread(info->body, sizeof(char), info->size, fp);

  return;
}

void to_lower(char *str) {
  while (*str) {
    *str = tolower((unsigned char)*str);
    str++;
  }
}

Node *create_node(void *data) {
  Node *node = (Node *)malloc(sizeof(Node));
  if (node == NULL) {
    perror("malloc");
    return NULL;
  }

  node->data = data;
  node->next = NULL;
  node->prev = NULL;

  return node;
}

void insert_node_at_tail(Node *head, Node *node) {
  Node *cur = head;
  while (cur->next != NULL) {
    cur = cur->next;
  }

  cur->next = node;
  node->prev = cur;
}

void delete_node_by_sock_fd(Node *head, int sock_fd) {
  Node *cur = head->next; // スタートポイントを修正
  while (cur != NULL) {
    client_info *client = (client_info *)cur->data;
    if (client != NULL && client->sock_fd == sock_fd) {
      cur->prev->next = cur->next;
      if (cur->next != NULL) {
        cur->next->prev = cur->prev;
      }

      // client_infoのメモリを解放
      free(client->return_info.body);
      free(client->return_info.name);
      free(client);

      free(cur);
      return;
    }
    cur = cur->next;
  }
}

void get_current_node_num(Node *head) {
  Node *cur = head;
  int num = 0;
  while (cur->next != NULL) {
    cur = cur->next;
    num++;
  }
}
