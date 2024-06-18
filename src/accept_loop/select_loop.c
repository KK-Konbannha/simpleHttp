#include "../../include/accept_loop/select_loop.h"
#include "../../include/http_session.h"
#include "../../include/lib.h"

void select_loop(int sock_listen) {
  int child_num = 0;
  int child_sock[MAX_CHILD_NUM];
  memset(child_sock, 0, sizeof(child_sock));

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
        }
      }
    }

    for (int i = 0; i < child_num; i++) {
      if (child_sock[i] != -1 && FD_ISSET(child_sock[i], &ready)) {
        printf("child_sock[%d] is ready\n", i);

        info_type info;
        http_session(child_sock[i], &info);

        shutdown(child_sock[i], SHUT_RDWR);
        close(child_sock[i]);

        child_sock[i] = -1;
      }
    }
  }
}
