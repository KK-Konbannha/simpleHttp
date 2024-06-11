#include "../include/router.h"
#include "../include/exp1.h"

void route_request(const char *path) {
  if (strcmp(path, "/") == 0) {
    // Handle the root path
  } else if (strcmp(path, "/about") == 0) {
    // Handle the about path
  } else if (strcmp(path, "/contact") == 0) {
    // Handle the contact path
  } else {
    // Handle the 404 path
  }
}
