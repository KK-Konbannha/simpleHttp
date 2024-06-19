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
    init_info(&info, 1);

    sock_client = accept(sock_listen, &addr, (socklen_t *)&len);
    while (1) {
      ret = http_session(sock_client, &info, auth);
      if (ret == -1 || (ret == EXIT_SUCCESS && info.keep_alive == 0)) {
        shutdown(sock_client, SHUT_RDWR);
        close(sock_client);
        break;
      } else if (ret == EXIT_SUCCESS && info.keep_alive == 1) {
        init_info(&info, 1);
      }
    }
  }
}
