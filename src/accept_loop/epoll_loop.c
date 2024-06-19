#include "../../include/accept_loop/epoll_loop.h"
#include "../../include/http_session.h"
#include "../../include/lib.h"

void epoll_loop(int sock_listen, int auth) {
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
        if (sock_client < 0) {
          perror("accept");
          continue;
        }

        info_type *info = (info_type *)malloc(sizeof(info_type));
        if (info == NULL) {
          perror("malloc");
          close(sock_client);
          continue;
        }
        info->body_size = 0;
        strcpy(info->body, "");

        set_nonblocking(sock_client);

        client_info *client = (client_info *)malloc(sizeof(client_info));
        if (client == NULL) {
          perror("malloc");
          close(sock_client);
          free(info);
          continue;
        }
        client->sock_fd = sock_client;
        client->info = *info;

        ev.events = EPOLLIN;
        ev.data.ptr = client;
        printf("sock_client: %d\n", sock_client);
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_client, &ev) == -1) {
          perror("epoll_ctl: sock_client");
          close(sock_client);
          free(info);
          continue;
        }
      } else {
        client_info *client = (client_info *)events[i].data.ptr;
        int sock_client = client->sock_fd;
        info_type *info = &client->info;

        printf("sock_client: %d\n", sock_client);

        int ret = http_session(sock_client, info, auth);

        if (ret == -1 || ret == EXIT_SUCCESS) {
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_client, NULL);

          shutdown(sock_client, SHUT_RDWR);
          close(sock_client);

          free(client);
        }
      }
    }
  }
}
