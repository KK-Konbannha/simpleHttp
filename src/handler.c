#include "../include/handler.h"

void handle_index(int sock, info_type *info, return_info_t *return_info) {

  FILE *fp = fopen("public/index.html", "r");
  if (fp == NULL) {
    return_info->code = 404;
    return;
  }

  get_file_info(fp, return_info);
  strcpy(return_info->type, "text/html");

  return_info->code = 200;

  fclose(fp);

  return;
}

void handle_test(int sock, info_type *info, return_info_t *return_info) {

  FILE *fp = fopen("public/test.html", "r");
  if (fp == NULL) {
    return_info->code = 404;
    return;
  }

  get_file_info(fp, return_info);
  strcpy(return_info->type, "text/html");

  return_info->code = 200;

  fclose(fp);

  return;
}

void handle_test_cgi(int sock, info_type *info, return_info_t *return_info) {
  char const *filename = "public/test_cgi.cgi";
  char *command = (char *)malloc(strlen(filename) + strlen("./") + 1);
  char output[1024] = "";
  char *all_output = (char *)malloc(16384);

  sprintf(command, "./%s", filename);

  FILE *fp = popen(command, "r");
  if (fp == NULL) {
    return_info->code = 404;
    free(command);
    free(all_output);
    return;
  }

  while (fgets(output, sizeof(output), fp) != NULL) {
    strcat(all_output, output);
  }

  return_info->body = all_output;
  return_info->size = strlen(return_info->body);
  strcpy(return_info->type, "text/html");

  return_info->code = 200;

  pclose(fp);
  free(command);

  return;
}

void handle_new(int sock, info_type *info, return_info_t *return_info) {

  FILE *fp = fopen("public/new.html", "r");
  if (fp == NULL) {
    return_info->code = 404;
    return;
  }

  get_file_info(fp, return_info);
  strcpy(return_info->type, "text/html");

  return_info->code = 200;

  fclose(fp);

  return;
}

void handle_done(int sock, info_type *info, return_info_t *return_info) {

  FILE *fp = fopen("public/done.html", "r");
  if (fp == NULL) {
    return_info->code = 404;
    return;
  }

  get_file_info(fp, return_info);
  strcpy(return_info->type, "text/html");

  return_info->code = 200;

  fclose(fp);

  return;
}

void handle_api(int sock, info_type *info, return_info_t *return_info) {
  if (strcmp(info->path, "/api/echo") == 0) {
    // Handle the echo api
    return_info->body = (char *)malloc(strlen("Echo API\r\n") + 1);
    strcpy(return_info->body, "Echo API\r\n");
    return_info->size = strlen(return_info->body);
    strcpy(return_info->type, "text/plain");

    return_info->code = 200;
  } else if (strcmp(info->path, "/api/date") == 0) {
    // Handle the date api
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char buf[64];
    strftime(buf, sizeof(buf), "{\"date\": \"%Y-%m-%d %H:%M:%S\"}\n", tm);

    return_info->body = (char *)malloc(strlen(buf) + 1);
    strcpy(return_info->body, buf);
    return_info->size = strlen(return_info->body);
    strcpy(return_info->type, "application/json");

    return_info->code = 200;
  } else if (strcmp(info->path, "/api/count") == 0) {
    // Handle the count api
    int count = 0;
    char buf[64];
    sprintf(buf, "{\"count\": %d}\n", count);

    return_info->body = (char *)malloc(strlen(buf) + 1);
    strcpy(return_info->body, buf);
    return_info->size = strlen(return_info->body);
    strcpy(return_info->type, "application/json");

    return_info->code = 200;
  } else {
    // Handle the 404 API
    return_info->code = 404;
  }

  return;
}

void handle_static(int sock, info_type *info, return_info_t *return_info) {

  char *path_copy = (char *)malloc(strlen(info->path) + 1);
  strcpy(path_copy, info->path);
  char *ext = strrchr(path_copy, '.');
  if (ext == NULL) {
    return_info->code = 404;
    return;
  }

  if (strncmp(info->path, "/static/img/", 12) == 0) {
    if (strcmp(ext, ".png") == 0) {
      strcpy(return_info->type, "image/png");
    } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
      strcpy(return_info->type, "image/jpeg");
    } else if (strcmp(ext, ".gif") == 0) {
      strcpy(return_info->type, "image/gif");
    } else if (strcmp(ext, ".svg") == 0) {
      strcpy(return_info->type, "image/svg+xml");
    } else if (strcmp(ext, ".tiff") == 0) {
      strcpy(return_info->type, "image/tiff");
    } else if (strcmp(ext, ".bmp") == 0) {
      strcpy(return_info->type, "image/bmp");
    } else if (strcmp(ext, ".avif") == 0) {
      strcpy(return_info->type, "image/avif");
    } else if (strcmp(ext, ".webp") == 0) {
      strcpy(return_info->type, "image/webp");
    } else {
      return_info->code = 404;
      return;
    }

    free(path_copy);

    char *filename = (char *)malloc(strlen(info->path) + 8);
    sprintf(filename, "public%s", info->path);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
      return_info->code = 404;
      return;
    }

    get_file_info(fp, return_info);
    return_info->code = 200;

    fclose(fp);

  } else if (strncmp(info->path, "/static/video/", 14) == 0) {
    if (strcmp(ext, ".mp4") == 0) {
      strcpy(return_info->type, "video/mp4");
    } else {
      return_info->code = 404;
      return;
    }

    free(path_copy);

    char *filename = (char *)malloc(strlen(info->path) + 8);
    sprintf(filename, "public%s", info->path);
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
      return_info->code = 404;
      return;
    }

    get_file_info(fp, return_info);
    return_info->code = 200;

    fclose(fp);

  } else if (strncmp(info->path, "/static/css/", 12) == 0) {
    if (strcmp(ext, ".css") != 0) {
      return_info->code = 404;
      return;
    }

    free(path_copy);

    char *filename = (char *)malloc(strlen(info->path) + 8);
    sprintf(filename, "public%s", info->path);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
      return_info->code = 404;
      return;
    }

    get_file_info(fp, return_info);
    strcpy(return_info->type, "text/css");

    return_info->code = 200;

    fclose(fp);

  } else if (strncmp(info->path, "/static/js/", 11) == 0 &&
             strcmp(ext, ".js") == 0) {
    strcpy(return_info->type, "application/javascript");
    char *filename = (char *)malloc(strlen(info->path) + 8);
    sprintf(filename, "public%s", info->path);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
      return_info->code = 404;
      return;
    }

    get_file_info(fp, return_info);
    return_info->code = 200;

    fclose(fp);

  } else {
    return_info->code = 404;
    return;
  }

  return;
}

void handle_form(int sock, info_type *info, return_info_t *return_info) {
  char *name = "";
  name = strstr(info->body, "name=");
  if (name == NULL) {
    return_info->code = 404;
    return;
  }

  char *return_url = (char *)malloc(strlen("/done?name=") + strlen(name) + 1);

  sprintf(return_url, "/done?name=%s", name + 5);
  strcpy(return_info->new_path, return_url);

  return_info->code = 303;

  return;
}
