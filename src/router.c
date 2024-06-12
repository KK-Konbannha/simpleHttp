#include "../include/router.h"
#include "../include/exp1.h"
#include "../include/handlers.h"
#include "../include/sendStatus.h"

void route_request(int sock, const char *path) {
  if (strstr(path, "..") != NULL || strstr(path, "//") != NULL ||
      strstr(path, "~") != NULL) {
    // Handle the 400 path
    // 400 Bad Request
    send_400(sock);

    return;
  }

  FILE *conf_file = fopen("server.conf", "r");
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
      } else if (strcmp(code, "303") == 0) {
        // Handle the 303 path
        // 303 See Other
        send_303(sock, new_path);
      } else {
        continue;
      }

      return;
    }
  }

  if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
    // Handle the root path
    handle_index(sock);
  } else if (strcmp(path, "/test.html") == 0) {
    // Handle the test path
    handle_test(sock);
  } else if (strcmp(path, "/moved") == 0) {
    // Handle the moved path
    // dummy function
    handle_index(sock);
  } else if (strcmp(path, "/maintenance") == 0) {
    // Handle the maintenance path
    // dummy function
    handle_index(sock);
  } else if (strcmp(path, "/from") == 0) {
    // Handle the from path
    // dummy function
    handle_index(sock);
  } else if (strcmp(path, "/new") == 0) {
    // Handle the new path
    handle_new(sock);
  } else if (strcmp(path, "/done") == 0) {
    // Handle the done path
    handle_done(sock);
  } else if (strncmp(path, "/api/", 5) == 0) {
    // Handle the API path
    handle_api(sock, path);
  } else if (strncmp(path, "/static/", 8) == 0) {
    // Handle the static path
    handle_static(sock, path);
  } else {
    // Handle the 404 path
    send_404(sock);

    return;
  }

  return;
}
