#include "../include/request_handler.h"
#include "../include/send_status.h"

int accept_get(char *buf, int remaining_size, info_type *info,
               return_info_t *return_info, int is_head) {
  int let = 0, recv_size = 0;
  int find_end = 0;
  char *start = NULL, *end = NULL, *token = NULL, *saveptr = NULL;

  printf("--buf\n%s\n\n", buf);
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

  if (find_end != 1) {
    return_info->code = 400;
    return EXIT_FAILURE;
  }

  printf("--buf\n%s\n\n", buf);

  // strtok_rで使用するためのコピーを作成
  char *buf_copy = (char *)malloc(strlen(buf) + 1);
  if (buf_copy == NULL) {
    perror("malloc");
    return_info->code = 500;
    return EXIT_FAILURE;
  }
  strncpy(buf_copy, buf, strlen(buf) + 1);

  // ヘッダーがあるなら解釈する
  // \r\nで残りを分割してparse_headerで解釈
  // (なければ初めから終了条件が満たされる)
  for (token = strtok_r(buf_copy, "\r\n", &saveptr); token;
       token = strtok_r(NULL, "\r\n", &saveptr)) {
    printf("token: %s\n", token);
    let = parse_header(token, info);
    if (let != EXIT_SUCCESS) {
      printf("parse_header failed\n");
      return_info->code = 400;
      free(buf_copy);
      return EXIT_FAILURE;
    }
  }

  free(buf_copy);

  // 認証情報があるか、正しいかをチェック
  // なければ401を返す
  // if (check_auth(info)) {
  //  send_401(sock);
  //  return;
  // }

  return EXIT_SUCCESS;
}

int accept_post(char *buf, int remaining_size, info_type *info,
                return_info_t *return_info) {
  int let = 0, recv_size = 0;
  int find_end = 0, is_remaining_body = 0;
  char *start = NULL, *end = NULL, *token = NULL, *saveptr = NULL, *body = NULL;

  printf("--buf\n%s\n\n", buf);
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

  if (find_end != 1) {
    return_info->code = 400;
    return EXIT_FAILURE;
  }

  printf("--buf\n%s\n\n", buf);

  if (strcmp(buf, "\r\n") == 0) {
    return_info->code = 400;
    return EXIT_FAILURE;
  }

  // strtok_rで使用するためのコピーを作成
  char *buf_copy = (char *)malloc(strlen(buf) + 1);
  if (buf_copy == NULL) {
    perror("malloc");
    return_info->code = 500;
    return EXIT_FAILURE;
  }
  strncpy(buf_copy, buf, strlen(buf) + 1);

  info->content_length = 0;

  // ヘッダーがあるなら解釈する
  // \r\nで残りを分割してparse_headerで解釈
  // (なければ初めから終了条件が満たされる)
  for (token = strtok_r(buf_copy, "\r\n", &saveptr); token;
       token = strtok_r(NULL, "\r\n", &saveptr)) {
    size_t token_offset = token - buf_copy;
    if (token_offset < 4) {
      continue;
    } else {
      const char *original_pos = buf + token_offset;
      printf("token_offset: %ld\n", token_offset);
      printf("strncmp: %d\n", strncmp(original_pos - 4, "\r\n\r\n", 4));

      if (strncmp(original_pos - 4, "\r\n\r\n", 4) == 0) {
        is_remaining_body = 1;
      }
    }

    if (is_remaining_body) {
      break;
    }

    printf("token: %s\n", token);
    let = parse_header(token, info);
    if (let != 1) {
      printf("parse_header failed\n");
      free(buf_copy);
      return_info->code = 400;
      return EXIT_FAILURE;
    }
  }

  // 認証情報があるか、正しいかをチェック
  // なければ401を返す
  // if (check_auth(info)) {
  // send_401(sock);
  // return;
  // }

  // content-lengthがあるかチェック
  // なければ400を返す
  if (info->content_length == 0) {
    printf("content-length is 0\n");
    return_info->code = 400;
    free(buf_copy);
    return EXIT_FAILURE;
  }

  // ボディ部分を取得
  // ボディ部分が残っている場合はそれを使う
  recv_size = 0;
  body = (char *)malloc(info->content_length + 1);
  if (is_remaining_body) {
    if (token == NULL) {
      printf("token is NULL\n");

      free(body);
      free(buf_copy);
      return_info->code = 500;
      return EXIT_FAILURE;
    }

    strcpy(body, token);
    recv_size = strlen(token);
  }

  free(buf_copy);

  if (recv_size < info->content_length) {
    return EXIT_FAILURE;
  }

  body[info->content_length] = '\0';
  printf("body: %s\n\n", body);

  strncpy(info->body, body, info->content_length);

  free(body);

  return EXIT_SUCCESS;
}

int parse_header(char *field, info_type *info) {
  char *key = NULL, *value = NULL, *token = NULL, *saveptr = NULL;

  char *field_copy = (char *)malloc(strlen(field) + 1);
  if (field_copy == NULL) {
    perror("malloc");
    return EXIT_FAILURE;
  }
  strcpy(field_copy, field);

  token = strtok_r(field_copy, ":", &saveptr);
  if (token == NULL) {
    free(field_copy);
    return EXIT_FAILURE;
  }

  key = token;
  value = field + strlen(key) + 2;
  printf("key: %s, value: %s\n", key, value);

  if (strcmp(key, "Authorization") == 0) {
    strcpy(info->auth, value);
    printf("auth: %s\n", info->auth);
  } else if (strcmp(key, "Content-Length") == 0) {
    info->content_length = atoi(value);
    printf("content-length: %d\n", info->content_length);
  }

  return EXIT_SUCCESS;
}

int parse_request(char *status, info_type *pinfo) {
  char *method = NULL, *path = NULL, *version = NULL;
  char *token = NULL, *saveptr = NULL;

  char *status_copy = (char *)malloc(strlen(status) + 1);
  if (status_copy == NULL) {
    perror("malloc");
    return -1;
  }
  strcpy(status_copy, status);

  token = strtok_r(status_copy, " ", &saveptr);
  if (token == NULL) {
    free(status_copy);
    return EXIT_FAILURE;
  }

  method = token;
  if (strlen(method) > 7) {
    free(status_copy);
    return EXIT_FAILURE;
  }
  printf("method: %s\n", method);

  token = strtok_r(NULL, " ", &saveptr);
  if (token == NULL || token[0] != '/' || strlen(token) > 255) {
    free(status_copy);
    return EXIT_FAILURE;
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
    return EXIT_FAILURE;
  }

  printf("version: %s\n", version);

  strcpy(pinfo->method, method);
  strcpy(pinfo->path, path);
  strcpy(pinfo->version, version);

  return EXIT_SUCCESS;
}
