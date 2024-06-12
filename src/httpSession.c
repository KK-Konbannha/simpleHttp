#include "../include/httpSession.h"
#include "../include/exp1.h"
#include "../include/requestHandler.h"
#include "../include/sendStatus.h"

int http_session(int sock) {
  int recv_size = 0, len = 0, token_len = 0, remaining_size = 0;
  char buf[8192] = "";
  info_type info = {0};
  char *token = NULL, *saveptr = NULL, *remaining_buf = NULL;

  // recvでデータを受信
  recv_size = recv(sock, buf, sizeof(buf) - 1, 0);
  if (recv_size < 0) {
    perror("recv");
    return -1;
  }

  // strtok_rで使用するためのコピーを作成
  char *buf_copy = (char *)malloc(recv_size + 1);
  if (buf_copy == NULL) {
    perror("malloc");
    send_500(sock);
    return -1;
  }
  strlcpy(buf_copy, buf, recv_size + 1);

  // 改行までの情報を取得
  token = strtok_r(buf_copy, "\r\n", &saveptr);
  if (token == NULL) {
    printf("token is NULL\n");
    send_400(sock);
    free(buf_copy);
    return -1;
  }
  token_len = strlen(token);
  // \r\nがない場合(\nだけの場合もある)
  if (buf[token_len] != '\r' || buf[token_len + 1] != '\n') {
    printf("token is invalid\n");
    send_400(sock);
    free(buf_copy);
    return -1;
  }

  // リクエスト部分を解析
  // 解析結果(メソッド、パス、認証情報)をinfoに格納
  len = parse_request(token, &info);
  if (len == 0) {
    printf("parse request failed\n");
    send_400(sock);
    free(buf_copy);
    return -1;
  } else if (len == -1) {
    printf("too long url\n");
    send_414(sock);
    free(buf_copy);
    return -1;
  }

  free(buf_copy);

  // 解析結果のメソッドがGET, HEADでない場合
  // 501 Not Implementedを返す
  if (strcmp(info.method, "GET") == 0 || strcmp(info.method, "HEAD") == 0) {
    // リクエスト部分を除いた残りのbufとそのサイズを渡す
    remaining_buf = buf + token_len + 2;
    remaining_size = sizeof(buf) - recv_size - token_len - 2;

    printf("--buf\n%s\n\n", buf);

    accept_get(sock, remaining_buf, remaining_size, &info,
               strcmp(info.method, "HEAD") == 0);

  } else if (strcmp(info.method, "POST") == 0) {
    remaining_buf = buf + token_len + 2;
    remaining_size = sizeof(buf) - recv_size - token_len - 2;

    printf("--buf\n%s\n\n", buf);

    accept_post(sock, remaining_buf, remaining_size, &info);
  } else if (strcmp(info.method, "PUT") == 0 ||
             strcmp(info.method, "DELETE") == 0 ||
             strcmp(info.method, "OPTIONS") == 0 ||
             strcmp(info.method, "TRACE") == 0 ||
             strcmp(info.method, "CONNECT") == 0) {
    send_501(sock);
  } else {
    send_400(sock);
  }

  return 0;
}
