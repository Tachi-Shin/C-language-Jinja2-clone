#ifndef HTTP_TYPES_H
#define HTTP_TYPES_H

#include <netinet/in.h>

#define MAX_BUFFER 8192
#define MAX_COOKIES 10

typedef struct {
    char key[64];
    char value[128];
} Cookie;

typedef struct {
    int client_socket;
    struct sockaddr_in client_addr;
    char method[8];      // GET, POST etc
    char path[1024];     // /index, /login etc
    char body[MAX_BUFFER];
    Cookie cookies[MAX_COOKIES];
    int cookie_count;
} Request;

typedef struct {
    char body[MAX_BUFFER];
    char content_type[64];
    int status_code;
    Cookie set_cookies[MAX_COOKIES];
    int set_cookie_count;
    
    int redirect_seconds;   // 0なら無効
    char redirect_url[256]; // 飛び先
} Response;

// ハンドラ関数の型定義 (Flaskのルート関数)
typedef void (*RouteHandler)(Request *req, Response *res);

#endif