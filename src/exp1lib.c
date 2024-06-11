#include "../include/exp1.h"
#include "../include/sendStatus.h"

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

void get_file_info(FILE *fp, return_info_t *info) {
  fseek(fp, 0, SEEK_END);
  info->size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  info->body = (char *)malloc(info->size);
  fread(info->body, sizeof(char), info->size, fp);

  return;
}

void check_file(info_type *info) {
  struct stat s;
  int ret;
  char *pext;

  if (strcmp(info->path, "/") == 0) {
    strcpy(info->path, "/index.html");
  }

  if (strncmp(info->path, "/api/data", 4) == 0) {
    info->code = 200;
    strcpy(info->type, "application/json");
    time_t t = time(NULL);
    strcpy(info->date, ctime(&t));
    info->size = strlen("This is loaded data!!\r\n");
    info->size += strlen(info->date);
    info->size += strlen("\r\n");
    return;
  }

  sprintf(info->real_path, "public%s", info->path);
  ret = stat(info->real_path, &s);

  if ((s.st_mode & S_IFMT) == S_IFDIR) {
    sprintf(info->real_path, "public%s/index.html", info->path);
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
  } else if (pext != NULL && strcmp(pext, ".js") == 0) {
    strcpy(info->type, "application/javascript");
  }
}

void send_file(int sock, char *filename) {
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
