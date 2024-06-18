#include "../../include/accept_loop/epoll_loop.h"
#include "../../include/http_session.h"
#include "../../include/lib.h"

void epoll_loop(int sock_listen) {
  int epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    perror("epoll_create1");
    return;
  }

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = sock_listen;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_listen, &ev) == -1) {
    perror("epoll_ctl: listen_sock");
    close(epoll_fd);
    return;
  }

  struct epoll_event events[MAX_EVENTS];
  while (1) {
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
      perror("epoll_wait");
      close(epoll_fd);
      return;
    }

    for (int i = 0; i < nfds; i++) {
      if (events[i].data.fd == sock_listen) {
        struct sockaddr_storage from;
        socklen_t fromlen = sizeof(from);
        int sock_client =
            accept(sock_listen, (struct sockaddr *)&from, &fromlen);
        if (sock_client == -1) {
          perror("accept");
          continue;
        }

        ev.events = EPOLLIN;
        ev.data.fd = sock_client;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_client, &ev) == -1) {
          perror("epoll_ctl: sock_client");
          close(sock_client);
          continue;
        }
      } else {
        int client_socket = events[i].data.fd;
        info_type info = {0};
        http_session(client_socket, &info);

        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);

        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
      }
    }
  }
}
