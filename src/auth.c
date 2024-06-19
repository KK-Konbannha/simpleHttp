#include "../include/auth.h"

#define _AUTH_FILE ".auth"

// base64エンコード
int base64_encode(const char *src, char *dst) {
  FILE *fp;
  char *cmd = (char *)malloc(strlen(src) + 64);
  sprintf(cmd, "echo -n \"%s\" | base64", src);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    free(cmd);
    return -1;
  }
  fgets(dst, 128, fp);
  pclose(fp);

  free(cmd);
  return 0;
}

// base64デコード
int base64_decode(const char *src, char *dst) {
  FILE *fp;
  char *cmd = (char *)malloc(strlen(src) + 64);
  sprintf(cmd, "echo -n \"%s\" | base64 -d", src);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    free(cmd);
    return -1;
  }
  fgets(dst, 128, fp);
  pclose(fp);

  free(cmd);
  return 0;
}

// 引数のauthをデコードしてuserとpassに分割
int basic_auth(const char *auth, char *user, char *pass) {
  char *decoded = (char *)malloc(128);
  base64_decode(auth, decoded);
  if (strstr(decoded, ":") == NULL) {
    free(decoded);
    return -1;
  }
  sscanf(decoded, "%[^:]:%s", user, pass);

  free(decoded);
  return 0;
}

// ユーザー名とパスワードを照合
// auth_kind: 0 -> 認証なし, 1 -> Basic認証
int check_auth(const char *auth, int auth_kind) {
  // char *user = (char *)malloc(64);
  // char *pass = (char *)malloc(64);
  char user[128];
  char pass[128];

  if (strlen(auth) == 0) {
    return -1;
  }

  if (auth_kind == 0) {
    return 0;
  } else if (auth_kind == 1) {
    if (strstr(auth, "Basic ") == NULL) {
      return -1;
    }
    if (strlen(auth) < 7 || strlen(auth) > 128) {
      return -1;
    }
    char *auth_base64 = (char *)malloc(strlen(auth) - 6);
    strcpy(auth_base64, auth + 6);
    // 引数のauthをデコードしてuserとpassに分割
    if (basic_auth(auth_base64, user, pass) == -1) {
      free(auth_base64);
      return -1;
    }
    free(auth_base64);
  }

  // ファイルからユーザー名とパスワードを取得
  FILE *fp = fopen(_AUTH_FILE, "r");
  if (fp == NULL) {
    // error出力
    fprintf(stderr, "Error: %s not found\n", _AUTH_FILE);
    return -1;
  }

  char *user_auth = (char *)malloc(128);
  char *pass_auth = (char *)malloc(128);

  // ファイルから1行ずつ読み込んでユーザー名とパスワードを取得、照合
  // ファイルの中身は以下のようになっている
  // user: admin
  // pass: YWRta
  // passはbase64でエンコードされている
  char *line = (char *)malloc(256);
  fgets(line, 256, fp);
  if (sscanf(line, "user: %s", user_auth) != 1) {
    free(user_auth);
    free(pass_auth);
    free(line);
    fclose(fp);
    return -1;
  }
  fgets(line, 256, fp);
  if (sscanf(line, "pass: %s", pass_auth) != 1) {
    free(user_auth);
    free(pass_auth);
    free(line);
    fclose(fp);
    return -1;
  }
  free(line);
  fclose(fp);

  // ファイルから取得したパスワードをデコード
  char *pass_auth_decoded = (char *)malloc(128);
  base64_decode(pass_auth, pass_auth_decoded);
  free(pass_auth);

  // ユーザー名とパスワードを照合
  if (strcmp(user, user_auth) == 0 && strcmp(pass, pass_auth_decoded) == 0) {
    free(user_auth);
    free(pass_auth_decoded);
    return 0;
  }

  free(user_auth);
  free(pass_auth_decoded);
  return -1;
}
