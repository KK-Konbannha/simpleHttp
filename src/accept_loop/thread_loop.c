#include "../../include/accept_loop/thread_loop.h"
#include "../../include/http_session.h"

void *thread_func(void *args) {
  int sock_client = *(int *)args;
  int auth = ((thread_info *)args)->auth;
  int epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    perror("epoll_create1");
    pthread_exit(NULL);
  }

  pthread_detach(pthread_self());

  info_type info;
  return_info_t return_info;

  init_info(&info, 1);
  init_return_info(&return_info);

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = sock_client;
  int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_client, &ev);
  if (ret == -1 && errno != EEXIST) {
    // do nothing
  } else if (ret == -1) {
    perror("epoll_ctl");
    pthread_exit(NULL);
  }

  while (1) {
    int ret = http_session(sock_client, &info, &return_info, auth, 0, epoll_fd);
    if (ret == -1 || (ret == EXIT_SUCCESS && info.keep_alive == 0)) {
      break;
    } else if (ret == EXIT_SUCCESS && info.keep_alive == 1) {
      init_info(&info, 1);
      init_return_info(&return_info);
    }
  }

  shutdown(sock_client, SHUT_RDWR);
  close(sock_client);

  free(return_info.body);

  free(args);

  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_client, NULL);
  close(epoll_fd);

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
