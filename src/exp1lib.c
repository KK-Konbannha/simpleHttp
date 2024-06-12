#include "../include/exp1.h"
#include "../include/sendStatus.h"

int exp1_tcp_listen(int port) {
  int sock;
  struct sockaddr_in addr;
  int yes = 1;
  int ret;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  bzero((char *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    perror("bind");
    exit(1);
  }

  ret = listen(sock, 5);
  if (ret < 0) {
    perror("reader: listen");
    close(sock);
    exit(-1);
  }

  return sock;
}

void get_file_info(FILE *fp, return_info_t *info) {
  fseek(fp, 0, SEEK_END);
  info->size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  info->body = (char *)malloc(info->size);
  fread(info->body, sizeof(char), info->size, fp);

  return;
}
