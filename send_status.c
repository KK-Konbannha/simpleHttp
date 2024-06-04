#include "exp1.h"

/*
 * 400番台
 */

void send_400(int sock) {
  char buf[16384];
  int ret;

  sprintf(buf, "HTTP/1.0 400 Bad Request\r\n\r\n");
  printf("%s", buf);
  ret = send(sock, buf, strlen(buf), 0);

  if (ret < 0) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }
}

void send_401(int sock) {
  char buf[16384];
  int ret;

  sprintf(buf, "HTTP/1.0 401 Unauthorized\r\n");
  sprintf(buf + strlen(buf),
          "WWW-Authenticate: Basic realm=\"Input your ID and Password\"\r\n");
  sprintf(buf + strlen(buf), "\r\n");
  printf("%s", buf);
  ret = send(sock, buf, strlen(buf), 0);

  if (ret < 0) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }
}

void send_404(int sock) {
  char buf[16384];
  int ret;

  sprintf(buf, "HTTP/1.0 404 Not Found\r\n\r\n");
  printf("%s", buf);
  ret = send(sock, buf, strlen(buf), 0);

  if (ret < 0) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }
}

/*
 * 500番台
 */

void send_500(int sock) {
  char buf[16384];
  int ret;

  sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n\r\n");
  printf("%s", buf);
  ret = send(sock, buf, strlen(buf), 0);

  if (ret < 0) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }
}

void send_501(int sock) {
  char buf[16384];
  int ret;

  sprintf(buf, "HTTP/1.0 501 Not Implemented\r\n\r\n");
  printf("%s", buf);
  ret = send(sock, buf, strlen(buf), 0);

  if (ret < 0) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }
}
