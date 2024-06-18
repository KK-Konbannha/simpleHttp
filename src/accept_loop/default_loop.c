#include "../../include/accept_loop/default_loop.h"
#include "../../include/http_session.h"
#include "../../include/lib.h"

void default_loop(int sock_listen) {

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
      printf("sock_client: %d\n", sock_client);
      printf("1: info->body: %s\n", info.body);
      ret = http_session(sock_client, &info);
      printf("ret: %d\n", ret);
      if (ret == -1 || ret == EXIT_SUCCESS) {
        break;
      }
      printf("info->body: %s\n", info.body);
    }

    shutdown(sock_client, SHUT_RDWR);
    close(sock_client);
  }
}
