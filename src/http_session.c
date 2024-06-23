#include "../include/http_session.h"
#include "../include/lib.h"
#include "../include/request_handler.h"
#include "../include/router.h"
#include "../include/send_status.h"

// return -1: 異常値
// return EXIT_FAILURE: 解析失敗
// return 2: POSTの場合、bodyを受け取っていない
int analyze_request(char *request_token, info_type *info,
                    return_info_t *return_info, int auth) {
  int len = 0, token_len = 0, remaining_size = 0, recv_size = info->body_size;
  char *token = NULL, *saveptr = NULL, *remaining_request_token = NULL;

  // strtok_rで使用するためのコピーを作成
  char *request_token_copy = (char *)malloc(recv_size + 1);
  if (request_token_copy == NULL) {
    perror("malloc");
    return -1;
  }
  strncpy(request_token_copy, request_token, recv_size + 1);

  // 改行までの情報を取得
  token = strtok_r(request_token_copy, "\r\n", &saveptr);
  if (token == NULL) {
    printf("token is NULL\n");
    return_info->code = 400;
    free(request_token_copy);
    return EXIT_FAILURE;
  }
  token_len = strlen(token);
  // \r\nがない場合(\nだけの場合もある)
  if ((request_token[token_len] != '\r' ||
       request_token[token_len + 1] != '\n') &&
      (request_token[token_len] != '\n')) {
    printf("token is invalid\n");
    printf("token is %s\n", token);
    printf("request_token is %s\n", request_token);
    return_info->code = 400;
    free(request_token_copy);
    return EXIT_FAILURE;
  }

  // リクエスト部分を解析
  // 解析結果(メソッド、パス、認証情報)をinfoに格納
  len = parse_request(token, info, return_info);
  if (len == EXIT_FAILURE) {
    printf("return_info->code: %d\n", return_info->code);
    free(request_token_copy);
    return EXIT_FAILURE;
  }

  free(request_token_copy);

  // 解析結果のメソッドがGET, HEADでない場合
  // 501 Not Implementedを返す
  if (strcmp(info->method, "GET") == 0 || strcmp(info->method, "HEAD") == 0) {
    // リクエスト部分を除いた残りのrequest_tokenとそのサイズを渡す
    remaining_request_token = request_token + token_len + 2;
    remaining_size = sizeof(request_token) - recv_size - token_len - 2;

    printf("--request_token\n%s\n\n", request_token);
    printf("--remaining_request_token\n%s\n\n", remaining_request_token);

    int ret = accept_get(remaining_request_token, remaining_size, info,
                         return_info, auth);
    if (ret == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }

  } else if (strcmp(info->method, "POST") == 0) {
    remaining_request_token = request_token + token_len + 2;
    remaining_size = sizeof(request_token) - recv_size - token_len - 2;

    printf("--request_token\n%s\n\n", request_token);
    printf("--remaining_request_token\n%s\n\n", remaining_request_token);

    int ret = accept_post(remaining_request_token, remaining_size, info,
                          return_info, auth);
    if (ret == EXIT_FAILURE) {
      return EXIT_FAILURE;
    } else if (ret == 2) {
      return 2;
    }
  } else if (strcmp(info->method, "PUT") == 0 ||
             strcmp(info->method, "DELETE") == 0 ||
             strcmp(info->method, "OPTIONS") == 0 ||
             strcmp(info->method, "TRACE") == 0 ||
             strcmp(info->method, "CONNECT") == 0) {
    return_info->code = 501;
    return EXIT_FAILURE;
  } else {
    return_info->code = 400;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

// -1: 異常値
// EXIT_FAILURE: 解析失敗
int http_session(int sock, info_type *info, return_info_t *return_info,
                 int auth, int is_non_blocking, int epoll_fd) {
  int ret = 0;
  int recv_size = info->body_size;
  char buf[8192] = "";

  set_nonblocking(sock);

  // bufにinfo->bodyの内容をコピー
  strcpy(buf, info->body);

  if (!is_non_blocking) {
    struct epoll_event events[1];
    int nfds = epoll_wait(epoll_fd, events, 1, 10000);
    if (nfds == -1) {
      perror("epoll_wait");
      return -1;
    } else if (nfds == 0) {
      return_info->code = 408;
      send_status(sock, info, return_info);
      return EXIT_SUCCESS;
    }
  }
  int len = recv(sock, buf + recv_size, 8192 - recv_size, 0);
  if (len <= 0) {
    perror("recv");
    return -1;
  }
  recv_size += len;
  info->body_size = recv_size;

  // info->bodyを更新
  strcpy(info->body, buf);

  // リクエスト部分が存在するかチェック
  if (strstr(buf, "\r\n\r\n") == NULL) {
    return EXIT_FAILURE;
  }

  // リクエスト部分を解析
  // 解析結果(メソッド、パス、認証情報)をinfoに格納
  ret = analyze_request(buf, info, return_info, auth);
  if (ret == EXIT_FAILURE) {
    send_status(sock, info, return_info);
    return EXIT_SUCCESS;
  } else if (ret == 2) {
    // bodyを受け取っていない
    return EXIT_FAILURE;
  }

  // ルーティング
  route_request(sock, info, return_info);
  if (strcmp(info->method, "HEAD") == 0) {
    return_info->is_head = 1;
  }
  send_status(sock, info, return_info);

  return EXIT_SUCCESS;
}
