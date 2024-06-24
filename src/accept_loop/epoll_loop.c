#include "../../include/accept_loop/epoll_loop.h"
#include "../../include/http_session.h"
#include "../../include/lib.h"
#include "../../include/send_status.h"

void epoll_loop(int sock_listen, int auth) {
  Node *head = create_node(NULL);
  if (head == NULL) {
    perror("create_node");
    return;
  }

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
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 1000);
    if (nfds == -1) {
      perror("epoll_wait");
      close(epoll_fd);
      return;
    } else if (nfds == 0) {
      printf("timeout\n");
      get_current_node_num(head);
      Node *cur = head;
      while (cur != NULL && cur->next != NULL) {
        cur = cur->next;
        client_info *client = (client_info *)cur->data;
        client->timeout++;
        if (client->timeout > 10) {
          client->return_info.code = 408;
          send_status(client->sock_fd, &client->info, &client->return_info);

          cur = cur->prev;
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->sock_fd, NULL);
          delete_node_by_sock_fd(head, client->sock_fd);

          shutdown(client->sock_fd, SHUT_RDWR);
          close(client->sock_fd);
        }
      }
      continue;
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
        init_info(info, 1);

        return_info_t *return_info =
            (return_info_t *)malloc(sizeof(return_info_t));
        if (return_info == NULL) {
          perror("malloc");
          close(sock_client);
          free(info);
          continue;
        }
        init_return_info(return_info);

        client_info *client = (client_info *)malloc(sizeof(client_info));
        if (client == NULL) {
          perror("malloc");
          close(sock_client);
          free(info);
          free(return_info);
          continue;
        }
        client->sock_fd = sock_client;
        client->timeout = 0;
        client->info = *info;
        client->return_info = *return_info;

        Node *node = create_node(client);
        if (node == NULL) {
          close(sock_client);
          free(info);
          free(return_info);
          free(client);
          continue;
        }
        insert_node_at_tail(head, node);

        ev.events = EPOLLIN;
        ev.data.ptr = client;
        printf("sock_client: %d\n", sock_client);
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_client, &ev) == -1) {
          perror("epoll_ctl: sock_client");
          close(sock_client);
          free(info);
          free(return_info);
          free(client);
          continue;
        }
      } else {
        client_info *client = (client_info *)events[i].data.ptr;
        int sock_client = client->sock_fd;
        info_type *info = &client->info;
        return_info_t *return_info = &client->return_info;

        client->timeout = 0;
        printf("sock_client: %d\n", sock_client);

        int ret = http_session(sock_client, info, return_info, auth, 1, 0);

        if (ret == -1 || (ret == EXIT_SUCCESS && info->keep_alive == 0)) {
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_client, NULL);
          delete_node_by_sock_fd(head, sock_client);

          shutdown(sock_client, SHUT_RDWR);
          close(sock_client);
        } else if (ret == EXIT_SUCCESS && info->keep_alive == 1) {
          init_info(info, 1);
          init_return_info(return_info);
        }
      }
    }
  }
}
