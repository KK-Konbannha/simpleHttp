typedef struct {
  char cmd[64];
  char path[256];
  char version[64];
  char real_path[512];
  char type[64];
  char auth[256];
  int code;
  int size;
  char date[64];
} info_type;

int exp1_tcp_listen(int port);

int http_session(int sock);

void check_file(info_type *info);
void send_file(int sock, char *filename);
