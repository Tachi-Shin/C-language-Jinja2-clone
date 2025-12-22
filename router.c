#include <string.h>
#include "router.h"

typedef struct {
    char method[8];
    char path[1024];
    RouteHandler handler;
} Route;

static Route routes[50];
static int route_count = 0;

void add_route(const char *method, const char *path, RouteHandler handler) {
    if (route_count < 50) {
        strncpy(routes[route_count].method, method, 7);
        strncpy(routes[route_count].path, path, 1023);
        routes[route_count].handler = handler;
        route_count++;
    }
}

RouteHandler get_route(const char *method, const char *path) {
    for (int i = 0; i < route_count; i++) {
        if (strcmp(routes[i].method, method) == 0 &&
            strcmp(routes[i].path, path) == 0) {
            return routes[i].handler;
        }
    }
    return NULL;
}