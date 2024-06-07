#include "exp1.h"
#include "request_handler.h"
#include "send_status.h"

int http_session(int sock);

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(1);
  }
  int sock_listen;
  int port = atoi(argv[1]);

  sock_listen = exp1_tcp_listen(port);

  while (1) {
    struct sockaddr addr;
    int sock_client;
    int len;

    sock_client = accept(sock_listen, &addr, (socklen_t *)&len);
    http_session(sock_client);

    shutdown(sock_client, SHUT_RDWR);
    close(sock_client);
  }
}

int http_session(int sock) {
  int recv_size = 0, len = 0, token_len = 0, remaining_size = 0;
  char buf[4096] = "";
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
  if (len != 1) {
    printf("parse request failed\n");
    send_400(sock);
    free(buf_copy);
    return -1;
  }

  free(buf_copy);

  // 解析結果のメソッドがGET, HEADでない場合
  // 501 Not Implementedを返す
  if (strcmp(info.cmd, "GET") == 0 || strcmp(info.cmd, "HEAD") == 0) {
    // リクエスト部分を除いた残りのbufとそのサイズを渡す
    remaining_buf = buf + token_len + 2;
    remaining_size = sizeof(buf) - recv_size - token_len - 2;

    printf("%s", buf);

    accept_get(sock, remaining_buf, remaining_size, &info,
               strcmp(info.cmd, "HEAD") == 0);
  } else {
    send_501(sock);
  }

  return 0;
}
