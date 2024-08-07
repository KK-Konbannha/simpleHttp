#include "../../include/accept_loop/process_loop.h"
#include "../../include/http_session.h"
#include "../../include/lib.h"

void process_loop(int sock_listen, int auth) {
  struct sigaction sa;
  sigaction(SIGCHLD, NULL, &sa);
  sa.sa_flags = SA_NODEFER;
  sigaction(SIGCHLD, &sa, NULL);

  while (1) {
    struct sockaddr_in addr;
    int sock_client;
    socklen_t len = sizeof(addr);

    memset(&addr, 0, sizeof(addr));

    sock_client =
        accept(sock_listen, (struct sockaddr *)&addr, (socklen_t *)&len);
    if (sock_client == -1) {
      if (errno != EINTR) {
        perror("accept");
      }
    } else {
      pid_t pid = fork();
      if (pid == -1) {
        perror("fork");
      } else if (pid == 0) {
        int epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) {
          perror("epoll_create1");
          return;
        }
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = sock_client;
        int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_client, &ev);
        if (ret == -1 && errno != EEXIST) {
          // do nothing
        } else if (ret == -1) {
          perror("epoll_ctl");
          return;
        }

        info_type info;
        return_info_t return_info;
        init_info(&info, 1);
        init_return_info(&return_info);

        while (1) {
          int ret =
              http_session(sock_client, &info, &return_info, auth, 0, epoll_fd);
          if (ret == -1 || (ret == EXIT_SUCCESS && info.keep_alive == 0)) {
            break;
          } else if (ret == EXIT_SUCCESS && info.keep_alive == 1) {
            init_info(&info, 1);
            init_return_info(&return_info);
          }
        }
        shutdown(sock_client, SHUT_RDWR);
        close(sock_client);

        close(epoll_fd);
        _exit(0);
      } else {
        close(sock_client);
      }

      while (waitpid(-1, NULL, WNOHANG) > 0) {
        ;
      }
    }
  }
}
