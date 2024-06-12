#include "../include/handlers.h"
#include "../include/exp1.h"
#include "../include/sendStatus.h"

void handle_index(int sock) {
  return_info_t info;

  FILE *fp = fopen("public/index.html", "r");
  if (fp == NULL) {
    send_404(sock);
    return;
  }

  get_file_info(fp, &info);
  strcpy(info.type, "text/html");

  send_200(sock, &info);

  fclose(fp);

  return;
}

void handle_test(int sock) {
  return_info_t info;

  FILE *fp = fopen("public/test.html", "r");
  if (fp == NULL) {
    send_404(sock);
    return;
  }

  get_file_info(fp, &info);
  strcpy(info.type, "text/html");

  send_200(sock, &info);

  fclose(fp);

  return;
}

void handle_new(int sock) {
  return_info_t info;

  FILE *fp = fopen("public/new.html", "r");
  if (fp == NULL) {
    send_404(sock);
    return;
  }

  get_file_info(fp, &info);
  strcpy(info.type, "text/html");

  send_200(sock, &info);

  fclose(fp);

  return;
}

void handle_done(int sock) {
  return_info_t info;

  FILE *fp = fopen("public/done.html", "r");
  if (fp == NULL) {
    send_404(sock);
    return;
  }

  get_file_info(fp, &info);
  strcpy(info.type, "text/html");

  send_200(sock, &info);

  fclose(fp);

  return;
}

void handle_api(int sock, const char *path) {
  if (strcmp(path, "/api/echo") == 0) {
    // Handle the echo api
    return_info_t info;
    info.body = "Echo API\r\n";
    info.size = strlen(info.body);
    strcpy(info.type, "text/plain");

    send_200(sock, &info);
  } else if (strcmp(path, "/api/date") == 0) {
    // Handle the date api
    return_info_t info;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char buf[64];
    strftime(buf, sizeof(buf), "{\"date\": \"%Y-%m-%d %H:%M:%S\"}\n", tm);

    info.body = buf;
    info.size = strlen(info.body);
    strcpy(info.type, "application/json");

    send_200(sock, &info);
  } else if (strcmp(path, "/api/count") == 0) {
    // Handle the count api
    return_info_t info;
    static int count = 0;
    char buf[64];
    sprintf(buf, "{\"count\": %d}\n", count);

    info.body = buf;
    info.size = strlen(info.body);
    strcpy(info.type, "application/json");

    send_200(sock, &info);
  } else {
    // Handle the 404 API
    send_404(sock);
  }

  return;
}

void handle_static(int sock, const char *path) {
  return_info_t info;

  char *path_copy = (char *)malloc(strlen(path) + 1);
  strcpy(path_copy, path);
  char *ext = strrchr(path_copy, '.');
  if (ext == NULL) {
    send_404(sock);
    return;
  }

  if (strncmp(path, "/static/img/", 12) == 0) {

    if (strcmp(ext, ".png") == 0) {
      strcpy(info.type, "image/png");
    } else if (strcmp(ext, ".jpg") == 0) {
      strcpy(info.type, "image/jpeg");
    } else if (strcmp(ext, ".gif") == 0) {
      strcpy(info.type, "image/gif");
    } else {
      send_404(sock);
      return;
    }

    free(path_copy);

    char *filename = (char *)malloc(strlen(path) + 8);
    sprintf(filename, "public%s", path);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
      send_404(sock);
      return;
    }

    get_file_info(fp, &info);
    send_200(sock, &info);

  } else if (strncmp(path, "/static/css/", 12) == 0) {

  } else if (strncmp(path, "/static/js/", 11) == 0 && strcmp(ext, ".js") == 0) {
    strcpy(info.type, "application/javascript");
    char *filename = (char *)malloc(strlen(path) + 8);
    sprintf(filename, "public%s", path);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
      send_404(sock);
      return;
    }

    get_file_info(fp, &info);
    send_200(sock, &info);

  } else {
    send_404(sock);
    return;
  }

  return;
}
