#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef AUTH_H
#define AUTH_H

int base64_encode(const char *src, char *dst);
int base64_decode(const char *src, char *dst);
int basic_auth(const char *auth, char *user, char *pass);
int check_auth(const char *auth, int auth_kind);

#endif
