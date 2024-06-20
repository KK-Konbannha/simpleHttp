#include "../include/send_status.h"

void send_status(int sock, info_type *info, return_info_t *return_info) {
  switch (return_info->code) {
  case 200:
    _send_200(sock, info, return_info);
    break;
  case 301:
    _send_301(sock, return_info->new_path, info);
    break;
  case 302:
    _send_302(sock, return_info->new_path, info);
    break;
  case 303:
    _send_303(sock, return_info->new_path, info);
    break;
  case 400:
    _send_400(sock, info);
    break;
  case 401:
    _send_401(sock, info);
    break;
  case 403:
    _send_403(sock, info);
    break;
  case 404:
    _send_404(sock, info);
    break;
  case 408:
    _send_408(sock, info);
    break;
  case 414:
    _send_414(sock, info);
    break;
  case 418:
    _send_418(sock, info);
    break;
  case 500:
    _send_500(sock, info);
    break;
  case 501:
    _send_501(sock, info);
    break;
  default:
    break;
  }

  return;
}

/*
 * 200番台
 */

void _send_200(int sock, info_type *info, return_info_t *return_info) {
  char buf[256];
  int ret;

  // 200 OK
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf + strlen(buf), "Content-Type: %s\r\n", return_info->type);
  sprintf(buf + strlen(buf), "Content-Length: %d\r\n", return_info->size);
  if (info->keep_alive) {
    sprintf(buf + strlen(buf), "Connection: keep-alive\r\n");
  } else {
    sprintf(buf + strlen(buf), "Connection: close\r\n");
  }
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
    return;
  }

  // bodyの送信
  ret = send(sock, return_info->body, return_info->size, 0);
  if (ret < 0) {
    perror("send");
    return;
  }

  return;
}

/*
 * 300番台
 */

void _send_301(int sock, char *location, info_type *info) {
  char buf[256];
  int ret;

  // 301 Moved Permanently
  sprintf(buf, "HTTP/1.0 301 Moved Permanently\r\n");
  sprintf(buf + strlen(buf), "Location: %s\r\n", location);
  if (info->keep_alive) {
    sprintf(buf + strlen(buf), "Connection: keep-alive\r\n");
  } else {
    sprintf(buf + strlen(buf), "Connection: close\r\n");
  }
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
    return;
  }

  return;
}

void _send_302(int sock, char *location, info_type *info) {
  char buf[256];
  int ret;

  // 302 Found
  sprintf(buf, "HTTP/1.0 302 Found\r\n");
  sprintf(buf + strlen(buf), "Location: %s\r\n", location);
  if (info->keep_alive) {
    sprintf(buf + strlen(buf), "Connection: keep-alive\r\n");
  } else {
    sprintf(buf + strlen(buf), "Connection: close\r\n");
  }
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
    return;
  }

  return;
}

void _send_303(int sock, char *location, info_type *info) {
  char buf[256];
  int ret;

  // 303 See Other
  sprintf(buf, "HTTP/1.0 303 See Other\r\n");
  sprintf(buf + strlen(buf), "Location: %s\r\n", location);
  if (info->keep_alive) {
    sprintf(buf + strlen(buf), "Connection: keep-alive\r\n");
  } else {
    sprintf(buf + strlen(buf), "Connection: close\r\n");
  }
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
    return;
  }

  return;
}

/*
 * 400番台
 */

void _send_400(int sock, info_type *info) {
  char buf[64];
  int ret;

  // 400 Bad Request
  sprintf(buf, "HTTP/1.0 400 Bad Request\r\n");
  if (info->keep_alive) {
    sprintf(buf + strlen(buf), "Connection: keep-alive\r\n");
  } else {
    sprintf(buf + strlen(buf), "Connection: close\r\n");
  }
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
  }

  return;
}

void _send_401(int sock, info_type *info) {
  char buf[256];
  int ret;

  // 401 Unauthorized
  sprintf(buf, "HTTP/1.0 401 Unauthorized\r\n");
  sprintf(buf + strlen(buf),
          "WWW-Authenticate: Basic realm=\"Input your ID and Password\"\r\n");
  if (info->keep_alive) {
    sprintf(buf + strlen(buf), "Connection: keep-alive\r\n");
  } else {
    sprintf(buf + strlen(buf), "Connection: close\r\n");
  }
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
  }
}

void _send_403(int sock, info_type *info) {
  char buf[64];
  int ret;

  // set keep_alive flag to 0
  info->keep_alive = 0;

  // 403 Forbidden
  sprintf(buf, "HTTP/1.0 403 Forbidden\r\n");
  sprintf(buf + strlen(buf), "Connection: close\r\n");
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
  }
}

void _send_404(int sock, info_type *info) {
  char buf[64];
  int ret;

  // set keep_alive flag to 0
  info->keep_alive = 0;

  // 404 Not Found
  sprintf(buf, "HTTP/1.0 404 Not Found\r\n");
  sprintf(buf + strlen(buf), "Connection: close\r\n");
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
  }
}

void _send_408(int sock, info_type *info) {
  char buf[64];
  int ret;

  // set keep_alive flag to 0
  info->keep_alive = 0;

  // 408 Request Timeout
  sprintf(buf, "HTTP/1.0 408 Request Timeout\r\n");
  sprintf(buf + strlen(buf), "Connection: close\r\n");
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
  }
}

void _send_414(int sock, info_type *info) {
  char buf[64];
  int ret;

  // set keep_alive flag to 0
  info->keep_alive = 0;

  // 414 Request-URI Too Long
  sprintf(buf, "HTTP/1.0 414 Request-URI Too Long\r\n");
  sprintf(buf + strlen(buf), "Connection: close\r\n");
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
  }
}

void _send_418(int sock, info_type *info) {
  char buf[64];
  int ret;

  // set keep_alive flag to 0
  info->keep_alive = 0;

  // 418 I'm a teapot
  sprintf(buf, "HTTP/1.0 418 I'm a teapot\r\n");
  sprintf(buf + strlen(buf), "Connection: close\r\n");
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
  }
}

/*
 * 500番台
 */

void _send_500(int sock, info_type *info) {
  char buf[64];
  int ret;

  // set keep_alive flag to 0
  info->keep_alive = 0;

  // 500 Internal Server Error
  sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
  sprintf(buf + strlen(buf), "Connection: close\r\n");
  sprintf(buf + strlen(buf), "\r\n");

  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
  }
}

void _send_501(int sock, info_type *info) {
  char buf[64];

  // set keep_alive flag to 0
  info->keep_alive = 0;

  int ret;

  // 501 Not Implemented
  sprintf(buf, "HTTP/1.0 501 Not Implemented\r\n");
  sprintf(buf + strlen(buf), "Connection: close\r\n");
  sprintf(buf + strlen(buf), "\r\n");
  // debug
  printf("%s", buf);

  // 送信
  ret = send(sock, buf, strlen(buf), 0);
  if (ret < 0) {
    perror("send");
  }
}
