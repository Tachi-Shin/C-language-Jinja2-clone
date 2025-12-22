#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server.h"
#include "http_types.h"
#include "router.h"
#include "session.h"
#include "utils.h"

// データが完全に届くまで待機するロジックを追加
void handle_client(int client_sock, struct sockaddr_in client_addr) {
    char buffer[MAX_BUFFER];
    Request req = {0};
    Response res = {0};
    
    // デフォルト設定
    res.status_code = 200;
    strcpy(res.content_type, "text/html; charset=UTF-8");

    req.client_socket = client_sock;
    req.client_addr = client_addr;

    // --- 1. まずヘッダ（と一部のボディ）を受信 ---
    int total_received = recv(client_sock, buffer, MAX_BUFFER - 1, 0);
    if (total_received <= 0) {
        close(client_sock);
        return;
    }
    buffer[total_received] = '\0';

    // --- 2. POSTデータ（Body）の読み込み漏れ対策 ---
    // Content-Length を探して、データが足りているかチェックする
    int content_length = 0;
    char *cl_ptr = strstr(buffer, "Content-Length: ");
    if (cl_ptr) {
        content_length = atoi(cl_ptr + 16);
    }

    // ヘッダとボディの境界 (\r\n\r\n) を探す
    char *body_start = strstr(buffer, "\r\n\r\n");
    if (body_start) {
        // ヘッダ部分の長さ（\r\n\r\n含む）
        int header_len = (body_start - buffer) + 4;
        // すでに受信済みのボディの長さ
        int body_received = total_received - header_len;

        // まだボディが全部届いていない場合、ループして続きを読み込む
        while (body_received < content_length && total_received < MAX_BUFFER - 1) {
            int r = recv(client_sock, buffer + total_received, MAX_BUFFER - 1 - total_received, 0);
            if (r <= 0) break; // エラーまたは切断
            
            total_received += r;
            buffer[total_received] = '\0';
            body_received += r;
        }

        // ボディをRequest構造体にコピー
        // body_startポインタが古くなっている可能性があるので再取得
        body_start = strstr(buffer, "\r\n\r\n");
        if (body_start && strlen(body_start + 4) > 0) {
            strncpy(req.body, body_start + 4, MAX_BUFFER - 1);
        } else {
            req.body[0] = '\0';
        }
    } else {
        req.body[0] = '\0';
    }

    // --- 3. HTTPリクエストラインのパース ---
    sscanf(buffer, "%s %s", req.method, req.path);
    
    // --- 4. Cookieのパース ---
    parse_cookies(buffer, &req);

    // --- 5. ルーティングとハンドラ実行 ---
    RouteHandler handler = get_route(req.method, req.path);
    if (handler) {
        handler(&req, &res);
    } else {
        res.status_code = 404;
        strcpy(res.body, "<h1>404 Not Found</h1>");
    }

    // --- 6. レスポンス送信 ---
    char header[MAX_BUFFER];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %lu\r\n",
        res.status_code, res.content_type, strlen(res.body)
    );

    // 遅延リダイレクト
    if (res.redirect_seconds > 0) {
        char refresh_header[512];
        snprintf(refresh_header, sizeof(refresh_header), "Refresh: %d; url=%s\r\n", res.redirect_seconds, res.redirect_url);
        strncat(header, refresh_header, sizeof(header) - strlen(header) - 1);
    }

    // Set-Cookie
    for(int i=0; i<res.set_cookie_count; i++){
        char cookie_line[256];
        snprintf(cookie_line, sizeof(cookie_line), "Set-Cookie: %s=%s; Path=/\r\n", res.set_cookies[i].key, res.set_cookies[i].value);
        strncat(header, cookie_line, sizeof(header) - strlen(header) - 1);
    }

    strcat(header, "\r\n");

    send(client_sock, header, strlen(header), 0);
    send(client_sock, res.body, strlen(res.body), 0);
    close(client_sock);
}

void run_server(int port) {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int opt = 1;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    listen(server_sock, 10);
    printf("Server running on port %d...\n", port);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) continue;
        
        handle_client(client_sock, client_addr);
    }
    close(server_sock);
}