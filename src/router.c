#include "../include/router.h"
#include "../include/handler.h"
#include "../include/send_status.h"

void route_request(int sock, info_type *info, return_info_t *return_info) {
  if (strcmp(info->method, "GET") == 0 || strcmp(info->method, "HEAD") == 0) {
    route_get_request(sock, info, return_info);
  } else if (strcmp(info->method, "POST") == 0) {
    route_post_request(sock, info, return_info);
  } else {
    // Handle the 501 path
    // 501 Not Implemented
    return_info->code = 501;
    return;
  }

  return;
}

void route_get_request(int sock, info_type *info, return_info_t *return_info) {

  if (strstr(info->path, "..") != NULL || strstr(info->path, "//") != NULL ||
      strstr(info->path, "~") != NULL) {
    // Handle the 400 info->path
    // 400 Bad Request
    return_info->code = 400;

    return;
  }

  char line[1024] = "";
  char *token = NULL, *code = NULL, *rejection_path = NULL, *old_path = NULL,
       *new_path = NULL, *saveptr = NULL;

  FILE *rejection_file = fopen("rejection.csv", "r");
  if (rejection_file == NULL) {
    // Handle the 500 path
    // 500 Internal Server Error
    perror("Error opening rejection file");
    return_info->code = 500;

    return;
  }

  while (fgets(line, sizeof(line), rejection_file)) {
    token = strtok_r(line, ",", &saveptr);
    code = token;
    token = strtok_r(NULL, ",", &saveptr);
    rejection_path = token;
    rejection_path[strlen(rejection_path) - 1] = '\0';

    if (strcmp(info->path, rejection_path) == 0) {
      if (strcmp(code, "403") == 0) {
        // Handle the 403 path
        // 403 Forbidden
        return_info->code = 403;
      } else if (strcmp(code, "404") == 0) {
        // Handle the 404 path
        // 404 Not Found
        return_info->code = 404;
      } else if (strcmp(code, "418") == 0) {
        // Handle the 418 path
        // 418 I'm a teapot
        return_info->code = 418;
      } else {
        continue;
      }

      fclose(rejection_file);
      return;
    }
  }
  fclose(rejection_file);

  strcpy(line, "");
  token = NULL, code = NULL;

  FILE *conf_file = fopen("redirecting.csv", "r");
  if (conf_file == NULL) {
    // Handle the 500 path
    // 500 Internal Server Error
    perror("Error opening conf file");
    return_info->code = 500;

    return;
  }

  *saveptr = '\0';
  while (fgets(line, sizeof(line), conf_file)) {
    token = strtok_r(line, ",", &saveptr);
    code = token;
    token = strtok_r(NULL, ",", &saveptr);
    old_path = token;
    token = strtok_r(NULL, ",", &saveptr);
    new_path = token;

    if (strcmp(info->path, old_path) == 0) {
      if (strcmp(code, "301") == 0) {
        // Handle the 301 path
        // 301 Moved Permanently
        return_info->code = 301;
      } else if (strcmp(code, "302") == 0) {
        // Handle the 302 path
        // 302 Found
        return_info->code = 302;
      } else if (strcmp(code, "303") == 0) {
        // Handle the 303 path
        // 303 See Other
        return_info->code = 303;
      } else {
        continue;
      }

      strcpy(return_info->new_path, new_path);
      fclose(conf_file);
      return;
    }
  }
  fclose(conf_file);

  if (strcmp(info->path, "/") == 0 || strcmp(info->path, "/index.html") == 0) {
    handle_index(sock, info, return_info);
  } else if (strcmp(info->path, "/test.html") == 0) {
    handle_test(sock, info, return_info);
  } else if (strcmp(info->path, "/test-cgi") == 0) {
    handle_test_cgi(sock, info, return_info);
  } else if (strcmp(info->path, "/moved") == 0) {
    // dummy function
    handle_index(sock, info, return_info);
  } else if (strcmp(info->path, "/found") == 0) {
    // dummy function
    handle_index(sock, info, return_info);
  } else if (strcmp(info->path, "/see-other") == 0) {
    // dummy function
    handle_index(sock, info, return_info);
  } else if (strcmp(info->path, "/form") == 0) {
    // dummy function
    handle_index(sock, info, return_info);
  } else if (strcmp(info->path, "/new") == 0) {
    handle_new(sock, info, return_info);
  } else if (strncmp(info->path, "/done", 5) == 0) {
    handle_done(sock, info, return_info);
  } else if (strncmp(info->path, "/api/", 5) == 0) {
    handle_api(sock, info, return_info);
  } else if (strncmp(info->path, "/static/", 8) == 0) {
    handle_static(sock, info, return_info);
  } else {
    return_info->code = 404;

    return;
  }

  return;
}

void route_post_request(int sock, info_type *info, return_info_t *return_info) {
  if (strstr(info->path, "..") != NULL || strstr(info->path, "//") != NULL ||
      strstr(info->path, "~") != NULL) {
    // Handle the 400 path
    // 400 Bad Request
    return_info->code = 400;

    return;
  }

  if (strcmp(info->path, "/form") == 0) {
    handle_form(sock, info, return_info);
  } else {
    return_info->code = 404;

    return;
  }

  return;
}
