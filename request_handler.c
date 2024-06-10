#include "request_handler.h"
#include "exp1.h"
#include "send_status.h"

void accept_get(int sock, char *buf, int remaining_size, info_type *info,
                int is_head) {
  int let = 0, recv_size = 0;
  int find_end = 0;
  char *start = NULL, *end = NULL, *token = NULL, *saveptr = NULL;

  while (1) {
    printf("buf: %s\n", buf);
    printf("recv_size: %d\n", recv_size);
    printf("remaining_size: %d\n", remaining_size);

    // \r\n(キャリッジリターンとラインフィード)だけの行(終了のしるし)が見つかった場合フラグを立てる
    start = buf;
    while ((end = strstr(start, "\r\n")) != NULL) {
      if (end == start) {
        find_end = 1;
        break;
      }
      start = end + 2;
    }

    if (find_end == 1) {
      break;
    }

    // バッファがいっぱいになった場合
    if (remaining_size <= 0) {
      printf("remaining_size is 0\n");
      send_400(sock);
      return;
    }

    let = recv(sock, buf + recv_size, remaining_size, 0);
    if (let <= 0) {
      shutdown(sock, SHUT_RDWR);
      close(sock);
      return;
    }

    recv_size += let;
    remaining_size -= let;
  }

  printf("buf: %s\n", buf);

  // strtok_rで使用するためのコピーを作成
  char *buf_copy = (char *)malloc(recv_size + 1);
  if (buf_copy == NULL) {
    perror("malloc");
    send_500(sock);
    return;
  }
  strlcpy(buf_copy, buf, recv_size + 1);

  // ヘッダーがあるなら解釈する
  // \r\nで残りを分割してparse_fieldで解釈
  // (なければ初めから終了条件が満たされる)
  for (token = strtok_r(buf_copy, "\r\n", &saveptr); token;
       token = strtok_r(NULL, "\r\n", &saveptr)) {
    printf("token: %s\n", token);
    let = parse_header(token, info);
    if (let != 1) {
      printf("parse_field failed\n");
      send_400(sock);
      free(buf_copy);
      return;
    }
  }

  free(buf_copy);

  // 認証情報があるか、正しいかをチェック
  // なければ401を返す
  // if (check_auth(info)) {
  //  send_401(sock);
  //  return;
  // }

  // ファイルの有無をチェック
  check_file(info);

  // 返答する
  http_reply(sock, info, is_head);

  return;
}

int parse_header(char *field, info_type *info) {
  char *key = NULL, *value = NULL, *token = NULL, *saveptr = NULL;

  char *field_copy = (char *)malloc(strlen(field) + 1);
  if (field_copy == NULL) {
    perror("malloc");
    return 0;
  }
  strcpy(field_copy, field);

  token = strtok_r(field_copy, ":", &saveptr);
  if (token == NULL) {
    free(field_copy);
    return 0;
  }

  key = token;
  value = field + strlen(key) + 2;
  printf("key: %s, value: %s\n", key, value);
  printf("value: %s\n", value);

  if (strcmp(key, "Authorization") == 0) {
    strcpy(info->auth, value);
    printf("auth: %s\n", info->auth);
  }

  return 1;
}

int parse_request(char *status, info_type *pinfo) {
  char *method = NULL, *path = NULL, *version = NULL;
  char *token = NULL, *saveptr = NULL;

  char *status_copy = (char *)malloc(strlen(status) + 1);
  if (status_copy == NULL) {
    perror("malloc");
    return 0;
  }
  strcpy(status_copy, status);

  token = strtok_r(status_copy, " ", &saveptr);
  if (token == NULL) {
    free(status_copy);
    return 0;
  }

  method = token;
  if (strlen(method) > 7) {
    free(status_copy);
    return 0;
  }
  printf("method: %s\n", method);

  token = strtok_r(NULL, " ", &saveptr);
  if (token == NULL || token[0] != '/' || strlen(token) > 255) {
    free(status_copy);
    return 0;
  }

  path = token;
  if (strlen(path) > 255) {
    free(status_copy);
    return -1;
  }
  printf("path: %s\n", path);

  version = &status[strlen(method) + 1 + strlen(path) + 1];
  if (strlen(version) <= 0 || strlen(version) > 9 ||
      (strcmp(version, "HTTP/1.0") != 0 && strcmp(version, "HTTP/1.1")) != 0) {
    free(status_copy);
    return 0;
  }

  printf("version: %s\n", version);

  strcpy(pinfo->method, method);
  strcpy(pinfo->path, path);
  strcpy(pinfo->version, version);

  return 1;
}

void http_reply(int sock, info_type *info, int is_head) {
  char buf[16384];
  int len;
  int ret;

  if (info->code == 404) {
    send_404(sock);
    printf("404 not found %s\n", info->path);
    return;
  }

  if (info->code == 401) {
    send_401(sock);
    printf("401\n");
    return;
  }

  len = sprintf(buf, "HTTP/1.0 200 OK\r\n");
  len += sprintf(buf + len, "Content-Length: %d\r\n", info->size);
  len += sprintf(buf + len, "Content-Type: %s\r\n", info->type);
  // len += sprintf(buf + len, "\r\n");

  ret = send(sock, buf, len, 0);
  if (ret < 0) {
    shutdown(sock, SHUT_RDWR);
    close(sock);
    return;
  }

  if (is_head == 0 && info->code == 200) {
    if (strcmp(info->type, "application/json") == 0) {
      send(sock, "\r\n", 2, 0);

      char buf2[16384];
      len = 0;
      len = sprintf(buf2, "This is loaded data!!\r\n");
      len += sprintf(buf2 + len, "%s\r\n", info->date);
      ret = send(sock, buf2, len, 0);
      if (ret < 0) {
        shutdown(sock, SHUT_RDWR);
        close(sock);
        return;
      }
    } else {
      send(sock, "\r\n", 2, 0);
      send_file(sock, info->real_path);
    }
  }
}
