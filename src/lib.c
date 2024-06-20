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
