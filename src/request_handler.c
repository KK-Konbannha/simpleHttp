#include "../include/request_handler.h"
#include "../include/auth.h"
#include "../include/send_status.h"

int accept_get(char *buf, int remaining_size, info_type *info,
               return_info_t *return_info, int auth) {
  int let = 0;
  char *token = NULL, *saveptr = NULL;

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
  int is_find_keep_alive = 0;
  for (token = strtok_r(buf_copy, "\r\n", &saveptr); token;
       token = strtok_r(NULL, "\r\n", &saveptr)) {
    printf("token: %s\n", token);
    let = parse_header(token, info, &is_find_keep_alive);
    if (let != EXIT_SUCCESS) {
      printf("parse_header failed\n");
      return_info->code = 400;
      free(buf_copy);
      return EXIT_FAILURE;
    }
  }

  if (is_find_keep_alive == 0) {
    info->keep_alive = 0;
  }
  printf("keep-alive: %d\n", info->keep_alive);

  free(buf_copy);

  int ret = check_auth(info->auth, auth);
  if (ret != 0) {
    return_info->code = 401;
    return EXIT_FAILURE;
  }
  printf("ret: %d\n", ret);

  return EXIT_SUCCESS;
}

// まだbodyを受け取っていない時は2を返す
int accept_post(char *buf, int remaining_size, info_type *info,
                return_info_t *return_info, int auth) {
  int let = 0, is_found_cr_lf_cr_lf = 0;
  char *token = NULL, *saveptr = NULL, *body = NULL;

  // まだbodyを受け取っていない時は2を返す
  if (strcmp(buf, "\r\n") == 0) {
    return 2;
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
  int is_find_keep_alive = 0;
  for (token = strtok_r(buf_copy, "\r\n", &saveptr); token;
       token = strtok_r(NULL, "\r\n", &saveptr)) {
    size_t token_offset = token - buf_copy;
    if (token_offset < 4) {
      continue;
    } else {
      const char *original_pos = buf + token_offset;

      if (strncmp(original_pos - 4, "\r\n\r\n", 4) == 0) {
        is_found_cr_lf_cr_lf = 1;
      }
    }

    if (is_found_cr_lf_cr_lf) {
      break;
    }

    let = parse_header(token, info, &is_find_keep_alive);
    if (let != EXIT_SUCCESS) {
      printf("parse_header failed\n");
      free(buf_copy);
      return_info->code = 400;
      return EXIT_FAILURE;
    }
  }

  if (is_find_keep_alive == 0) {
    info->keep_alive = 0;
  }

  // 認証情報をチェック
  if (check_auth(info->auth, auth)) {
    return_info->code = 401;
    free(buf_copy);
    return EXIT_FAILURE;
  }

  // content-lengthがあるかチェック
  // なければ400を返す
  if (info->content_length == 0) {
    printf("content-length is 0\n");
    return_info->code = 400;
    free(buf_copy);
    return EXIT_FAILURE;
  }

  body = (char *)malloc(info->content_length + 1);

  if (strlen(token) < info->content_length) {
    printf("body is too short\n");

    free(body);
    free(buf_copy);
    return 2;
  }

  strcpy(body, token);

  free(buf_copy);

  body[info->content_length] = '\0';
  printf("body: %s\n\n", body);

  strncpy(info->body, body, info->content_length);
  info->body[info->content_length] = '\0';

  free(body);

  return EXIT_SUCCESS;
}

int parse_header(char *field, info_type *info, int *is_find_keep_alive) {
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
  } else if (strcmp(key, "Connection") == 0) {
    *is_find_keep_alive = 1;
    if (strcmp(value, "keep-alive") == 0) {
      info->keep_alive = 1;
    } else {
      info->keep_alive = 0;
    }
  }

  free(field_copy);
  return EXIT_SUCCESS;
}

int parse_request(char *status, info_type *pinfo, return_info_t *return_info) {
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
    return_info->code = 400;
    return EXIT_FAILURE;
  }
  printf("method: %s\n", method);

  token = strtok_r(NULL, " ", &saveptr);
  if (token == NULL || token[0] != '/' || strlen(token) > 255) {
    free(status_copy);
    return_info->code = 400;
    return EXIT_FAILURE;
  }

  path = token;
  if (strlen(path) > 255) {
    free(status_copy);
    return_info->code = 414;
    return EXIT_FAILURE;
  }
  printf("path: %s\n", path);

  version = &status[strlen(method) + 1 + strlen(path) + 1];
  if (strlen(version) <= 0 || strlen(version) > 9 ||
      (strcmp(version, "HTTP/1.0") != 0 && strcmp(version, "HTTP/1.1")) != 0) {
    free(status_copy);
    return_info->code = 400;
    return EXIT_FAILURE;
  }

  printf("version: %s\n", version);

  strcpy(pinfo->method, method);
  strcpy(pinfo->path, path);
  strcpy(pinfo->version, version);

  return EXIT_SUCCESS;
}
