#include "exp1lib.h"
#include "exp1.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int exp1_tcp_listen(int port) {
  int sock;
  struct sockaddr_in addr;
  int yes = 1;
  int ret;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  // Test Code: check socket buffer size
  //	socklen_t slen;
  //	int sockBufSize;
  //	getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &sockBufSize, &slen);
  //	printf("Socket Buffer Size(default): %d\n", sockBufSize);

  bzero((char *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    perror("bind");
    exit(1);
  }

  ret = listen(sock, 5);
  if (ret < 0) {
    perror("reader: listen");
    close(sock);
    exit(-1);
  }

  return sock;
}

int exp1_tcp_connect(const char *hostname, int port) {
  int sock;
  int ret;
  struct addrinfo hints, *res, *p;
  char port_str[6];

  snprintf(port_str, sizeof(port_str), "%d", port);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_STREAM;

  ret = getaddrinfo(hostname, port_str, &hints, &res);
  if (ret != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
    return -1;
  }

  for (p = res; p != NULL; p = p->ai_next) {
    sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sock == -1) {
      continue;
    }

    ret = connect(sock, p->ai_addr, p->ai_addrlen);
    if (ret == -1) {
      close(sock);
      continue;
    }

    break; // 成功した場合ループを抜ける
  }

  if (p == NULL) {
    // 接続できなかった場合
    fprintf(stderr, "Could not connect\n");
    return -1;
  }

  freeaddrinfo(res);
  return sock;
}

int exp1_udp_listen(int port) {
  int sock;
  struct sockaddr_in addr;
  int yes = 1;
  int ret;

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(1);
  }

  bzero((char *)&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    perror("bind");
    exit(1);
  }
  return sock;
}

int exp1_udp_connect(const char *hostname, int port) {
  int sock;
  int ret;
  struct addrinfo hints, *res, *p;
  char port_str[6];

  snprintf(port_str, sizeof(port_str), "%d", port);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_DGRAM;

  ret = getaddrinfo(hostname, port_str, &hints, &res);
  if (ret != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
    return -1;
  }

  for (p = res; p != NULL; p = p->ai_next) {
    sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sock == -1) {
      continue;
    }
    ret = connect(sock, p->ai_addr, p->ai_addrlen);
    if (ret == -1) {
      close(sock);
      continue;
    }
    break; // 成功した場合ループを抜ける
  }
  if (p == NULL) {
    // 接続できなかった場合
    fprintf(stderr, "Could not connect\n");
    return -1;
  }
  freeaddrinfo(res);
  return sock;
}

double gettimeofday_sec() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

int exp1_do_talk(int sock) {
  fd_set fds;
  char buf[1024];

  FD_ZERO(&fds);
  FD_SET(0, &fds);
  FD_SET(sock, &fds);

  select(sock + 1, &fds, NULL, NULL, NULL);

  if (FD_ISSET(0, &fds) != 0) {
    char *res = fgets(buf, 1024, stdin);
    if (res == NULL) {
      perror("error in fgets: ");
    }
    int size = write(sock, buf, strlen(buf));
    if (size == -1) {
      perror("error in write: ");
    }
  }

  if (FD_ISSET(sock, &fds) != 0) {
    int ret = recv(sock, buf, 1024, 0);
    if (ret > 0) {
      int size = write(1, buf, ret);
      if (size == -1) {
        perror("error in write: ");
      }
    } else {
      return -1;
    }
  }

  return 1;
}

void exp1_send_file(int sock, char *filename) {
  FILE *fp;
  int len;
  char buf[16384];

  fp = fopen(filename, "r");
  if (fp == NULL) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
    return;
  }

  len = fread(buf, sizeof(char), 16384, fp);
  while (len > 0) {
    int ret = send(sock, buf, len, 0);
    if (ret < 0) {
      shutdown(sock, SHUT_RDWR);
      close(sock);
      break;
    }
    len = fread(buf, sizeof(char), 1460, fp);
  }

  fclose(fp);
}

void exp1_send_404(int sock) {
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

void exp1_http_reply(int sock, exp1_info_type *info) {
  char buf[16384];
  int len;
  int ret;

  if (info->code == 404) {
    exp1_send_404(sock);
    printf("404 not found %s\n", info->path);
    return;
  }

  len = sprintf(buf, "HTTP/1.0 200 OK\r\n");
  len += sprintf(buf + len, "Content-Length: %d\r\n", info->size);
  len += sprintf(buf + len, "Content-Type: %s\r\n", info->type);
  len += sprintf(buf + len, "\r\n");

  ret = send(sock, buf, len, 0);
  if (ret < 0) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
    return;
  }

  exp1_send_file(sock, info->real_path);
}

void exp1_check_file(exp1_info_type *info) {
  struct stat s;
  int ret;
  char *pext;

  sprintf(info->real_path, "html%s", info->path);
  ret = stat(info->real_path, &s);

  if ((s.st_mode & S_IFMT) == S_IFDIR) {
    sprintf(info->real_path, "%s/index.html", info->real_path);
  }

  ret = stat(info->real_path, &s);

  if (ret == -1) {
    info->code = 404;
  } else {
    info->code = 200;
    info->size = (int)s.st_size;
  }

  pext = strstr(info->path, ".");
  if (pext != NULL && strcmp(pext, ".html") == 0) {
    strcpy(info->type, "text/html");
  } else if (pext != NULL && strcmp(pext, ".jpg") == 0) {
    strcpy(info->type, "image/jpeg");
  }
}

void exp1_parse_status(char *status, exp1_info_type *pinfo) {
  char cmd[1024];
  char path[1024];
  char *pext;
  int i, j;

  enum state_type { SEARCH_CMD, SEARCH_PATH, SEARCH_END } state;

  state = SEARCH_CMD;
  j = 0;
  for (i = 0; i < strlen(status); i++) {
    switch (state) {
    case SEARCH_CMD:
      if (status[i] == ' ') {
        cmd[j] = '\0';
        j = 0;
        state = SEARCH_PATH;
      } else {
        cmd[j] = status[i];
        j++;
      }
      break;
    case SEARCH_PATH:
      if (status[i] == ' ') {
        path[j] = '\0';
        j = 0;
        state = SEARCH_END;
      } else {
        path[j] = status[i];
        j++;
      }
      break;
    }
  }

  strcpy(pinfo->cmd, cmd);
  strcpy(pinfo->path, path);
}

int exp1_parse_header(char *buf, int size, exp1_info_type *info) {
  char status[1024];
  int i, j;

  enum state_type { PARSE_STATUS, PARSE_END } state;

  state = PARSE_STATUS;
  j = 0;
  for (i = 0; i < size; i++) {
    switch (state) {
    case PARSE_STATUS:
      if (buf[i] == '\r') {
        status[j] = '\0';
        j = 0;
        state = PARSE_END;
        exp1_parse_status(status, info);
        exp1_check_file(info);
      } else {
        status[j] = buf[i];
        j++;
      }
      break;
    }

    if (state == PARSE_END) {
      return 1;
    }
  }

  return 0;
}

int exp1_http_session(int sock) {
  char buf[2048];
  int recv_size = 0;
  exp1_info_type info;
  int ret = 0;

  while (ret == 0) {
    int size = recv(sock, buf + recv_size, 2048, 0);

    if (size == -1) {
      return -1;
    }

    recv_size += size;
    ret = exp1_parse_header(buf, recv_size, &info);
  }

  exp1_http_reply(sock, &info);

  return 0;
}
