#include "../../include/accept_loop/default_loop.h"
#include "../../include/http_session.h"

void default_loop(int sock_listen) {
  while (1) {
    struct sockaddr addr;
    int sock_client;
    int len;

    sock_client = accept(sock_listen, &addr, (socklen_t *)&len);
    http_session(sock_client);

    shutdown(sock_client, SHUT_RDWR);
    close(sock_client);
  }
}
