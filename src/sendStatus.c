#include "../include/sendStatus.h"
#include "../include/exp1.h"

/*
 * 400番台
 */

void send_400(int sock) {
  char buf[64];
  int ret;

  // 400 Bad Request
  int len = sprintf(buf, "HTTP/1.0 400 Bad Request\r\n\r\n");
  if (len < 0) {
    perror("sprintf");
    shutdown(sock, SHUT_RDWR);
    close(sock);
    return;
  }

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }

  return;
}

void send_401(int sock) {
  char buf[256];
  int ret;

  // 401 Unauthorized
  sprintf(buf, "HTTP/1.0 401 Unauthorized\r\n");
  sprintf(buf + strlen(buf),
          "WWW-Authenticate: Basic realm=\"Input your ID and Password\"\r\n");
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }
}

void send_404(int sock) {
  char buf[64];
  int ret;

  // 404 Not Found
  sprintf(buf, "HTTP/1.0 404 Not Found\r\n\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }
}

void send_414(int sock) {
  char buf[64];
  int ret;

  // 414 Request-URI Too Long
  sprintf(buf, "HTTP/1.0 414 Request-URI Too Long\r\n\r\n");

  // debug
  printf("%s", buf);

  // 送信
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
  char buf[64];
  int ret;

  // 500 Internal Server Error
  sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }
}

void send_501(int sock) {
  char buf[64];
  int ret;

  // 501 Not Implemented
  sprintf(buf, "HTTP/1.0 501 Not Implemented\r\n\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }
}
