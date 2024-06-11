#include "../include/acceptLoop.h"
#include "../include/exp1.h"
#include "../include/requestHandler.h"

int http_session(int sock);

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(1);
  }
  int sock_listen;
  int port = atoi(argv[1]);

  sock_listen = exp1_tcp_listen(port);

  accept_loop(sock_listen);

  close(sock_listen);

  return 0;
}
