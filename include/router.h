#ifndef ROUTER_H
#define ROUTER_H

void route_request(int sock, const char *path, const char *method, char *body);
void route_get_request(int sock, const char *path);
void route_post_request(int sock, const char *path, char *body);

#endif
