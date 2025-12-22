#ifndef ROUTER_H
#define ROUTER_H
#include "http_types.h"

void add_route(const char *method, const char *path, RouteHandler handler);
RouteHandler get_route(const char *method, const char *path);

#endif