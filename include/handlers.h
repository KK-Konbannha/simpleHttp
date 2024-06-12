void handle_index(int sock);
void handle_test(int sock);
void handle_test_cgi(int sock);
void handle_new(int sock);
void handle_done(int sock);
void handle_api(int sock, const char *path);
void handle_static(int sock, const char *path);
void handle_form(int sock, char *body);
