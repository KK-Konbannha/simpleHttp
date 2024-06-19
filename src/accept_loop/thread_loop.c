#include "../../include/accept_loop/thread_loop.h"
#include "../../include/http_session.h"

void *thread_func(void *args) {
  int sock_client = *(int *)args;
  int auth = ((thread_info *)args)->auth;

  pthread_detach(pthread_self());

  info_type info;
  init_info(&info, 1);

  while (1) {
    int ret = http_session(sock_client, &info, auth);
    if (ret == -1 || (ret == EXIT_SUCCESS && info.keep_alive == 0)) {
      break;
    } else if (ret == EXIT_SUCCESS && info.keep_alive == 1) {
      init_info(&info, 1);
    }
  }

  shutdown(sock_client, SHUT_RDWR);
  close(sock_client);

  free(args);

  pthread_exit(NULL);
}

void thread_loop(int sock_listen, int auth) {
  while (1) {
    struct sockaddr_storage from;
    socklen_t len = sizeof(from);
    thread_info *info = (thread_info *)malloc(sizeof(thread_info));
    if (info == NULL) {
      perror("malloc");
      continue;
    }
    info->sock_fd = accept(sock_listen, (struct sockaddr *)&from, &len);
    if (info->sock_fd == -1) {
      if (errno != EINTR) {
        perror("accept");
      }
      free(info);
    } else {
      pthread_t thread;
      info->auth = auth;
      if (pthread_create(&thread, NULL, thread_func, (void *)info) != 0) {
        perror("pthread_create");
        free(info);
      }
    }
  }
}
