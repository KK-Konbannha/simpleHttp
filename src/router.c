#include "../include/router.h"
#include "../include/exp1.h"
#include "../include/handlers.h"
#include "../include/sendStatus.h"

void route_get_request(int sock, const char *path) {
  if (strstr(path, "..") != NULL || strstr(path, "//") != NULL ||
      strstr(path, "~") != NULL) {
    // Handle the 400 path
    // 400 Bad Request
    send_400(sock);

    return;
  }

  FILE *conf_file = fopen("redirecting.csv", "r");
  if (conf_file == NULL) {
    // Handle the 500 path
    // 500 Internal Server Error
    perror("Error opening conf file");
    send_500(sock);

    return;
  }

  char line[1024];
  char *token;
  char *code;
  char *old_path;
  char *new_path;

  while (fgets(line, sizeof(line), conf_file)) {
    token = strtok(line, ",");
    code = token;
    token = strtok(NULL, ",");
    old_path = token;
    token = strtok(NULL, ",");
    new_path = token;

    if (strcmp(path, old_path) == 0) {
      if (strcmp(code, "301") == 0) {
        // Handle the 301 path
        // 301 Moved Permanently
        send_301(sock, new_path);
      } else if (strcmp(code, "302") == 0) {
        // Handle the 302 path
        // 302 Found
        send_302(sock, new_path);
      } else {
        continue;
      }

      return;
    }
  }

  if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
    handle_index(sock);
  } else if (strcmp(path, "/test.html") == 0) {
    handle_test(sock);
  } else if (strcmp(path, "/test_cgi") == 0) {
    handle_test_cgi(sock);
  } else if (strcmp(path, "/moved") == 0) {
    // dummy function
    handle_index(sock);
  } else if (strcmp(path, "/maintenance") == 0) {
    // dummy function
    handle_index(sock);
  } else if (strcmp(path, "/form") == 0) {
    // dummy function
    handle_index(sock);
  } else if (strcmp(path, "/new") == 0) {
    handle_new(sock);
  } else if (strncmp(path, "/done", 5) == 0) {
    handle_done(sock);
  } else if (strncmp(path, "/api/", 5) == 0) {
    handle_api(sock, path);
  } else if (strncmp(path, "/static/", 8) == 0) {
    handle_static(sock, path);
  } else {
    send_404(sock);

    return;
  }

  return;
}

void route_post_request(int sock, const char *path, char *body) {
  if (strstr(path, "..") != NULL || strstr(path, "//") != NULL ||
      strstr(path, "~") != NULL) {
    // Handle the 400 path
    // 400 Bad Request
    send_400(sock);

    return;
  }

  if (strcmp(path, "/form") == 0) {
    handle_form(sock, body);
  } else {
    send_404(sock);

    return;
  }

  return;
}
