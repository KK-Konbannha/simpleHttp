#include "../../include/accept_loop/select_loop.h"
#include "../../include/http_session.h"
#include "../../include/lib.h"
#include "../../include/send_status.h"

void select_loop(int sock_listen, int auth) {
  int child_num = 0;
  int child_sock[MAX_CHILD_NUM];
  int child_timeout[MAX_CHILD_NUM];

  info_type *child_info[MAX_CHILD_NUM];
  return_info_t *child_return_info[MAX_CHILD_NUM];

  memset(child_sock, 0, sizeof(child_sock));
  memset(child_timeout, 0, sizeof(child_timeout));

  while (1) {
    fd_set mask;
    FD_ZERO(&mask);
    FD_SET(sock_listen, &mask);
    int width = sock_listen + 1;
    for (int i = 0; i < child_num; i++) {
      if (child_sock[i] != -1) {
        FD_SET(child_sock[i], &mask);
        if (width <= child_sock[i]) {
          width = child_sock[i] + 1;
        }
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
      printf("timeout\n");
      printf("child_num: %d\n", child_num);
      for (int i = 0; i < child_num; i++) {
        if (child_sock[i] != -1) {
          child_timeout[i]++;

          if (child_timeout[i] > 10) {
            printf("\x1b[31mtimeout sock %d\n", child_sock[i]);
            child_return_info[i]->code = 408;
            send_status(child_sock[i], child_info[i], child_return_info[i]);

            shutdown(child_sock[i], SHUT_RDWR);
            close(child_sock[i]);
            child_sock[i] = -1;

            printf("\x1b[31mclose sock %d\n\x1b[39m", child_sock[i]);
          }
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
      } else {
        int pos = -1;
        for (int i = 0; i < child_num; i++) {
          if (child_sock[i] == 0) {
            pos = i;
            break;
          }
        }

        if (pos == -1) {
          if (child_num >= MAX_CHILD_NUM) {
            fprintf(stderr, "too many clients\n");
            close(sock_client);
          } else {
            pos = child_num;
            child_num++;
          }
        }

        if (pos != -1) {
          child_sock[pos] = sock_client;
          child_timeout[pos] = 0;

          child_info[pos] = (info_type *)malloc(sizeof(info_type));
          if (child_info[pos] == NULL) {
            perror("malloc");
            close(sock_client);
            child_sock[pos] = -1;
          } else {
            init_info(child_info[pos], 1);
          }

          child_return_info[pos] =
              (return_info_t *)malloc(sizeof(return_info_t));
          if (child_return_info[pos] == NULL) {
            perror("malloc");
            close(sock_client);
            child_sock[pos] = -1;
            free(child_info[pos]);
          } else {
            init_return_info(child_return_info[pos]);
          }
        }
      }
    }

    for (int i = 0; i < child_num; i++) {
      if (child_sock[i] != -1 && FD_ISSET(child_sock[i], &ready)) {
        child_timeout[i] = 0;

        int ret = http_session(child_sock[i], child_info[i],
                               child_return_info[i], auth, 1);
        if (ret == -1 ||
            (ret == EXIT_SUCCESS && child_info[i]->keep_alive == 0)) {
          printf("\x1b[31mclose sock %d\n\x1b[39m", child_sock[i]);

          free(child_info[i]);
          free(child_return_info[i]);

          shutdown(child_sock[i], SHUT_RDWR);
          close(child_sock[i]);

          child_sock[i] = -1;
        } else if (ret == EXIT_SUCCESS && child_info[i]->keep_alive == 1) {
          printf("keep alive sock %d\n", child_sock[i]);
          init_info(child_info[i], 1);
          init_return_info(child_return_info[i]);
        }
      }
    }
  }
}
