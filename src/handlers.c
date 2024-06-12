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

void handle_test_cgi(int sock) {
  return_info_t info;
  char const *filename = "public/test_cgi.cgi";
  char *command = (char *)malloc(strlen(filename) + strlen("./") + 1);
  char output[1024] = "";
  char *all_output = (char *)malloc(16384);

  sprintf(command, "./%s", filename);

  FILE *fp = popen(command, "r");
  if (fp == NULL) {
    send_404(sock);
    return;
  }

  while (fgets(output, sizeof(output), fp) != NULL) {
    strcat(all_output, output);
  }

  info.body = all_output;
  info.size = strlen(info.body);
  strcpy(info.type, "text/html");

  send_200(sock, &info);

  pclose(fp);
  free(command);

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

void handle_form(int sock, char *body) {
  char *name = strstr(body, "name=");
  if (name == NULL) {
    send_404(sock);
    return;
  }
  printf("name: %s\n", name);

  char *return_url = (char *)malloc(strlen("/done?name=") + strlen(name) + 1);

  sprintf(return_url, "/done?name=%s", name + 5);

  send_303(sock, return_url);

  return;
}
