#include "../../include/accept_loop/default_loop.h"
#include "../../include/http_session.h"
#include "../../include/lib.h"

void default_loop(int sock_listen, int auth) {

  while (1) {
    struct sockaddr addr;
    int sock_client = 0;
    int len = sizeof(addr);
    int ret = 0;
    info_type info;
    strcpy(info.body, "");
    info.body_size = 0;

    sock_client = accept(sock_listen, &addr, (socklen_t *)&len);
    while (1) {
      ret = http_session(sock_client, &info, auth);
      if (ret == -1 || ret == EXIT_SUCCESS) {
        break;
      }
    }

    shutdown(sock_client, SHUT_RDWR);
    close(sock_client);
  }
}
