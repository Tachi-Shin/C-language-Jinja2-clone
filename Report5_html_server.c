#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "server.h"
#include "router.h"
#include "template.h"
#include "session.h"
#include "utils.h"

// ルートハンドラ: /
void index_view(Request *req, Response *res) {
    (void)req; // 警告消去: reqを使わないことを明示
    render_template("index.html", res->body, sizeof(res->body), "get_data", "Hello from C-Flask!");
}

// REST APIハンドラ: /api/data
void api_data_view(Request *req, Response *res) {
    (void)req;
    strcpy(res->content_type, "application/json");
    strcpy(res->body, "{\"message\": \"This is a JSON response\", \"status\": \"success\"}");
}

// ログイン (Cookieセット): /login
void login_view(Request *req, Response *res) {
    (void)req;
    set_cookie(res, "session_id", "12345abcde");
    strcpy(res->body, "<h1>Logged in!</h1><p>Cookie has been set.</p><a href='/'>Top</a>");
}

// GET /form : 入力フォームを表示
void form_view(Request *req, Response *res) {
    (void)req; // 警告消去
    strcpy(res->body, 
        "<h1>Message Board</h1>"
        "<form action='/form' method='POST'>"
        "Name: <input type='text' name='username'><br>"
        "Message: <input type='text' name='message'><br>"
        "<button type='submit'>Send</button>"
        "</form>");
}

// POST /form : データの受け取り
void form_handler(Request *req, Response *res) {
    char username[64];
    char message[256];

    // ボディからデータを取得
    get_form_data(req->body, "username", username, sizeof(username));
    get_form_data(req->body, "message", message, sizeof(message));

    // 結果を表示
    snprintf(res->body, sizeof(res->body),
        "<h1>Received!</h1>"
        "<p>User: %s</p>"
        "<p>Message: %s</p>"
        "<a href='/'>Back to Top</a>",
        username, message
    );
}

// GET /logout : Cookieを削除
void logout_view(Request *req, Response *res) {
    (void)req;
    delete_cookie(res, "session_id");
    
    strcpy(res->body, "<h1>Logged out</h1><p>Cookie has been deleted.</p><a href='/'>Top</a>");
}

// GET /wait : 3秒後にトップへリダイレクト
void delayed_redirect_view(Request *req, Response *res) {
    (void)req;
    res->redirect_seconds = 3;
    strcpy(res->redirect_url, "/"); // トップページへ

    strcpy(res->body, 
        "<h1>Redirecting...</h1>"
        "<p>You will be redirected to the top page in 3 seconds.</p>");
}

int main(void) {
    // 既存のルート登録
    add_route("GET", "/", index_view);
    add_route("GET", "/index", index_view);
    add_route("GET", "/api/data", api_data_view);
    add_route("GET", "/login", login_view);

    // 新機能のルート登録
    add_route("GET", "/form", form_view);
    add_route("POST", "/form", form_handler);
    add_route("GET", "/logout", logout_view);
    add_route("GET", "/wait", delayed_redirect_view);

    // サーバー起動
    run_server(8080);
    return 0;
}