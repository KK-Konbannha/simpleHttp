#include "../../include/accept_loop/default_loop.h"
#include "../../include/http_session.h"
#include "../../include/lib.h"

void default_loop(int sock_listen, int auth) {

  int epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    perror("epoll_create1");
    return;
  }

  while (1) {
    struct sockaddr addr;
    int sock_client = 0;
    int len = sizeof(addr);
    int ret = 0;
    info_type info;
    return_info_t return_info;

    init_info(&info, 1);
    init_return_info(&return_info);

    sock_client = accept(sock_listen, &addr, (socklen_t *)&len);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sock_client;
    ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_client, &ev);
    if (ret == -1 && errno != EEXIST) {
      // do nothing
    } else if (ret == -1) {
      perror("epoll_ctl");
      return;
    }
    while (1) {
      ret = http_session(sock_client, &info, &return_info, auth, 0, epoll_fd);
      if (ret == -1 || (ret == EXIT_SUCCESS && info.keep_alive == 0)) {

        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_client, NULL);

        shutdown(sock_client, SHUT_RDWR);
        close(sock_client);

        break;
      } else if (ret == EXIT_SUCCESS && info.keep_alive == 1) {
        init_info(&info, 1);
        init_return_info(&return_info);
      }
    }
  }
}
