typedef struct {
  char method[8];
  char path[256];
  char version[10];
  char real_path[512];
  char type[128];
  char auth[1024];
  int code;
  int size;
  char date[64];
} info_type;

int exp1_tcp_listen(int port);

int http_session(int sock);

void check_file(info_type *info);
void send_file(int sock, char *filename);
