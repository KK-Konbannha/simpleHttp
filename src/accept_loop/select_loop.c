#include "../../include/accept_loop/select_loop.h"
#include "../../include/http_session.h"
#include "../../include/lib.h"
#include "../../include/send_status.h"

void select_loop(int sock_listen, int auth) {
  Node *head = create_node(NULL);
  if (head == NULL) {
    perror("create_node");
    return;
  }

  while (1) {
    fd_set mask;
    FD_ZERO(&mask);
    FD_SET(sock_listen, &mask);
    int width = sock_listen + 1;
    Node *cur = head;
    while (cur->next != NULL) {
      cur = cur->next;
      client_info *client = (client_info *)cur->data;
      FD_SET(client->sock_fd, &mask);
      if (width <= client->sock_fd) {
        width = client->sock_fd + 1;
      }
    }

    fd_set ready = mask;

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int status = select(width, &ready, NULL, NULL, &timeout);
    if (status < 0) {
      perror("select");
      return;
    } else if (status == 0) {
      cur = head;
      while (cur->next != NULL) {
        cur = cur->next;
        client_info *client = (client_info *)cur->data;
        client->timeout++;
        if (client->timeout > 10) {
          client->return_info.code = 408;
          send_status(client->sock_fd, &client->info, &client->return_info);

          shutdown(client->sock_fd, SHUT_RDWR);
          close(client->sock_fd);

          cur = cur->prev;
          delete_node_by_sock_fd(head, client->sock_fd);
        }
      }
      continue;
    }

    if (FD_ISSET(sock_listen, &ready)) {
      struct sockaddr_storage from;
      socklen_t len = sizeof(from);
      int sock_client = accept(sock_listen, (struct sockaddr *)&from, &len);
      if (sock_client == -1) {
        if (errno != EINTR) {
          perror("accept");
        }
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

      free(info);
      free(return_info);

      Node *node = create_node(client);
      if (node == NULL) {
        close(sock_client);
        free(client);
        continue;
      }
      insert_node_at_tail(head, node);
    }

    cur = head;
    while (cur->next != NULL) {
      cur = cur->next;
      client_info *client = (client_info *)cur->data;
      if (FD_ISSET(client->sock_fd, &ready)) {
        client->timeout = 0;

        int ret = http_session(client->sock_fd, &client->info,
                               &client->return_info, auth, 1, 0);
        if (ret == -1 ||
            (ret == EXIT_SUCCESS && client->info.keep_alive == 0)) {

          shutdown(client->sock_fd, SHUT_RDWR);
          close(client->sock_fd);

          cur = cur->prev;
          delete_node_by_sock_fd(head, client->sock_fd);

        } else if (ret == EXIT_SUCCESS && client->info.keep_alive == 1) {
          init_info(&client->info, 1);
          init_return_info(&client->return_info);
        }
      }
    }
  }
}
