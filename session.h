#ifndef SESSION_H
#define SESSION_H
#include "http_types.h"

void parse_cookies(const char *header, Request *req);
void set_cookie(Response *res, const char *key, const char *value);
void delete_cookie(Response *res, const char *key);

#endif