#include "../include/accept_loop/default_loop.h"
#include "../include/accept_loop/epoll_loop.h"
#include "../include/accept_loop/process_loop.h"
#include "../include/accept_loop/select_loop.h"
#include "../include/accept_loop/thread_loop.h"
#include "../include/request_handler.h"

#define DEFAULT_LOOP 0
#define SELECT_LOOP 1
#define THREAD_LOOP 2
#define PROCESS_LOOP 3
#define EPOLL_LOOP 4

int http_session(int sock);

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <mode> <port> [authentication]\n", argv[0]);
    fprintf(stderr, "mode: 0(default) 1(select)"
                    " 2(thread) 3(process) 4(epoll)\n");
    fprintf(stderr, "port: 1-65535\n");
    fprintf(stderr, "authentication: 0(disable) 1(basic)\n");
    exit(1);
  }
  int sock_listen = 0;
  int mode = atoi(argv[1]);
  if (mode < DEFAULT_LOOP || mode > EPOLL_LOOP) {
    fprintf(stderr, "Invalid mode\n");
    exit(1);
  }

  int port = atoi(argv[2]);
  if (port <= 0 || port > 65535) {
    fprintf(stderr, "Invalid port number\n");
    exit(1);
  }

  int auth = 0;
  if (argc == 4) {
    auth = atoi(argv[3]);
    if (auth < 0 || auth > 1) {
      fprintf(stderr, "Invalid authentication\n");
      exit(1);
    }
  }

  sock_listen = tcp_listen(port);
  if (sock_listen < 0) {
    fprintf(stderr, "tcp_listen error\n");
    exit(1);
  }

  switch (mode) {
  case DEFAULT_LOOP:
    default_loop(sock_listen, auth);
    break;
  case SELECT_LOOP:
    select_loop(sock_listen, auth);
    break;
  case THREAD_LOOP:
    thread_loop(sock_listen, auth);
    break;
  case PROCESS_LOOP:
    process_loop(sock_listen, auth);
    break;
  case EPOLL_LOOP:
    epoll_loop(sock_listen, auth);
    break;
  default:
    fprintf(stderr, "Invalid mode\n");
    exit(1);
  }

  close(sock_listen);

  return 0;
}
