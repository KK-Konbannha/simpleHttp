#include "../../include/accept_loop/thread_loop.h"
#include "../../include/http_session.h"

void *thread_func(void *arg) {
  int sock_client = *(int *)arg;

  pthread_detach(pthread_self());

  info_type info = {0};

  http_session(sock_client, &info);

  shutdown(sock_client, SHUT_RDWR);
  close(sock_client);

  free(arg);

  pthread_exit(NULL);
}

void thread_loop(int sock_listen) {
  while (1) {
    struct sockaddr_storage from;
    socklen_t len = sizeof(from);
    int *sock_client = (int *)malloc(sizeof(int));
    *sock_client = accept(sock_listen, (struct sockaddr *)&from, &len);
    if (*sock_client == -1) {
      if (errno != EINTR) {
        perror("accept");
      }
      free(sock_client);
    } else {
      pthread_t thread;
      if (pthread_create(&thread, NULL, thread_func, (void *)sock_client) !=
          0) {
        perror("pthread_create");
        free(sock_client);
      }
    }
  }
}
