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

  if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
    // Handle the root path
    handle_index(sock);
  } else if (strcmp(path, "/test.html") == 0) {
    // Handle the test path
    handle_test(sock);
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
