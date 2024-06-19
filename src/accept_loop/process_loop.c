#include "../../include/accept_loop/process_loop.h"
#include "../../include/http_session.h"
#include "../../include/lib.h"

void process_loop(int sock_listen, int auth) {
  struct sigaction sa;
  sigaction(SIGCHLD, NULL, &sa);
  sa.sa_flags = SA_NODEFER;
  sigaction(SIGCHLD, &sa, NULL);

  while (1) {
    struct sockaddr addr;
    int sock_client;
    int len;

    sock_client = accept(sock_listen, &addr, (socklen_t *)&len);
    if (sock_client == -1) {
      if (errno != EINTR) {
        perror("accept");
      }
    } else {
      pid_t pid = fork();
      if (pid == -1) {
        perror("fork");
      } else if (pid == 0) {
        info_type info = {0};
        strcpy(info.body, "");
        info.body_size = 0;

        while (1) {
          int ret = http_session(sock_client, &info);
          if (ret == -1 || ret == EXIT_SUCCESS) {
            break;
          }
        }
        shutdown(sock_client, SHUT_RDWR);
        close(sock_client);
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
